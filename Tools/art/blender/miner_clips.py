"""Miner Mixamo clips: measure, make in place, export SK_Miner and nine A_Miner_* clips (task-386-01).

Usage (headless Blender):
  blender -b --factory-startup -P miner_clips.py -- build <miner.fbx> <clips_dir> <out_dir> <report.json>
  blender -b --factory-startup -P miner_clips.py -- verify <out_dir> <report.json>   (re-reads the ten exports)

build: per clip, measures the Mixamo Hips travel (direction in the character frame, 0 = forward,
+90 = right, 180 = back, -90 = left; speed in cm/s), sets the Hips horizontal location keys to the first
frame (vertical and every rotation untouched), puts the action on a fresh copy of the miner armature
(so all clips share its exact rest pose) and exports armature + one action. Then exports the miner
mesh with its armature and no animation as SK_Miner.fbx. Blender units here are metres (importer).
"""
import json
import math
import os
import sys

import bpy

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rr_blender as rr  # noqa: E402
from mathutils import Vector  # noqa: E402

CLIPS = [  # source file, export name
    ("Rifle Aiming Idle", "Idle"), ("Rifle Run", "RunFwd"), ("Walk With Rifle", "WalkFwd"),
    ("Run Backwards", "RunBwd"), ("Backwards Rifle Walk", "WalkBwd"), ("Run Left", "RunLeft"),
    ("Strafe Left", "StrafeLeft"), ("Run Right", "RunRight"), ("Strafe", "StrafeRight")]
HIPS = "mixamorig:Hips"
EXPORT_KW = dict(apply_scale_options="FBX_SCALE_UNITS", add_leaf_bones=False, use_selection=True,
                 mesh_smooth_type="FACE", axis_forward="-Z", axis_up="Y", path_mode="COPY", embed_textures=False)


def arm_obj():
    return [o for o in bpy.data.objects if o.type == "ARMATURE"][0]


def hierarchy(arm):
    return [(b.name, b.parent.name if b.parent else None) for b in arm.data.bones]


def fcurves(act):
    return [fc for cb in act.layers[0].strips[0].channelbags for fc in cb.fcurves]


def hips_world(arm, frame):
    bpy.context.scene.frame_set(frame)
    bpy.context.view_layer.update()
    return arm.matrix_world @ arm.pose.bones[HIPS].head


def frames_of(act):
    return int(act.frame_range[0]), int(act.frame_range[1])


def char_frame(arm):
    """(right, forward) unit vectors in the horizontal plane, from the rest pose (toes point forward)."""
    b = arm.data.bones

    def h(n):
        return arm.matrix_world @ b[n].head_local
    right = h("mixamorig:RightUpLeg") - h("mixamorig:LeftUpLeg")
    right.z = 0
    right.normalize()
    fwd = Vector((0, 0, 1)).cross(right)
    toe = h("mixamorig:LeftToeBase") - h("mixamorig:LeftFoot")
    toe.z = 0
    if fwd.dot(toe) < 0:
        fwd = -fwd
    return right, fwd


def measure(arm, act):
    f0, f1 = frames_of(act)
    fps = bpy.context.scene.render.fps
    p0, p1 = hips_world(arm, f0), hips_world(arm, f1)
    d = (p1 - p0) * 100
    right, fwd = char_frame(arm)
    dr, df = d.dot(right), d.dot(fwd)
    dur = (f1 - f0) / fps
    horiz = math.hypot(dr, df)
    zs = [hips_world(arm, f).z * 100 for f in range(f0, f1 + 1)]
    return {"frames": f1 - f0 + 1, "fps": fps, "duration_s": round(dur, 4),
            "displacement_cm_right_forward": [round(dr, 2), round(df, 2)],
            "horizontal_cm_per_cycle": round(horiz, 2),
            "speed_cm_s": round(horiz / dur, 1) if dur else 0,
            "direction_deg": round(math.degrees(math.atan2(dr, df)), 1) if horiz > 1 else None,
            "vertical_range_cm": round(max(zs) - min(zs), 2),
            "character_right_axis": [round(x, 3) for x in right], "character_forward_axis": [round(x, 3) for x in fwd]}


def make_in_place(arm, act):
    """Hips horizontal travel (armature space) set to the first-frame value; vertical and rotations kept."""
    R = arm.data.bones[HIPS].matrix_local.to_3x3()  # pose location is expressed in the bone rest frame
    Ri = R.inverted()
    path = 'pose.bones["%s"].location' % HIPS
    locs = {i: next(fc for fc in fcurves(act) if fc.data_path == path and fc.array_index == i) for i in range(3)}
    n = len(locs[0].keyframe_points)
    frames = [kp.co[0] for kp in locs[0].keyframe_points]
    for i in range(3):
        assert [kp.co[0] for kp in locs[i].keyframe_points] == frames
    arm_d = [R @ Vector([locs[i].keyframe_points[k].co[1] for i in range(3)]) for k in range(n)]
    ref = arm_d[0]
    for k in range(n):
        d = arm_d[k].copy()
        d.x, d.y = ref.x, ref.y
        new = Ri @ d
        for i in range(3):
            kp = locs[i].keyframe_points[k]
            kp.co[1] = new[i]
            kp.handle_left[1] = new[i]
            kp.handle_right[1] = new[i]
    for i in range(3):
        locs[i].update()


def export_clip(src_path, name, miner, out_dir):
    rr.import_fbx(src_path)
    clip_arm = arm_obj()
    act = bpy.data.actions[0]
    act.use_fake_user = True
    fps = bpy.context.scene.render.fps
    m = measure(clip_arm, act)
    ref_h = hierarchy(clip_arm)
    make_in_place(clip_arm, act)
    f0, f1 = frames_of(act)
    p0 = hips_world(clip_arm, f0)
    after = 0.0
    for f in range(f0, f1 + 1):
        dd = hips_world(clip_arm, f) - p0
        after = max(after, math.hypot(dd.x, dd.y) * 100)
    bpy.ops.object.select_all(action="DESELECT")
    clip_arm.select_set(True)
    bpy.ops.object.delete()
    bpy.ops.import_scene.fbx(filepath=miner)
    arm = arm_obj()
    for o in [o for o in bpy.data.objects if o.type == "MESH"]:
        bpy.data.objects.remove(o, do_unlink=True)
    assert hierarchy(arm) == ref_h, "clip and miner hierarchy differ"
    arm.animation_data_create()
    arm.animation_data.action = act
    arm.animation_data.action_slot = act.slots[0]  # the slot was made for the clip armature: bind it explicitly
    zs = [hips_world(arm, f).z for f in range(f0, f1 + 1)]
    assert max(zs) - min(zs) > 1e-4 or name == "Idle", "action does not drive the miner armature"
    act.name = "A_Miner_" + name
    bpy.context.scene.render.fps = fps
    bpy.context.scene.frame_start, bpy.context.scene.frame_end = f0, f1
    path = os.path.join(out_dir, "Anims", "A_Miner_%s.fbx" % name)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    bpy.ops.object.select_all(action="DESELECT")
    arm.select_set(True)
    bpy.context.view_layer.objects.active = arm
    bpy.ops.export_scene.fbx(filepath=path, object_types={"ARMATURE"}, bake_anim=True,
                             bake_anim_use_all_actions=False, bake_anim_use_nla_strips=False,
                             bake_anim_simplify_factor=0.0, bake_anim_force_startend_keying=True, **EXPORT_KW)
    m.update({"export": "Anims/A_Miner_%s.fbx" % name, "in_place_max_horizontal_hips_cm": round(after, 4)})
    return m


def build(miner, clips_dir, out_dir, report):
    res = {"export_settings": dict(EXPORT_KW), "clips": {}}
    for src, name in CLIPS:
        m = export_clip(os.path.join(clips_dir, src + ".fbx"), name, miner, out_dir)
        m["source"] = src + ".fbx"
        res["clips"][name] = m
        print("CLIP", name, m)
    rr.import_fbx(miner)
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.export_scene.fbx(filepath=os.path.join(out_dir, "SK_Miner.fbx"), object_types={"MESH", "ARMATURE"},
                             bake_anim=False, **EXPORT_KW)
    res["sk_miner"] = "SK_Miner.fbx"
    with open(report, "w") as f:
        json.dump(res, f, indent=2)


def verify(out_dir, report):
    with open(report) as f:
        res = json.load(f)
    rr.import_fbx(os.path.join(out_dir, "SK_Miner.fbx"))
    arm = arm_obj()
    ref = hierarchy(arm)
    meshes = [o for o in bpy.data.objects if o.type == "MESH"]
    res["skeleton_check"] = {"bones": len(ref), "root": [n for n, p in ref if p is None],
                             "sk_miner_triangles": sum(rr.tri_count(o) for o in meshes),
                             "sk_miner_dimensions_cm": [round(x * 100, 2) for x in rr.dimensions_cm(meshes)],
                             "sk_miner_actions": len(bpy.data.actions), "clips_identical_hierarchy": {}}
    for name, m in res["clips"].items():
        rr.import_fbx(os.path.join(out_dir, m["export"]))
        a = arm_obj()
        act = bpy.data.actions
        ok = hierarchy(a) == ref
        f0, f1 = int(act[0].frame_range[0]), int(act[0].frame_range[1])
        p0 = hips_world(a, f0)
        dmax = 0.0
        zs = []
        for f in range(f0, f1 + 1):
            dd = hips_world(a, f) - p0
            dmax = max(dmax, math.hypot(dd.x, dd.y) * 100)
            zs.append(hips_world(a, f).z * 100)
        res["skeleton_check"]["clips_identical_hierarchy"][name] = ok
        m["reread"] = {"actions": len(act), "frames": f1 - f0 + 1, "fps": bpy.context.scene.render.fps,
                       "hips_horizontal_after_cm": round(dmax, 4),
                       "hips_vertical_range_cm": round(max(zs) - min(zs), 2)}
        print("VERIFY", name, ok, m["reread"])
    with open(report, "w") as f:
        json.dump(res, f, indent=2)
    print("SK", res["skeleton_check"])


argv = sys.argv[sys.argv.index("--") + 1:]
if argv[0] == "build":
    build(*argv[1:5])
else:
    verify(*argv[1:3])
