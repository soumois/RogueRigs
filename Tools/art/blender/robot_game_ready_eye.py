"""task-283-02: game-ready robot mesh with a robot_eye slot in one headless run (heavy first; arguments keep it reusable for the other Meshy robots).
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/robot_game_ready_eye.py -- --in-fbx <flat.fbx> --in-tex <clean basecolor png> --out-dir <dir> --name heavy_meshy_game
      --height-cm 248 [--object SM_Heavy_Meshy] [--eye-zmin <cm>] [--eye-zmax <cm>] [--eye-xmin <cm>] [--no-write]
Same transform, LightMap UV1 and FBX export as meshy_game_ready.py (uniform scale to the height, +90 deg about Z so front -Y -> +X, origin on the floor under the
feet midpoint, UV0 untouched, flat shading, FBX_SCALE_UNITS), but the single Meshy material (4 texture nodes) is replaced by two clean slots: body
(Principled, base colour texture on UV0, referenced not embedded) then robot_eye (plain material, no texture). Eye faces = faces whose UV0 triangle covers a texel
with linear mask saturate((R - 2 max(G,B)) x 3) > 0.1 and whose centroid is inside the zone (game cm, defaults: whole mesh). Reports every red texel not in an eye face.
Writes <name>.fbx and <name>.report.json. Nothing is imported in Unreal."""
import bpy, bmesh, sys, os, json, shutil, struct
import numpy as np
from mathutils import Vector, Matrix

a = sys.argv[sys.argv.index("--") + 1:]


def arg(n, d=None):
    return a[a.index(n) + 1] if n in a else d


IN_FBX = os.path.abspath(arg("--in-fbx"))
IN_TEX = os.path.abspath(arg("--in-tex"))
OUT = os.path.abspath(arg("--out-dir"))
NAME = arg("--name", "heavy_meshy_game")
OBJ = arg("--object", "SM_Heavy_Meshy")
HEIGHT = float(arg("--height-cm", 248))
ZMIN, ZMAX, XMIN = float(arg("--eye-zmin", -1e9)), float(arg("--eye-zmax", 1e9)), float(arg("--eye-xmin", -1e9))
TEXNAME = os.path.basename(IN_TEX)
WORK = os.path.abspath(arg("--work", "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work_eye"))
os.makedirs(WORK, exist_ok=True)
FBX = os.path.join(WORK, NAME + ".fbx")
LM = 2048
THR = 0.1


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.data.objects if o.type == "MESH"]


def world_v(ob):
    mw = np.array(ob.matrix_world)
    co = np.empty(len(ob.data.vertices) * 3)
    ob.data.vertices.foreach_get("co", co)
    return co.reshape(-1, 3) @ mw[:3, :3].T + mw[:3, 3]


def tri_arrays(ob):
    m = ob.data
    v = world_v(ob)
    vi = np.empty(len(m.loops), np.int64)
    m.loops.foreach_get("vertex_index", vi)
    uv = np.empty(len(m.loops) * 2)
    m.uv_layers[0].data.foreach_get("uv", uv)
    return v, vi.reshape(-1, 3), uv.reshape(-1, 3, 2)


obs = import_fbx(IN_FBX)
assert len(obs) == 1
ob = obs[0]
me = ob.data
V0, VI0, UV_IN = tri_arrays(ob)
assert all(len(p.vertices) == 3 for p in me.polygons)
h_in = float(V0[:, 2].max() - V0[:, 2].min())
tris_in = len(me.polygons)
verts_in = len(me.vertices)

# ---------------------------------------------------------------- transform
scale = HEIGHT / h_in
mw = Matrix.Rotation(np.radians(90.0), 4, "Z") @ Matrix.Scale(scale, 4) @ ob.matrix_world
v = np.array([(mw @ x.co)[:] for x in me.vertices])
zmin = v[:, 2].min()
low = v[v[:, 2] < zmin + 0.03 * HEIGHT]
feet_mid = [float((low[:, 0].min() + low[:, 0].max()) / 2), float((low[:, 1].min() + low[:, 1].max()) / 2)]
off = Vector((-feet_mid[0], -feet_mid[1], -zmin))
for x in me.vertices:
    x.co = mw @ x.co + off
ob.matrix_world = Matrix.Identity(4)
ob.name = OBJ
me.name = OBJ
for p in me.polygons:
    p.use_smooth = False
me.update()

# ---------------------------------------------------------------- eye faces
img = bpy.data.images.load(IN_TEX, check_existing=False)
img.colorspace_settings.name = "Non-Color"
W, H = img.size
px = np.empty(W * H * 4, np.float32)
img.pixels.foreach_get(px)
px = px.reshape(H, W, 4)[:, :, :3]
lin = np.where(px <= 0.04045, px / 12.92, ((px + 0.055) / 1.055) ** 2.4)
mask = np.clip((lin[:, :, 0] - 2 * np.maximum(lin[:, :, 1], lin[:, :, 2])) * 3, 0, 1)
RED = (mask > THR).reshape(-1)


def cover(uv3):
    p = uv3 * np.array([W, H])
    out = set()
    for q in list(p) + [p.mean(0)]:
        x, y = int(np.floor(q[0])), int(np.floor(q[1]))
        if 0 <= x < W and 0 <= y < H:
            out.add(y * W + x)
    x0, x1 = int(np.floor(p[:, 0].min())), int(np.ceil(p[:, 0].max()))
    y0, y1 = int(np.floor(p[:, 1].min())), int(np.ceil(p[:, 1].max()))
    x0, y0 = max(x0, 0), max(y0, 0)
    x1, y1 = min(x1, W - 1), min(y1, H - 1)
    if x1 >= x0 and y1 >= y0:
        xs, ys = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        A, B, C = p
        d = (B[1] - C[1]) * (A[0] - C[0]) + (C[0] - B[0]) * (A[1] - C[1])
        if abs(d) > 1e-12:
            l1 = ((B[1] - C[1]) * (xs - C[0]) + (C[0] - B[0]) * (ys - C[1])) / d
            l2 = ((C[1] - A[1]) * (xs - C[0]) + (A[0] - C[0]) * (ys - C[1])) / d
            ins = (l1 >= 0) & (l2 >= 0) & (1 - l1 - l2 >= 0)
            for yy, xx in zip(ys[ins], xs[ins]):
                out.add(int(yy) * W + int(xx))
    return out


V1, VI1, UV1_ = tri_arrays(ob)
fcen = V1[VI1].mean(1)
eye_idx, eye_cov, body_red = [], set(), set()
red_cov_faces = []
for i in range(len(VI1)):
    ids = np.fromiter(cover(UV1_[i].astype(np.float64)), np.int64)
    red = ids[RED[ids]] if len(ids) else ids
    if not len(red):
        continue
    c = fcen[i]
    red_cov_faces.append(i)
    if ZMIN <= c[2] <= ZMAX and c[0] >= XMIN:
        eye_idx.append(i)
        eye_cov |= set(red.tolist())
    else:
        body_red |= set(red.tolist())
eye_idx = np.array(eye_idx, int)
par = {}


def find(x):
    while par.setdefault(x, x) != x:
        par[x] = par[par[x]]
        x = par[x]
    return x


for i in eye_idx:
    for vv in VI1[i]:
        par[find(("f", int(i)))] = find(("v", int(vv)))
comp = {}
for i in eye_idx:
    comp.setdefault(find(("f", int(i))), []).append(int(i))
comps = [dict(faces=len(f), min_cm=[round(float(x), 1) for x in fcen[f].min(0)], max_cm=[round(float(x), 1) for x in fcen[f].max(0)])
         for f in sorted(comp.values(), key=len, reverse=True)]
allred = np.fromiter(red_cov_faces, int)
print("red-covering faces", len(allred), "extent", fcen[allred].min(0), fcen[allred].max(0), "eye faces", len(eye_idx), "components", len(comps))
print("components", json.dumps(comps[:10]))
print("red texels in non-eye faces", len(body_red))

# ---------------------------------------------------------------- materials: body, robot_eye
body = bpy.data.materials.new("body")
body.use_nodes = True
nt = body.node_tree
tx = nt.nodes.new("ShaderNodeTexImage")
shutil.copyfile(IN_TEX, os.path.join(WORK, TEXNAME))
bimg = bpy.data.images.load(os.path.join(WORK, TEXNAME), check_existing=False)
bimg.colorspace_settings.name = "sRGB"
tx.image = bimg
nt.links.new(tx.outputs["Color"], nt.nodes["Principled BSDF"].inputs["Base Color"])
eye = bpy.data.materials.new("robot_eye")
eye.use_nodes = True
me.materials.clear()
me.materials.append(body)
me.materials.append(eye)
mi = np.zeros(len(me.polygons), np.int32)
mi[eye_idx] = 1
me.polygons.foreach_set("material_index", mi)

# ---------------------------------------------------------------- LightMap UV1
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
    mx_, my_ = min(q[0] for q in loc), min(q[1] for q in loc)
    loc = [(q[0] - mx_, q[1] - my_) for q in loc]
    island.append((loc, max(q[0] for q in loc), max(q[1] for q in loc)))
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


def lm_check(o):
    u1 = np.array([l.uv[:] for l in o.data.uv_layers["LightMap"].data]).reshape(-1, 3, 2) * LM
    bmin, bmax = u1.min(1), u1.max(1)
    ov = 0
    mg = 1e9
    for i in range(len(u1) - 1):
        gx = np.maximum(bmin[i + 1:, 0] - bmax[i, 0], bmin[i, 0] - bmax[i + 1:, 0])
        gy = np.maximum(bmin[i + 1:, 1] - bmax[i, 1], bmin[i, 1] - bmax[i + 1:, 1])
        g = np.maximum(gx, gy)
        ov += int((g < 0).sum())
        mg = min(mg, float(g.min()))
    border = float(min(bmin.min(), (LM - bmax).min()))
    return dict(layer="LightMap", atlas_px=LM, faces=len(u1), island_scale_px_per_cm=round(lo, 5), method="one island per face, shelf packed, bbox test",
                overlapping_face_pairs_bbox=ov, min_gap_px=round(mg, 3), min_border_px=round(border, 3),
                bounds_min=round(float(u1.min() / LM), 5), bounds_max=round(float(u1.max() / LM), 5),
                passed=bool(ov == 0 and mg >= 1.0 and u1.min() >= 0 and u1.max() <= LM))


# ---------------------------------------------------------------- export
bpy.context.scene.unit_settings.scale_length = 1.0
bpy.ops.object.select_all(action="DESELECT")
ob.select_set(True)
bpy.context.view_layer.objects.active = ob
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
bpy.ops.export_scene.fbx(filepath=FBX, **EXPORT)
b = open(FBX, "rb").read()
usf = None
j = b.find(b"UnitScaleFactor")
if j >= 0:
    k = b.find(b"D", j + 20)
    usf = struct.unpack("<d", b[k + 1:k + 9])[0]
from io_scene_fbx import parse_fbx
FILE_MATS = []
for el in parse_fbx.parse(FBX)[0].elems:
    if el.id == b"Objects":
        FILE_MATS = [g.props[1].split(bytes(1))[0].decode() for g in el.elems if g.id == b"Material"]

# ---------------------------------------------------------------- read back
obs2 = import_fbx(FBX)
ob2 = obs2[0]
m2 = ob2.data
V2, VI2, UV2 = tri_arrays(ob2)
lm2 = lm_check(ob2)
bb_min, bb_max = V2.min(0), V2.max(0)
size = bb_max - bb_min
mats2 = [m.name if m else None for m in m2.materials]
mi2 = np.empty(len(m2.polygons), np.int64)
m2.polygons.foreach_get("material_index", mi2)
imgs2 = sorted({n.image.filepath for m in m2.materials for n in m.node_tree.nodes if n.type == "TEX_IMAGE" and n.image})
uv_direct = float(np.abs(UV2 - UV_IN).max()) if UV2.shape == UV_IN.shape else None
Rz = np.array([[0, -1, 0], [1, 0, 0], [0, 0, 1]], float)
exp = (V0 * scale) @ Rz.T + np.array(off[:])
kA = np.round(np.hstack([exp[VI0].reshape(-1, 3), UV_IN.reshape(-1, 2)]), 2)
kA = kA[np.lexsort(kA.T[::-1])]
kB = np.round(np.hstack([V2[VI2].reshape(-1, 3), UV2.reshape(-1, 2)]), 2)
kB = kB[np.lexsort(kB.T[::-1])]
fc2 = V2[VI2].mean(1)
eye2 = (mi2 == mats2.index("robot_eye")) if "robot_eye" in mats2 else np.zeros(len(mi2), bool)
body_red2, eye_cov2, body_red_zone = set(), set(), set()
for i in range(len(VI2)):
    ids = np.fromiter(cover(UV2[i].astype(np.float64)), np.int64)
    red = ids[RED[ids]] if len(ids) else ids
    (eye_cov2 if eye2[i] else body_red2).update(red.tolist())
    if not eye2[i] and ZMIN <= fc2[i][2] <= ZMAX and fc2[i][0] >= XMIN:
        body_red_zone.update(red.tolist())
sole = V2[V2[:, 2] < 0.03 * HEIGHT]
red_c = fc2[eye2].mean(0)
body_c = V2.mean(0)
rep = dict(
    inputs=dict(fbx=os.path.basename(IN_FBX), texture=TEXNAME, input_height_units=round(h_in, 5), input_triangles=tris_in, input_vertices=verts_in),
    outputs=dict(fbx=NAME + ".fbx", texture_referenced=TEXNAME, embedded=False, texture_paths_in_fbx=imgs2),
    scale_factor=round(scale, 6), rotation_about_z_deg=90, applied_origin_offset_cm=[round(x, 4) for x in off[:]],
    export_settings=dict(**{k_: (sorted(v_) if isinstance(v_, set) else v_) for k_, v_ in EXPORT.items()}, blender=bpy.app.version_string, scene_unit_scale_length=1.0),
    axis_note="Blender side: X forward (robot front), Y left, Z up. Exporter axis_forward=-Z, axis_up=Y (same as chaser): Unreal maps it to X forward, Z up. Not verified in Unreal (nothing imported).",
    fbx_unit_scale_factor_in_file=usf,
    readback=dict(bounds_min_cm=[round(float(x), 3) for x in bb_min], bounds_max_cm=[round(float(x), 3) for x in bb_max], size_cm=[round(float(x), 3) for x in size],
                  height_cm=round(float(size[2]), 3), triangles=len(VI2), vertices=len(m2.vertices), objects=len(obs2), object_name=ob2.name,
                  material_slots=mats2, fbx_file_material_order=FILE_MATS, uv_layers=[u.name for u in m2.uv_layers], flat_shaded=not any(p.use_smooth for p in m2.polygons)),
    origin=dict(floor_z_cm=round(float(bb_min[2]), 4),
                feet_midpoint_xy_cm=[round(float(sole[:, 0].min() + sole[:, 0].max()) / 2, 4), round(float(sole[:, 1].min() + sole[:, 1].max()) / 2, 4)]),
    front_axis=dict(eye_centroid_cm=[round(float(x), 2) for x in red_c], mesh_centroid_cm=[round(float(x), 2) for x in body_c], front_is_plus_x=bool(red_c[0] > body_c[0] + 1.0),
                    note="the eye (red faces) lies on +X in Blender; the raw mesh faced -Y"),
    eye=dict(criterion="UV0 triangle covers a texel with linear saturate((R - 2 max(G,B)) x 3) > 0.1 (texel centres inside + texels of vertices and centroid) and face centroid inside the zone",
             zone_game_cm=dict(zmin=ZMIN, zmax=ZMAX, xmin=XMIN), texture=TEXNAME, red_texels_total=int(RED.sum()),
             eye_faces=int(eye2.sum()), eye_faces_before_export=int(len(eye_idx)), eye_texels_in_eye_faces=len(eye_cov2),
             red_texels_left_in_body_faces_in_zone=len(body_red_zone), red_texels_in_body_faces_outside_zone=len(body_red2 - body_red_zone), body_faces_outside_zone_covering_red=int(len(allred) - len(eye_idx)), components=comps[:12], component_count=len(comps)),
    uv0_unchanged=dict(max_abs_diff_direct=uv_direct,
                       vertices_equal=len(m2.vertices) == verts_in, triangles_equal=len(VI2) == tris_in),
    lightmap_uv1=lm2, warnings=[])
if abs(size[2] - HEIGHT) > 1.0:
    rep["warnings"].append("height off by more than 1 cm")
if len(VI2) != tris_in:
    rep["warnings"].append("triangle count changed")
if mats2 != ["body", "robot_eye"]:
    rep["warnings"].append("slot order wrong")
if len(body_red_zone):
    rep["warnings"].append("red texels left in body faces in the eye zone")
if len(body_red2):
    rep["warnings"].append("%d red texels (stray red spots, not the eye) stay in body faces outside the zone" % len(body_red2))
if not lm2["passed"]:
    rep["warnings"].append("lightmap check failed")
os.makedirs(OUT, exist_ok=True)
if "--no-write" not in a:
    shutil.copyfile(FBX, os.path.join(OUT, NAME + ".fbx"))
    with open(os.path.join(OUT, NAME + ".report.json"), "w", encoding="utf-8") as f:
        json.dump(rep, f, indent=1)
print("DONE", json.dumps({k_: rep[k_] for k_ in ("readback", "origin", "front_axis", "uv0_unchanged", "warnings")}))
print("EYE", json.dumps({k_: v_ for k_, v_ in rep["eye"].items() if k_ != "components"}), "LM", json.dumps(lm2))
