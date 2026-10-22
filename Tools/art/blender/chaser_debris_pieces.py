"""task-166-01: cut the 10 rigid chaser parts into 13 debris pieces for the Niagara death burst. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser_debris_pieces.py -- [--in-fbx Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx] [--in-tex Art/Meshes/chaser-meshy/chaser_meshy_fixed_basecolor.png] [--out-dir Art/Source/chaser-meshy/debris]
Pieces (fixed order = Niagara mesh index): Torso (HeadTorso + Pelvis), UpperArm_L/R, Forearm_L/R (ForearmHand), Arm_L/R (upper + forearm), Thigh_L/R, LowerLeg_L/R (ShinFoot), Leg_L/R (thigh + lower leg).
Rest pose = the parts as imported (world matrices). Cut caps kept. Each piece is baked to world space, then its vertices are moved so the pivot is the bounds centre.
UV0 and the base colour texture are unchanged; UV1 (LightMap), UV2 (PartId) and the PartIndex colour are dropped (a merged lightmap would overlap, debris needs none).
Export: cm, FBX_SCALE_UNITS, axis -Z/Y (Blender default = Unreal import: X kept, Y mirrored). Report debris_pieces.json: centre offsets in Unreal axes (x, -y_blender, z)."""
import bpy, sys, os, json, shutil
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--in-fbx": "Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx", "--in-tex": "Art/Meshes/chaser-meshy/chaser_meshy_fixed_basecolor.png", "--out-dir": "Art/Source/chaser-meshy/debris"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
# task-288-01: optional per-robot config (--config robot_configs/<robot>.json, key "debris": in_fbx, in_tex, out_dir, prefix, eye_to_body); no config = chaser v1 defaults
PREFIX, EYE_TO_BODY = "Chaser", False
if "--config" in opt:
    _d = json.load(open(opt["--config"]))["debris"]
    PREFIX, EYE_TO_BODY = _d.get("prefix", PREFIX), _d.get("eye_to_body", False)
    for _k, _o in (("in_fbx", "--in-fbx"), ("in_tex", "--in-tex"), ("out_dir", "--out-dir")):
        if _k in _d and _o not in argv:
            opt[_o] = _d[_k]
OUT = os.path.abspath(opt["--out-dir"])
TEX = os.path.abspath(opt["--in-tex"])
os.makedirs(OUT, exist_ok=True)
for f in os.listdir(OUT):
    if f.startswith("SM_" + PREFIX + "Debris_") or f == "debris_pieces.json":
        os.remove(os.path.join(OUT, f))
shutil.copyfile(TEX, os.path.join(OUT, os.path.basename(TEX)))

# (piece, source parts) in the fixed order
PIECES = [("Torso", ["HeadTorso", "Pelvis"])]
for s in ("L", "R"):
    PIECES.append(("UpperArm_" + s, ["UpperArm_" + s]))
for s in ("L", "R"):
    PIECES.append(("Forearm_" + s, ["ForearmHand_" + s]))
for s in ("L", "R"):
    PIECES.append(("Arm_" + s, ["UpperArm_" + s, "ForearmHand_" + s]))
for s in ("L", "R"):
    PIECES.append(("Thigh_" + s, ["Thigh_" + s]))
for s in ("L", "R"):
    PIECES.append(("LowerLeg_" + s, ["ShinFoot_" + s]))
for s in ("L", "R"):
    PIECES.append(("Leg_" + s, ["Thigh_" + s, "ShinFoot_" + s]))

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.abspath(opt["--in-fbx"]), colors_type="LINEAR")
parts = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
assert len(parts) == 10, sorted(parts)
img = bpy.data.images.load(os.path.join(OUT, os.path.basename(TEX)), check_existing=False)
img.colorspace_settings.name = "sRGB"
for o in parts.values():
    for m in o.data.materials:
        if m and m.node_tree:
            for n in m.node_tree.nodes:
                if n.type == "TEX_IMAGE":
                    n.image = img
bpy.context.view_layer.update()
allz = [(o.matrix_world @ v.co).z for o in parts.values() for v in o.data.vertices]
print("rest-pose height cm", max(allz) - min(allz), "floor z", min(allz))


def tris(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


part_tris = {n: tris(o.data) for n, o in parts.items()}
report = dict(
    source=opt["--in-fbx"], texture=os.path.basename(TEX),
    offset_convention=("centre_offset_cm_unreal = (x, -y, z) of the piece bounds centre in the rest-pose Blender world (cm, origin at the floor between the feet, front +X, robot left +Y, Z up). "
                       "The FBX is exported with the Blender default axes (forward -Z, up Y) which Unreal imports with X and Z kept and Y mirrored; the chaser pipeline mirrors Y the same way (ChaserAnimWPO). "
                       "So the robot's left (+Y Blender) is -Y in Unreal. Mesh pivot = bounds centre, so the piece's Unreal location at rest = this offset."),
    units="cm", pieces=[])
for idx, (name, srcs) in enumerate(PIECES):
    dups = []
    for s in srcs:
        d = parts[s].copy()
        d.data = parts[s].data.copy()
        d.parent = None
        bpy.context.scene.collection.objects.link(d)
        d.matrix_world = parts[s].matrix_world.copy()
        dups.append(d)
    bpy.ops.object.select_all(action="DESELECT")
    for d in dups:
        d.select_set(True)
    bpy.context.view_layer.objects.active = dups[0]
    # bake world transform into the vertices (matrix_world was copied; parent removed keeps it via the assignment above)
    for d in dups:
        me = d.data
        mw = d.matrix_world.copy()
        me.transform(mw)
        d.matrix_world.identity()
    if len(dups) > 1:
        bpy.ops.object.join()
    ob = bpy.context.view_layer.objects.active
    me = ob.data
    if EYE_TO_BODY:  # eye faces keep their UV0 on the body texture: one material, no glow slot
        for p in me.polygons:
            p.material_index = 0
        while len(me.materials) > 1:
            me.materials.pop(index=len(me.materials) - 1)
    for n in [u.name for u in me.uv_layers][1:]:
        me.uv_layers.remove(me.uv_layers[n])
    for n in [c.name for c in me.color_attributes]:
        me.color_attributes.remove(me.color_attributes[n])
    xs = [v.co for v in me.vertices]
    lo = Vector((min(c.x for c in xs), min(c.y for c in xs), min(c.z for c in xs)))
    hi = Vector((max(c.x for c in xs), max(c.y for c in xs), max(c.z for c in xs)))
    ctr = (lo + hi) / 2
    me.transform(__import__("mathutils").Matrix.Translation(-ctr))
    ob.name = me.name = "SM_" + PREFIX + "Debris_" + name
    ob.location = (0, 0, 0)
    bpy.ops.object.select_all(action="DESELECT")
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    path = os.path.join(OUT, ob.name + ".fbx")
    bpy.ops.export_scene.fbx(filepath=path, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0,
                             axis_forward="-Z", axis_up="Y", mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
    report["pieces"].append(dict(index=idx, name=ob.name, file=ob.name + ".fbx", triangles=tris(me), source_parts=srcs,
                                 centre_offset_cm_unreal=[round(ctr.x, 2), round(-ctr.y, 2), round(ctr.z, 2)],
                                 size_cm=[round(hi.x - lo.x, 2), round(hi.y - lo.y, 2), round(hi.z - lo.z, 2)], uv_layers=[u.name for u in me.uv_layers],
                                 materials=[m.name for m in me.materials]))
    bpy.data.objects.remove(ob)
report["part_triangles"] = part_tris
report["sum_parts_triangles"] = sum(part_tris.values())
with open(os.path.join(OUT, "debris_pieces.json"), "w") as f:
    json.dump(report, f, indent=2)
print("done", len(report["pieces"]))
