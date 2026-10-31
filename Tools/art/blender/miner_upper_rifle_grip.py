"""Miner upper rifle V2Grip (task-409-02): A_Miner_UpperRifle_V2Grip from A_Miner_UpperRifle_V2.fbx.

Usage (headless Blender):
  blender -b --factory-startup -P miner_upper_rifle_grip.py -- build <anims_dir> <report.json>
  blender -b --factory-startup -P miner_upper_rifle_grip.py -- verify <anims_dir> <report.json>

Method: on every frame the V2 grip point G = WeaponSocket_R (on RightHand) applied to the RifleMesh offset
(9.898091, -1.129037, -13.0) cm is computed; the right hand is translated so that the socket origin lands on G
(hand rotation kept, fingers untouched in local space). RightArm and RightForeArm are then solved as a two bone
chain (bone lengths kept): hand head pinned, elbow on the circle of possible elbows at the V2 elbow height
(the one nearest the V2 elbow; clamped to the nearest reachable height when the arm cannot keep it).

Unreal bone space (socket) to Blender bone frame: Y flipped (UE left-handed). Checked on A_Miner_Idle frame 0:
the socket origin lands on the midpoint RightHand head / RightHandMiddle1 head (0.00002 cm), as bp-miner derived it.
"""
import json
import math
import os
import sys

import bpy

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rr_blender as rr  # noqa: E402
from mathutils import Matrix, Vector  # noqa: E402

P = "mixamorig:"
CHAIN = [P + "RightArm", P + "RightForeArm", P + "RightHand"]
SRC, DST = "A_Miner_UpperRifle_V2.fbx", "A_Miner_UpperRifle_V2Grip.fbx"
SOCK_LOC = Vector((0.005868, -0.065782, -0.001565))     # UE bone space, bone scale 100 x socket scale 0.01 -> metres
SOCK_ROT = (-17.502942, -79.928061, 17.606433)          # pitch, yaw, roll
OFFSET_CM = Vector((9.898091, -1.129037, -13.0))        # RifleMesh location in the socket frame
FLIP_Y = Matrix.Diagonal((1, -1, 1))
EXPORT_KW = dict(apply_scale_options="FBX_SCALE_UNITS", add_leaf_bones=False, use_selection=True,
                 mesh_smooth_type="FACE", axis_forward="-Z", axis_up="Y", path_mode="COPY", embed_textures=False)


def ue_rot(p, y, r):
    p, y, r = map(math.radians, (p, y, r))
    cp, sp, cy, sy, cr, sr = math.cos(p), math.sin(p), math.cos(y), math.sin(y), math.cos(r), math.sin(r)
    X = Vector((cp * cy, cp * sy, sp))
    Y = Vector((sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp))
    Z = Vector((-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp))
    return Matrix((X, Y, Z)).transposed()


RS = ue_rot(*SOCK_ROT)


def load(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    rr.import_fbx(path)
    arm = [o for o in bpy.data.objects if o.type == "ARMATURE"][0]
    act = bpy.data.actions[0]
    if arm.animation_data is None or arm.animation_data.action is None:
        arm.animation_data_create()
        arm.animation_data.action = act
        arm.animation_data.action_slot = act.slots[0]
    return arm, act


def setf(f):
    bpy.context.scene.frame_set(f)
    bpy.context.view_layer.update()


def frange(act):
    return int(act.frame_range[0]), int(act.frame_range[1])


def hand_frame(arm):
    """World: hand head, hand bone rotation (3x3), socket origin, socket x axis, grip target G (metres)."""
    W = arm.matrix_world
    pb = arm.pose.bones[CHAIN[2]]
    H = W @ pb.head
    Rb = (W @ pb.matrix).to_3x3().normalized()
    B = Rb @ FLIP_Y
    org = H + B @ SOCK_LOC
    G = org + B @ RS @ OFFSET_CM * 0.01
    return H, Rb, org, B @ RS.col[0], G


def arc(a, b):
    return a.rotation_difference(b).to_matrix().to_4x4()


def solve_elbow(S, E0, H0, Hn):
    """Return (E_new, height gap m, reach ok) for the pinned hand Hn."""
    a, b = (E0 - S).length, (H0 - E0).length
    d = Hn - S
    D = d.length
    if D > a + b - 1e-6:
        return None, 0.0, False
    u = d / D
    x = (a * a - b * b + D * D) / (2 * D)
    C = S + u * x
    rho = math.sqrt(max(a * a - x * x, 0.0))
    z = Vector((0, 0, 1))
    e1 = z - u * u.dot(z)
    k = e1.length
    e1.normalize()
    e2 = u.cross(e1)
    c = (E0.z - C.z) / (rho * k)
    gap = 0.0
    if abs(c) > 1:
        gap = abs(E0.z - (C.z + rho * k * math.copysign(1, c)))
        c = math.copysign(1.0, c)
    phi = math.acos(c)
    cands = [C + rho * (math.cos(s * phi) * e1 + math.sin(s * phi) * e2) for s in (1, -1)]
    return min(cands, key=lambda p: (p - E0).length), gap, True


def key_chain(arm, f):
    for n in CHAIN:
        pbn = arm.pose.bones[n]
        pbn.keyframe_insert("rotation_quaternion" if pbn.rotation_mode == "QUATERNION" else "rotation_euler", frame=f)
        pbn.keyframe_insert("location", frame=f)
        pbn.keyframe_insert("scale", frame=f)


def build(anims, report):
    arm, act = load(os.path.join(anims, SRC))
    f0, f1 = frange(act)
    fps = bpy.context.scene.render.fps
    pb = arm.pose.bones
    W = arm.matrix_world
    inv = W.inverted()
    rows, bad = [], []
    for f in range(f0, f1 + 1):
        setf(f)
        S, E0, H0 = (W @ pb[n].head for n in CHAIN)
        Hh, Rb, org, sx, G = hand_frame(arm)
        Hn = Hh + (G - org)
        old = {n: W @ pb[n].matrix for n in CHAIN}
        En, gap, ok = solve_elbow(S, E0, H0, Hn)
        move = (G - org).length * 100
        if not ok:
            bad.append((f, round((Hn - S).length * 100, 2), round(((E0 - S).length + (H0 - E0).length) * 100, 2)))
            continue
        # upper arm: shortest arc about S, old direction to new direction
        Ta = Matrix.Translation(S) @ arc((E0 - S).normalized(), (En - S).normalized()) @ Matrix.Translation(-S)
        pb[CHAIN[0]].matrix = inv @ (Ta @ old[CHAIN[0]])
        bpy.context.view_layer.update()
        # forearm: carried by the arm, then shortest arc about the new elbow onto the pinned hand
        F1 = Ta @ old[CHAIN[1]]
        d1 = (Ta.to_3x3() @ (H0 - E0)).normalized()
        Tf = Matrix.Translation(En) @ arc(d1, (Hn - En).normalized()) @ Matrix.Translation(-En)
        pb[CHAIN[1]].matrix = inv @ (Tf @ F1)
        bpy.context.view_layer.update()
        # hand: V2 world orientation, new head, original bone scale
        sc = old[CHAIN[2]].to_scale()
        Mh = Matrix.Translation(Hn) @ Rb.to_4x4() @ Matrix.Diagonal((sc.x, sc.y, sc.z, 1))
        pb[CHAIN[2]].matrix = inv @ Mh
        bpy.context.view_layer.update()
        key_chain(arm, f)
        rows.append({"frame": f, "hand_move_cm": round(move, 3), "elbow_gap_cm": round(gap * 100, 3)})
    if bad:
        print("UNREACHABLE", bad)
        with open(report, "w") as fh:
            json.dump({"unreachable_frames(frame, needed cm, max reach cm)": bad}, fh, indent=2)
        sys.exit(2)
    act.name = "A_Miner_UpperRifle_V2Grip"
    scn = bpy.context.scene
    scn.render.fps = fps
    scn.frame_start, scn.frame_end = f0, f1
    bpy.ops.object.select_all(action="DESELECT")
    arm.select_set(True)
    bpy.context.view_layer.objects.active = arm
    bpy.ops.export_scene.fbx(filepath=os.path.join(anims, DST), object_types={"ARMATURE"}, bake_anim=True,
                             bake_anim_use_all_actions=False, bake_anim_use_nla_strips=False,
                             bake_anim_simplify_factor=0.0, bake_anim_force_startend_keying=True, **EXPORT_KW)
    with open(report, "w") as fh:
        json.dump({"source": SRC, "frames": rows}, fh, indent=2)
    print("BUILD done", len(rows), "frames")


def dump(path):
    arm, act = load(path)
    f0, f1 = frange(act)
    W = arm.matrix_world
    info = {"frames": f1 - f0 + 1, "fps": bpy.context.scene.render.fps, "bones": [b.name for b in arm.data.bones]}
    out = []
    for f in range(f0, f1 + 1):
        setf(f)
        out.append({"hf": hand_frame(arm),
                    "mat": {b.name: W @ b.matrix for b in arm.pose.bones},
                    "head": {b.name: W @ b.head for b in arm.pose.bones},
                    "tail": {b.name: W @ b.tail for b in arm.pose.bones},
                    "basis": {b.name: b.matrix_basis.copy() for b in arm.pose.bones}})
    sub = set()
    for b in arm.data.bones:
        if b.name == P + "RightShoulder":
            sub = {c.name for c in b.children_recursive} | {b.name}
    info["right_arm_subtree"] = sub
    return info, out


def ang(a, b):
    return math.degrees(a.to_quaternion().rotation_difference(b.to_quaternion()).angle)


def verify(anims, report):
    with open(report) as fh:
        res = json.load(fh)
    i0, d0 = dump(os.path.join(anims, SRC))
    i1, d1 = dump(os.path.join(anims, DST))
    sub = i1["right_arm_subtree"]
    m = {"bones": len(i1["bones"]), "same_bones": i1["bones"] == i0["bones"], "frames": i1["frames"],
         "same_frames": i1["frames"] == i0["frames"], "fps": i1["fps"], "same_fps": i1["fps"] == i0["fps"],
         "socket_dist_to_G_cm_max": 0.0, "hand_rot_dev_deg_max": 0.0, "finger_and_other_local_dev_max": 0.0,
         "outside_right_arm_pos_dev_cm_max": 0.0, "outside_right_arm_rot_dev_deg_max": 0.0,
         "bone_length_dev_cm_max": 0.0}
    hand, fore = CHAIN[2], CHAIN[1]
    gaps = []
    for a, b in zip(d0, d1):
        G = a["hf"][4]
        m["socket_dist_to_G_cm_max"] = max(m["socket_dist_to_G_cm_max"], (b["hf"][2] - G).length * 100)
        m["hand_rot_dev_deg_max"] = max(m["hand_rot_dev_deg_max"], ang(a["mat"][hand], b["mat"][hand]))
        gaps.append(round((b["head"][fore].z - a["head"][fore].z) * 100, 3))
        for n in i1["bones"]:
            if n not in CHAIN:
                dv = max(abs(x - y) for ra, rb in zip(a["basis"][n], b["basis"][n]) for x, y in zip(ra, rb))
                m["finger_and_other_local_dev_max"] = max(m["finger_and_other_local_dev_max"], dv)
            if n not in sub:
                m["outside_right_arm_pos_dev_cm_max"] = max(m["outside_right_arm_pos_dev_cm_max"], (a["head"][n] - b["head"][n]).length * 100)
                m["outside_right_arm_rot_dev_deg_max"] = max(m["outside_right_arm_rot_dev_deg_max"], ang(a["mat"][n], b["mat"][n]))
            la, lb = (a["tail"][n] - a["head"][n]).length, (b["tail"][n] - b["head"][n]).length
            m["bone_length_dev_cm_max"] = max(m["bone_length_dev_cm_max"], abs(la - lb) * 100)
    m["elbow_height_dev_cm_min_max"] = [min(gaps), max(gaps)]
    m["elbow_frames_beyond_1cm"] = [[i, g] for i, g in enumerate(gaps) if abs(g) > 1.0]
    m["hand_move_cm_range"] = [min(r["hand_move_cm"] for r in res["frames"]), max(r["hand_move_cm"] for r in res["frames"])]
    for k in ("socket_dist_to_G_cm_max", "hand_rot_dev_deg_max", "outside_right_arm_pos_dev_cm_max",
              "outside_right_arm_rot_dev_deg_max", "bone_length_dev_cm_max", "finger_and_other_local_dev_max"):
        m[k] = round(m[k], 5)
    res["verify"] = m
    print("VERIFY", json.dumps(m))
    with open(report, "w") as fh:
        json.dump(res, fh, indent=2)


argv = sys.argv[sys.argv.index("--") + 1:]
{"build": build, "verify": verify}[argv[0]](*argv[1:3])
