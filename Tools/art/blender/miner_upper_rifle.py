"""Miner upper rifle variants (task-408-02): A_Miner_UpperRifle_V1..V3 from A_Miner_Idle.fbx.

Usage (headless Blender):
  blender -b --factory-startup -P miner_upper_rifle.py -- build <anims_dir> <report.json>   (reads A_Miner_Idle.fbx, writes V1..V3)
  blender -b --factory-startup -P miner_upper_rifle.py -- verify <anims_dir> <report.json>  (re-reads, compares with the idle)
  blender -b --factory-startup -P miner_upper_rifle.py -- render <anims_dir> <out_dir>      (side stills, first frame of each)

Method: on every frame, the right arm chain (RightArm, RightForeArm) is rotated rigidly about the
shoulder-to-hand axis (hand head stays fixed) by the smallest angle that raises the elbow (RightForeArm
head) by E cm in world Z; RightHand is put back to its idle world orientation (fingers follow). Baked as keys.
"""
import json
import math
import os
import sys

import bpy

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rr_blender as rr  # noqa: E402
from mathutils import Matrix, Vector  # noqa: E402

VARIANTS = [("V1", 3.0), ("V2", 6.0), ("V3", 9.0)]
P = "mixamorig:"
CHAIN = [P + "RightArm", P + "RightForeArm", P + "RightHand"]
EXPORT_KW = dict(apply_scale_options="FBX_SCALE_UNITS", add_leaf_bones=False, use_selection=True,
                 mesh_smooth_type="FACE", axis_forward="-Z", axis_up="Y", path_mode="COPY", embed_textures=False)


def arm_obj():
    return [o for o in bpy.data.objects if o.type == "ARMATURE"][0]


def load(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    rr.import_fbx(path)
    arm = arm_obj()
    act = bpy.data.actions[0]
    if arm.animation_data is None or arm.animation_data.action is None:
        arm.animation_data_create()
        arm.animation_data.action = act
        arm.animation_data.action_slot = act.slots[0]
    return arm, act


def frame_range(act):
    return int(act.frame_range[0]), int(act.frame_range[1])


def setf(f):
    bpy.context.scene.frame_set(f)
    bpy.context.view_layer.update()


def head(arm, name):
    return arm.matrix_world @ arm.pose.bones[name].head


def raise_frame(arm, E_m):
    """Rotate the arm chain about the shoulder-hand axis; return (angle rad, achieved raise m)."""
    pb = arm.pose.bones
    S, W = head(arm, CHAIN[0]), head(arm, CHAIN[2])
    El = head(arm, CHAIN[1])
    ax = (W - S).normalized()
    r = El - S
    r_perp = r - ax * r.dot(ax)
    t = ax.cross(r_perp)

    def dz(th):
        return (r_perp * (math.cos(th) - 1) + t * math.sin(th)).z
    step = math.radians(0.5)
    best = None
    for k in range(1, 361):
        for sgn in (1, -1):
            if dz(sgn * k * step) >= E_m:
                best = (sgn * (k - 1) * step, sgn * k * step)
                break
        if best:
            break
    if not best:
        return None, 0.0
    lo, hi = best
    for _ in range(40):
        mid = (lo + hi) / 2
        if dz(mid) >= E_m:
            hi = mid
        else:
            lo = mid
    th = hi
    Tr = Matrix.Translation(S) @ Matrix.Rotation(th, 4, ax) @ Matrix.Translation(-S)
    old = {n: arm.matrix_world @ pb[n].matrix for n in CHAIN}
    inv = arm.matrix_world.inverted()
    for n in CHAIN[:2]:
        pb[n].matrix = inv @ (Tr @ old[n])
        bpy.context.view_layer.update()
    pb[CHAIN[2]].matrix = inv @ old[CHAIN[2]]
    bpy.context.view_layer.update()
    return th, dz(th)


def make_variant(idle_path, out_path, name, E_cm):
    arm, act = load(idle_path)
    f0, f1 = frame_range(act)
    fps = bpy.context.scene.render.fps
    log = []
    for f in range(f0, f1 + 1):
        setf(f)
        th, _ = raise_frame(arm, E_cm / 100.0)
        assert th is not None, "elbow raise unreachable at frame %d" % f
        for n in CHAIN:
            pbn = arm.pose.bones[n]
            if pbn.rotation_mode == "QUATERNION":
                pbn.keyframe_insert("rotation_quaternion", frame=f)
            else:
                pbn.keyframe_insert("rotation_euler", frame=f)
        log.append(math.degrees(th))
    act.name = "A_Miner_UpperRifle_" + name
    bpy.context.scene.render.fps = fps
    bpy.context.scene.frame_start, bpy.context.scene.frame_end = f0, f1
    bpy.ops.object.select_all(action="DESELECT")
    arm.select_set(True)
    bpy.context.view_layer.objects.active = arm
    bpy.ops.export_scene.fbx(filepath=out_path, object_types={"ARMATURE"}, bake_anim=True,
                             bake_anim_use_all_actions=False, bake_anim_use_nla_strips=False,
                             bake_anim_simplify_factor=0.0, bake_anim_force_startend_keying=True, **EXPORT_KW)
    return {"E_cm": E_cm, "chain_rot_deg_min": round(min(log, key=abs), 2), "chain_rot_deg_max": round(max(log, key=abs), 2)}


def pose_dump(path):
    arm, act = load(path)
    f0, f1 = frame_range(act)
    info = {"frames": f1 - f0 + 1, "fps": bpy.context.scene.render.fps, "bones": [b.name for b in arm.data.bones]}
    data = []
    for f in range(f0, f1 + 1):
        setf(f)
        data.append({b.name: (arm.matrix_world @ b.matrix, arm.matrix_world @ b.head) for b in arm.pose.bones})
    return info, data


def build(anims, report):
    idle = os.path.join(anims, "A_Miner_Idle.fbx")
    res = {"source": "A_Miner_Idle.fbx", "variants": {}}
    for name, E in VARIANTS:
        res["variants"][name] = make_variant(idle, os.path.join(anims, "A_Miner_UpperRifle_%s.fbx" % name), name, E)
        print("BUILD", name, res["variants"][name])
    with open(report, "w") as f:
        json.dump(res, f, indent=2)


def ang(a, b):
    return math.degrees(a.to_quaternion().rotation_difference(b.to_quaternion()).angle)


def verify(anims, report):
    with open(report) as f:
        res = json.load(f)
    info0, d0 = pose_dump(os.path.join(anims, "A_Miner_Idle.fbx"))
    hand, fore = CHAIN[2], CHAIN[1]
    for name, E in VARIANTS:
        info, d = pose_dump(os.path.join(anims, "A_Miner_UpperRifle_%s.fbx" % name))
        m = {"bones": len(info["bones"]), "same_bones": info["bones"] == info0["bones"],
             "frames": info["frames"], "same_frames": info["frames"] == info0["frames"], "fps": info["fps"],
             "hand_dev_cm_max": 0.0, "elbow_raise_cm": [999, -999],
             "other_bones_pos_dev_cm_max": 0.0, "other_bones_rot_dev_deg_max": 0.0,
             "hips_horizontal_cm_max": 0.0, "hand_rot_dev_deg_max": 0.0}
        moved = set(CHAIN[:2])
        h0 = d[0][P + "Hips"][1]
        for a, b in zip(d0, d):
            m["hand_dev_cm_max"] = max(m["hand_dev_cm_max"], (a[hand][1] - b[hand][1]).length * 100)
            dz = (b[fore][1] - a[fore][1]).z * 100
            m["elbow_raise_cm"] = [min(m["elbow_raise_cm"][0], dz), max(m["elbow_raise_cm"][1], dz)]
            m["hand_rot_dev_deg_max"] = max(m["hand_rot_dev_deg_max"], ang(a[hand][0], b[hand][0]))
            hd = b[P + "Hips"][1] - h0
            m["hips_horizontal_cm_max"] = max(m["hips_horizontal_cm_max"], math.hypot(hd.x, hd.y) * 100)
            for n in info["bones"]:
                if n in moved:
                    continue
                m["other_bones_pos_dev_cm_max"] = max(m["other_bones_pos_dev_cm_max"], (a[n][1] - b[n][1]).length * 100)
                m["other_bones_rot_dev_deg_max"] = max(m["other_bones_rot_dev_deg_max"], ang(a[n][0], b[n][0]))
        for k in ("hand_dev_cm_max", "other_bones_pos_dev_cm_max", "other_bones_rot_dev_deg_max",
                  "hips_horizontal_cm_max", "hand_rot_dev_deg_max"):
            m[k] = round(m[k], 4)
        m["elbow_raise_cm"] = [round(x, 3) for x in m["elbow_raise_cm"]]
        res["variants"][name]["verify"] = m
        print("VERIFY", name, m)
    with open(report, "w") as f:
        json.dump(res, f, indent=2)


def render(anims, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    for name in ["Idle"] + ["UpperRifle_" + v for v, _ in VARIANTS]:
        arm, act = load(os.path.join(anims, "A_Miner_%s.fbx" % name))
        f0, _ = frame_range(act)
        setf(f0)
        cu = bpy.data.curves.new("sk", "CURVE")
        cu.dimensions = "3D"
        cu.bevel_depth = 0.006
        for b in arm.pose.bones:
            if b.parent:
                sp = cu.splines.new("POLY")
                sp.points.add(1)
                sp.points[0].co = (*(arm.matrix_world @ b.parent.head), 1)
                sp.points[1].co = (*(arm.matrix_world @ b.head), 1)
        bpy.context.collection.objects.link(bpy.data.objects.new("sk", cu))
        cam = bpy.data.cameras.new("c")
        cam.type = "ORTHO"
        cam.ortho_scale = 0.9
        co = bpy.data.objects.new("c", cam)
        bpy.context.collection.objects.link(co)
        ctr = head(arm, CHAIN[0])
        co.location = ctr + Vector((3, 0, -0.05))
        co.rotation_euler = (math.radians(90), 0, math.radians(90))
        sc = bpy.context.scene
        sc.camera = co
        sc.render.resolution_x = sc.render.resolution_y = 600
        sc.render.filepath = os.path.join(out_dir, "side_%s.png" % name)
        sc.render.engine = "BLENDER_WORKBENCH"
        bpy.ops.render.render(write_still=True)


argv = sys.argv[sys.argv.index("--") + 1:]
{"build": build, "verify": verify, "render": render}[argv[0]](*argv[1:3])
