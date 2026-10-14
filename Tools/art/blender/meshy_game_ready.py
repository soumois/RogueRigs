"""task-156-01: game-ready version of the corrected Meshy chaser. Geometry, UV0 and flat shading are never changed. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_game_ready.py -- --in-fbx <chaser_meshy_fixed.fbx> --in-tex <chaser_meshy_fixed_basecolor.png> --out-dir Art/Meshes/chaser-meshy [--height-cm 123] [--work <short dir>]
Steps: uniform scale to --height-cm (1 Blender unit = 1 cm = 1 Unreal unit, FBX UnitScaleFactor 1), rotation about Z of +90 deg (front -Y -> +X, left +X -> +Y),
origin on the floor under the midpoint of the two feet, transforms applied, UV1 'LightMap' (one island per face, shelf packed, 2048 atlas), object SM_Chaser_Meshy,
one material slot meshy_chaser, texture referenced (not embedded). Writes chaser_meshy_game.fbx and chaser_meshy_game.report.json (checked by re-importing the FBX).
Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, bmesh, sys, os, json, time, shutil, struct
import numpy as np
from mathutils import Vector, Matrix

T0 = time.time()
TIM = {}
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--in-fbx": "", "--in-tex": "", "--out-dir": "", "--height-cm": "123", "--work": "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-fbx", "--in-tex", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
HEIGHT = float(opt["--height-cm"])
WO = os.path.join(WORK, "game_out")
os.makedirs(WO, exist_ok=True)
for f in os.listdir(WO):
    p_ = os.path.join(WO, f)
    shutil.rmtree(p_, ignore_errors=True) if os.path.isdir(p_) else os.remove(p_)
TEXNAME = os.path.basename(opt["--in-tex"])
IN_FBX = os.path.join(WORK, "in_" + os.path.basename(opt["--in-fbx"]))
shutil.copyfile(opt["--in-fbx"], IN_FBX)
WTEX = os.path.join(WO, TEXNAME)
shutil.copyfile(opt["--in-tex"], WTEX)
OUT_NAME = "chaser_meshy_game"
LM = 2048


def tick(n, t):
    TIM[n] = round(time.time() - t, 2)


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.data.objects if o.type == "MESH"]


def stats(ob):
    m = ob.data
    bb = np.array([(ob.matrix_world @ Vector(c))[:] for c in ob.bound_box])
    return dict(tris=sum(len(p.vertices) - 2 for p in m.polygons), vertices=len(m.vertices), min=bb.min(0).tolist(), max=bb.max(0).tolist())


def geom(ob):
    m = ob.data
    v = np.array([(ob.matrix_world @ x.co)[:] for x in m.vertices])
    vi = np.array([[m.loops[j].vertex_index for j in p.loop_indices] for p in m.polygons])
    uv = np.array([l.uv[:] for l in m.uv_layers[0].data])
    return v, vi, uv


# ---------------------------------------------------------------- input
t = time.time()
obs = import_fbx(IN_FBX)
assert len(obs) == 1
ob = obs[0]
S_IN = stats(ob)
V_IN, VI_IN, UV_IN = geom(ob)
h_in = S_IN["max"][2] - S_IN["min"][2]
tick("load", t)

# ---------------------------------------------------------------- transform
t = time.time()
scale = HEIGHT / h_in
ob.matrix_world = Matrix.Rotation(np.radians(90.0), 4, "Z") @ Matrix.Scale(scale, 4)      # uniform scale then +90 deg about Z (front -Y -> +X)
bpy.context.view_layer.update()
mw = ob.matrix_world.copy()
me = ob.data
v = np.array([(mw @ x.co)[:] for x in me.vertices])
zmin = v[:, 2].min()
low = v[v[:, 2] < zmin + 0.03 * HEIGHT]      # soles of both feet
feet_mid = [float((low[:, 0].min() + low[:, 0].max()) / 2), float((low[:, 1].min() + low[:, 1].max()) / 2)]
off = Vector((-feet_mid[0], -feet_mid[1], -zmin))
for x in me.vertices:
    x.co = mw @ x.co + off      # bake: all transforms applied, object matrix identity
ob.matrix_world = Matrix.Identity(4)
ob.name = "SM_Chaser_Meshy"
me.name = "SM_Chaser_Meshy"
for p in me.polygons:
    p.use_smooth = False      # input is flat shaded: kept
me.update()
assert len(me.materials) == 1
mat = me.materials[0]
mat.name = "meshy_chaser"
img = None
for n in mat.node_tree.nodes:
    if n.type == "TEX_IMAGE":
        img = bpy.data.images.load(WTEX, check_existing=False)
        img.colorspace_settings.name = "sRGB"
        n.image = img
assert img is not None
tick("transform", t)

# ---------------------------------------------------------------- LightMap UV1
t = time.time()
bm = bmesh.new()
bm.from_mesh(me)
bm.faces.ensure_lookup_table()
bm.normal_update()
uv1 = bm.loops.layers.uv.get("LightMap") or bm.loops.layers.uv.new("LightMap")
PAD = 2.0
island = []
for f in bm.faces:
    nn = f.normal.normalized()
    tt = Vector((1, 0, 0)) if abs(nn.z) > 0.99 else Vector((-nn.y, nn.x, 0)).normalized()
    ss = nn.cross(tt).normalized()
    loc = [(l.vert.co.dot(tt), l.vert.co.dot(ss)) for l in f.loops]
    mx_, my_ = min(a for a, b in loc), min(b for a, b in loc)
    loc = [(a - mx_, b - my_) for a, b in loc]
    island.append((loc, max(a for a, b in loc), max(b for a, b in loc)))
order = sorted(range(len(island)), key=lambda i: -island[i][2])


def pack(sc):
    x = y = PAD
    rowh = 0.0
    out = {}
    for i in order:
        _, w, hh = island[i]
        pw, ph = w * sc, hh * sc
        if x + pw + PAD > LM:
            x = PAD
            y += rowh + PAD
            rowh = 0.0
        if x + pw + PAD > LM or y + ph + PAD > LM:
            return None
        out[i] = (x, y)
        x += pw + PAD
        rowh = max(rowh, ph)
    return out


lo, hi = 1e-4, 50.0
for _ in range(50):
    mid = (lo + hi) / 2
    if pack(mid):
        lo = mid
    else:
        hi = mid
placed = pack(lo)
for f in bm.faces:
    x, y = placed[f.index]
    for l, (lx, ly) in zip(f.loops, island[f.index][0]):
        l[uv1].uv = ((x + lx * lo) / LM, (y + ly * lo) / LM)
bm.to_mesh(me)
bm.free()
for p in me.polygons:
    p.use_smooth = False
me.uv_layers.active_index = 0
tick("lightmap", t)

# overlap check: bounding boxes of the per-face islands, pairwise (islands are triangles packed in disjoint shelf cells, so disjoint boxes = no overlap)
t = time.time()
u1 = np.array([l.uv[:] for l in me.uv_layers["LightMap"].data]).reshape(-1, 3, 2) * LM
bmin, bmax = u1.min(1), u1.max(1)
ov_bbox = 0
min_gap = 1e9
n = len(u1)
for i in range(n - 1):
    gx = np.maximum(bmin[i + 1:, 0] - bmax[i, 0], bmin[i, 0] - bmax[i + 1:, 0])
    gy = np.maximum(bmin[i + 1:, 1] - bmax[i, 1], bmin[i, 1] - bmax[i + 1:, 1])
    g = np.maximum(gx, gy)
    ov_bbox += int((g < 0).sum())
    min_gap = min(min_gap, float(g.min()))
border = float(min(bmin.min(), (LM - bmax).min()))
lm_rep = dict(layer="LightMap", atlas_px=LM, faces=n, island_scale_px_per_cm=round(lo, 5), method="one island per face, shelf packed, bbox test",
              overlapping_face_pairs_bbox=ov_bbox, min_gap_px=round(min_gap, 3), min_border_px=round(border, 3),
              bounds_min=round(float(u1.min() / LM), 5), bounds_max=round(float(u1.max() / LM), 5),
              uv_area_fraction=round(float(0.5 * np.abs((u1[:, 1, 0] - u1[:, 0, 0]) * (u1[:, 2, 1] - u1[:, 0, 1]) - (u1[:, 2, 0] - u1[:, 0, 0]) * (u1[:, 1, 1] - u1[:, 0, 1])).sum() / LM ** 2), 4))
lm_rep["pass"] = bool(ov_bbox == 0 and min_gap >= 1.0 and u1.min() >= 0 and u1.max() <= LM)
tick("lightmap_check", t)

# ---------------------------------------------------------------- export
t = time.time()
bpy.context.scene.unit_settings.scale_length = 1.0
bpy.ops.object.select_all(action="DESELECT")
ob.select_set(True)
bpy.context.view_layer.objects.active = ob
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_NONE", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
FBX = os.path.join(WO, OUT_NAME + ".fbx")
bpy.ops.export_scene.fbx(filepath=FBX, **EXPORT)
b = open(FBX, "rb").read()
usf = None
j = b.find(b"UnitScaleFactor")
if j >= 0:
    k = b.find(b"D", j + 20)
    usf = struct.unpack("<d", b[k + 1:k + 9])[0]
tick("export", t)

# ---------------------------------------------------------------- read back
t = time.time()
obs2 = import_fbx(FBX)
ob2 = obs2[0]
S2 = stats(ob2)
V2, VI2, UV2 = geom(ob2)
uvn2 = [u.name for u in ob2.data.uv_layers]
mats2 = [m.name if m else None for m in ob2.data.materials]
imgs2 = sorted({n.image.filepath for m in ob2.data.materials for n in m.node_tree.nodes if n.type == "TEX_IMAGE" and n.image})
flat2 = not any(p.use_smooth for p in ob2.data.polygons)
# front check: the red eye = faces whose base colour texel (at the UV0 centroid) is strongly red
img = bpy.data.images.load(WTEX, check_existing=False)
tex = np.array(img.pixels[:], dtype=np.float32).reshape(img.size[1], img.size[0], 4)
cen_uv = UV2.reshape(-1, 3, 2).mean(1)
px = tex[np.clip((cen_uv[:, 1] * img.size[1]).astype(int), 0, img.size[1] - 1), np.clip((cen_uv[:, 0] * img.size[0]).astype(int), 0, img.size[0] - 1), :3]
red = (px[:, 0] > 0.6) & (px[:, 1] < 0.25) & (px[:, 2] < 0.25)
fc = V2[VI2].mean(1)
red_c = fc[red].mean(0).tolist() if red.any() else None
body_c = V2.mean(0).tolist()
tick("readback", t)

Rz = np.array([[0, -1, 0], [1, 0, 0], [0, 0, 1]], float)
exp = (V_IN * scale) @ Rz.T + np.array(off[:])
g_ok = dict(vertices_equal=S2["vertices"] == S_IN["vertices"], triangles_equal=S2["tris"] == S_IN["tris"],
            max_vertex_error_cm_vs_expected=float(np.abs(exp - V2).max()) if len(exp) == len(V2) else None,
            uv0_max_abs_diff_vs_input=float(np.abs(UV_IN - UV2).max()) if UV_IN.shape == UV2.shape else None)
size = [S2["max"][i] - S2["min"][i] for i in range(3)]
sole = V2[V2[:, 2] < 0.03 * HEIGHT]
rep = dict(
    inputs=dict(fbx=os.path.basename(opt["--in-fbx"]), texture=TEXNAME, input_height_units=round(h_in, 5), input_triangles=S_IN["tris"], input_vertices=S_IN["vertices"]),
    outputs=dict(fbx=OUT_NAME + ".fbx", texture_referenced=TEXNAME, embedded=False),
    scale_factor=round(scale, 6), rotation_about_z_deg=90, applied_origin_offset_cm=[round(x, 4) for x in off[:]],
    feet_midpoint_before_offset_cm=[round(x, 4) for x in feet_mid], feet_definition="vertices within 3 percent of the height above the sole; midpoint = centre of their x/y bounding box",
    export_settings=dict(**{k: (sorted(v) if isinstance(v, set) else v) for k, v in EXPORT.items()}, blender=bpy.app.version_string, scene_unit_scale_length=1.0),
    axis_note="Blender side: X forward (robot front, red eye), Y left, Z up. Exporter axis_forward=-Z, axis_up=Y (Blender default): the file is Y-up and Unreal's importer maps it back to X forward, Z up with the handedness mirror; no extra rotation or scale at import (uniform scale 1). Not verified in Unreal (nothing imported).",
    fbx_unit_scale_factor_in_file=usf,
    readback=dict(blender_import="default settings", bounds_min_cm=[round(x, 3) for x in S2["min"]], bounds_max_cm=[round(x, 3) for x in S2["max"]],
                  size_cm=[round(x, 3) for x in size], height_cm=round(size[2], 3), triangles=S2["tris"], vertices=S2["vertices"], objects=len(obs2), object_name=ob2.name,
                  material_slots=mats2, uv_layers=uvn2, texture_paths=imgs2, flat_shaded=flat2),
    forward_axis_check=dict(red_eye_faces=int(red.sum()), red_eye_centroid_cm=[round(x, 2) for x in red_c] if red_c else None, mesh_centroid_cm=[round(x, 2) for x in body_c],
                            red_eye_on_plus_x=bool(red_c and red_c[0] > body_c[0] + 1.0),
                            left_plus_y_note="rotation +90 deg about Z maps the robot left (+X in the input) to +Y"),
    feet_midpoint_after_cm=[round(float(sole[:, 0].min() + sole[:, 0].max()) / 2, 4), round(float(sole[:, 1].min() + sole[:, 1].max()) / 2, 4)],
    geometry_check=g_ok, lightmap=lm_rep, warnings=[],
)
if abs(size[2] - HEIGHT) > 0.5:
    rep["warnings"].append("height off")
if S2["tris"] != S_IN["tris"]:
    rep["warnings"].append("triangle count changed")
if not lm_rep["pass"]:
    rep["warnings"].append("lightmap check failed")
rep["timings_s"] = dict(TIM, total=round(time.time() - T0, 2))
with open(os.path.join(WO, OUT_NAME + ".report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
os.makedirs(OUT, exist_ok=True)
for fn in os.listdir(WO):
    dst = os.path.join(OUT, fn)
    if os.path.isdir(os.path.join(WO, fn)):
        continue
    if fn == TEXNAME and os.path.exists(dst):      # texture already next to the FBX
        continue
    shutil.copyfile(os.path.join(WO, fn), dst)
print("DONE", json.dumps({k: rep[k] for k in ("readback", "forward_axis_check", "lightmap", "warnings")}))
