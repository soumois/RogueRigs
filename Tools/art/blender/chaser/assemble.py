"""Chaser assembly (task-151-05): builds the ten objects from the six part modules, parents them, writes the part reports, merges them into the whole report, exports the FBX.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser/assemble.py -- --out Art/Meshes/chaser/SM_Chaser_parts.fbx [--render-dir Art/Meshes/chaser] [--parts-dir Art/Meshes/chaser/parts]
(Tools/art/blender/chaser_model.py is a thin wrapper with the same command line.)
Units: 1 Blender unit = 1 cm. H = 124 cm. Forward = +X, robot left = +Y. Hierarchy Pelvis > HeadTorso > UpperArm > ForearmHand, Pelvis > Thigh > ShinFoot.
Flat shading, four flat materials (yellow_body, gunmetal, eye_red emissive, logo_plate textured decal). UV0: logo decal only; UV1 LightMap: one island per face."""
import bpy, math, json, os, sys, importlib
from mathutils import Vector

HERE = os.path.dirname(os.path.abspath(__file__)) if "__file__" in globals() else os.path.abspath("Tools/art/blender/chaser")
if HERE not in sys.path:
    sys.path.insert(0, HERE)
import common as C
from common import H

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/chaser/SM_Chaser_parts.fbx", "--render-dir": "", "--parts-dir": ""}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"])
OUTDIR = os.path.dirname(OUT)
PARTS_DIR = os.path.abspath(opt["--parts-dir"]) if opt["--parts-dir"] else os.path.join(OUTDIR, "parts")
os.makedirs(OUTDIR, exist_ok=True)

C.setup(OUTDIR)
# ---------------------------------------------------------------- build the objects, one module per unique part
mods = {g: importlib.import_module(v[0]) for g, v in C.GROUPS.items()}
base_group = {v[1]: g for g, v in C.GROUPS.items()}
objs = {}
for name in C.BUILD_ORDER:
    base, _, side = name.partition("_")
    objs[name] = mods[base_group[base]].build(side or None)
for name, ob in objs.items():
    base, _, side = name.partition("_")
    pn = C.parent_name(base, side or None)
    if pn:
        ob.parent = objs[pn]
        ob.location = C.PIVOT_W[name] - C.PIVOT_W[pn]
    else:
        ob.location = C.PIVOT_W[name]
bpy.context.view_layer.update()
PIVOT_W = C.PIVOT_W

# ---------------------------------------------------------------- part reports (one file per part), then the merge
reports = {}
for g, (modn, base, sided) in C.GROUPS.items():
    names = [n for n in C.BUILD_ORDER if n.partition("_")[0] == base]
    C.write_part_report(g, [objs[n] for n in names], PARTS_DIR)
    reports[g] = json.load(open(os.path.join(PARTS_DIR, g + ".report.json"), encoding="utf-8"))
per = {}
for n in C.BUILD_ORDER:
    g = base_group[n.partition("_")[0]]
    per[n] = reports[g]["objects"][n]
warnings = []
tot = sum(v["tris"] for v in per.values())
for k, v in per.items():
    if not v["lightmap"]["pass"]:
        warnings.append("LightMap check FAILED on " + k)
glo = Vector(tuple(min(v["bounds_min_cm"][i] for v in per.values()) for i in range(3)))
ghi = Vector(tuple(max(v["bounds_max_cm"][i] for v in per.values()) for i in range(3)))
# the whole report keeps the legacy per-part entries (the extras stay in the part reports)
per = {k: {kk: vv for kk, vv in v.items() if kk not in ("attachment_section", "material_slots")} for k, v in per.items()}
if tot > C.BUDGET:
    warnings.append("tris above %d budget" % C.BUDGET)
dim = [round(ghi[i] - glo[i], 2) for i in range(3)]
if abs(ghi.z - H) > 0.3 or abs(glo.z) > 0.01:
    warnings.append("height off %.0f cm" % H)


def overlap(a, b):
    la, ha, lb, hb = per[a]["bounds_min_cm"], per[a]["bounds_max_cm"], per[b]["bounds_min_cm"], per[b]["bounds_max_cm"]
    return [round(max(0.0, min(ha[i], hb[i]) - max(la[i], lb[i])), 2) for i in range(3)]


bbox_overlap = {}
for k in C.BUILD_ORDER:
    if per[k]["parent"]:
        bbox_overlap[per[k]["parent"] + " / " + k] = overlap(per[k]["parent"], k)


def inside_depth(child, parent):
    """largest depth (cm) of a child vertex inside the closed parent mesh (ray parity for inside, BVH nearest-surface distance for the depth)"""
    from mathutils.bvhtree import BVHTree
    bvh = BVHTree.FromObject(parent, bpy.context.evaluated_depsgraph_get())
    inv = parent.matrix_world.inverted()
    dirs = [Vector(t).normalized() for t in ((0.3713, 0.5113, 0.7713), (-0.6211, 0.2417, 0.7441), (0.1931, -0.8123, 0.5511))]
    deepest = 0.0
    for v in child.data.vertices:
        lp = inv @ (child.matrix_world @ v.co)
        votes = 0
        for d in dirs:                                    # majority vote of three ray parities (a ray through an edge can miscount)
            o, cnt = lp.copy(), 0
            while True:
                hit = bvh.ray_cast(o, d)
                if hit[0] is None:
                    break
                cnt += 1
                o = hit[0] + d * 1e-4
            votes += cnt % 2
        if votes >= 2:
            nr = bvh.find_nearest(lp)
            if nr[0] is not None:
                deepest = max(deepest, nr[3])
    return deepest


pen = {}
for k in C.BUILD_ORDER:
    pn = per[k]["parent"]
    if pn:
        pen[pn + " / " + k] = round(max(inside_depth(objs[k], objs[pn]), inside_depth(objs[pn], objs[k])), 2)


def J(n):
    return [round(c, 2) for c in PIVOT_W[n]]


report = {
    "asset": "SM_Chaser_parts", "route": "blender", "script": "Tools/art/blender/chaser/assemble.py (wrapper Tools/art/blender/chaser_model.py)", "source_image": "vault/assets/chaser-turnaround.png (crops chaser-front, chaser-side, chaser-back)",
    "height_cm": H, "height_source": "ARRChaser capsule 24 x 62 half height (Source/RogueRigs/Enemies/RRChaser.cpp InitCapsuleSize(24, 62)) = 124 cm; bp-chaser BodyMesh 124 uu centred on the capsule; model from the sole to the top of the cap",
    "axes": "forward +X, robot left +Y, up +Z; _L parts at +Y", "units": "cm",
    "total_tris": tot, "budget_tris": C.BUDGET, "parts": per, "dimensions_cm": {"x": dim[0], "y": dim[1], "z": dim[2]},
    "bbox_min": [round(c, 2) for c in glo], "bbox_max": [round(c, 2) for c in ghi],
    "hierarchy": C.HIERARCHY,
    "joints_cm": {"Pelvis_root_hip_height": J("Pelvis"), "waist_HeadTorso": J("HeadTorso"), "shoulder_L": J("UpperArm_L"), "shoulder_R": J("UpperArm_R"),
                  "elbow_L": J("ForearmHand_L"), "elbow_R": J("ForearmHand_R"), "hip_L": J("Thigh_L"), "hip_R": J("Thigh_R"), "knee_L": J("ShinFoot_L"), "knee_R": J("ShinFoot_R")},
    "joint_bbox_overlap_cm_xyz": bbox_overlap, "joint_penetration_cm": pen,
    "part_reports": {g: "Art/Meshes/chaser/parts/%s.report.json" % g for g in C.GROUPS},
    "materials": C.ORDER, "slots": C.ORDER, "paint_colours_srgb": C.HEX, "emissive_material": ["eye_red"],
    "decal": {"texture": "Art/Meshes/chaser/T_ChaserLogo.png", "uv": "UVMap (UV0) 0..1 on the logo quad only, other faces collapsed to (0,0)", "offset_cm": 0.25, "material": "logo_plate (Masked not needed: no alpha)"},
    "uv_layers": ["UVMap", "LightMap"], "shading": "flat", "warnings": warnings,
}
warnings.append("eye_red is emissive in Blender (strength 4, #FF1010): the FBX does not carry emission, the Unreal material sets the glow")

# ---------------------------------------------------------------- optional renders (fidelity loop, whole robot)
RD = opt["--render-dir"]
if RD:
    RD = os.path.abspath(RD)
    sc = bpy.context.scene
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            pass
    sc.view_settings.view_transform = "Standard"
    sc.render.film_transparent = False
    w = bpy.data.worlds.new("W")
    sc.world = w
    w.use_nodes = True
    bgn = w.node_tree.nodes["Background"]
    BG = 0.45
    bgn.inputs["Color"].default_value = (BG, BG, BG, 1)
    bgn.inputs["Strength"].default_value = 1.0
    suns = []
    for e in (3.0, 0.8):
        sun = bpy.data.lights.new("sun", "SUN")
        sun.energy = e
        so = bpy.data.objects.new("sun", sun)
        sc.collection.objects.link(so)
        suns.append(so)

    def light_from(cam_obj):
        """key sun comes from the camera side, raised; fill from above-behind"""
        fwd = (cam_obj.rotation_euler.to_matrix() @ Vector((0, 0, -1)))
        k = (fwd + Vector((0, 0, -0.9))).normalized()
        suns[0].rotation_euler = k.to_track_quat("-Z", "Y").to_euler()
        f = (-fwd * 0.5 + Vector((0, 0, -1))).normalized()
        suns[1].rotation_euler = f.to_track_quat("-Z", "Y").to_euler()
    cam = bpy.data.cameras.new("cam")
    cam.type = "ORTHO"
    co = bpy.data.objects.new("cam", cam)
    sc.collection.objects.link(co)
    sc.camera = co
    # crop measurements: (res w, h, bottom row of sole, px per cm, camera lateral coordinate, rotation)
    VIEWS = {
        "front": ((550, 790), 757, 713 / H, (300.0, -0.28, None), (90, 0, 90)),
        "side": ((320, 790), 757, 736 / H, (None, 300.0, None), (90, 0, 180)),
        "back": ((560, 790), 757, 715 / H, (-300.0, -1.44, None), (90, 0, -90)),
    }
    for vn, (res, bot, ppc, loc, rot) in VIEWS.items():
        cz = (bot - res[1] / 2) / ppc
        if vn == "side":
            cx = (221 - res[0] / 2) / 5.935
            location = (cx, 300.0, cz)
        else:
            location = (loc[0], loc[1], cz)
        co.location = location
        co.rotation_euler = tuple(math.radians(a) for a in rot)
        cam.ortho_scale = res[1] / ppc
        bpy.context.view_layer.update()
        light_from(co)
        sc.render.resolution_x, sc.render.resolution_y = res
        sc.render.resolution_percentage = 100
        sc.render.filepath = os.path.join(RD, "render_%s.png" % vn)
        bpy.ops.render.render(write_still=True)
    CLOSE = {"hand_front": ((528, 528), (300.0, -36.0, 65.0), (90, 0, 90), 22.0), "hand_side": ((528, 528), (3.5, 300.0, 65.0), (90, 0, 180), 22.0),
             "calf_front": ((528, 528), (300.0, -17.0, 24.0), (90, 0, 90), 44.0), "calf_side": ((528, 528), (0.5, 300.0, 24.0), (90, 0, 180), 44.0)}
    for vn, (res, loc, rot, osz) in CLOSE.items():
        co.location = loc
        co.rotation_euler = tuple(math.radians(a) for a in rot)
        cam.ortho_scale = osz
        bpy.context.view_layer.update()
        light_from(co)
        sc.render.resolution_x, sc.render.resolution_y = res
        sc.render.filepath = os.path.join(RD, "closeup_%s.png" % vn)
        bpy.ops.render.render(write_still=True)
    tgt = Vector((0, 0, 62))
    d = Vector((math.cos(math.radians(-35)) * math.cos(math.radians(25)), math.sin(math.radians(-35)) * math.cos(math.radians(25)), math.sin(math.radians(25)))) * 300
    co.location = tgt + d
    co.rotation_euler = (tgt - co.location).to_track_quat("-Z", "Y").to_euler()
    cam.ortho_scale = 150
    bpy.context.view_layer.update()
    light_from(co)
    sc.render.resolution_x, sc.render.resolution_y = 800, 900
    sc.render.filepath = os.path.join(RD, "render_threequarter.png")
    bpy.ops.render.render(write_still=True)
    for o in [co] + suns:
        bpy.data.objects.remove(o)

# ---------------------------------------------------------------- export
bpy.ops.object.select_all(action="DESELECT")
for o in objs.values():
    o.select_set(True)
bpy.context.view_layer.objects.active = objs["Pelvis"]
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
RP = os.path.splitext(OUT)[0] + ".report.json"
if os.path.exists(RP):
    try:
        old = json.load(open(RP, encoding="utf-8"))
        for key in ("fidelity", "refinement"):
            if key in old:
                report[key] = old[key]
    except Exception:
        pass
with open(RP, "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print("TOTAL_TRIS", tot, "DIM", dim)
print(json.dumps({k: v["tris"] for k, v in per.items()}))
print("WARNINGS", warnings)
