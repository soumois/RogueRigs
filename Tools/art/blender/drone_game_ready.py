"""task-252-01: game-ready version of the Meshy drone (body + laser, no dish). Geometry and UV0 are kept as Meshy made them. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/drone_game_ready.py -- --in-dir Art/Source/drone-meshy/meshy --out-dir Art/Meshes/drone/stage [--width-cm 70] [--max-tris 5800] [--keep-smooth]
Steps: uniform scale so the width along the pod axis (pods included) = --width-cm (1 Blender unit = 1 cm), rotation +90 deg about Z (Meshy front -Y -> +X, pod axis X -> Y),
origin: x and y at the middle of the bounding box, z = 0 at the lowest point (emitter tip), transforms applied, flat shading (art guide A1; --keep-smooth keeps the Meshy smooth normals),
decimation (Collapse, UVs interpolated) only when the triangle count is above --max-tris, UV1 'LightMap' (one island per face, shelf packed, 2048 atlas), one material 'meshy_drone'
(base colour, normal map, metallic-roughness map copied next to the FBX), FBX_SCALE_UNITS. Writes drone_game.fbx and drone_game.report.json (read back from the FBX).
Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, bmesh, sys, os, json, time, shutil, struct
import numpy as np
from mathutils import Vector, Matrix, kdtree

T0 = time.time()
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
KEEP_SMOOTH = "--keep-smooth" in argv
argv = [a for a in argv if a != "--keep-smooth"]
opt = {"--in-dir": "", "--out-dir": "", "--width-cm": "70", "--max-tris": "5800", "--name": "Meshy_AI_Rusty_Satellite_Drone_1020195641_texture"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-dir", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
IN = os.path.abspath(opt["--in-dir"])
OUT = os.path.abspath(opt["--out-dir"])
WIDTH = float(opt["--width-cm"])
MAXT = int(opt["--max-tris"])
N = opt["--name"]
LM = 2048
TEX = {"basecolor": (N + ".png", "drone_basecolor.png"), "normal": (N + "_normal.png", "drone_normal.png"),
       "metallic_roughness": (N + "_metallic_roughness.png", "drone_metallic_roughness.png")}
os.makedirs(OUT, exist_ok=True)
for k, (src, dst) in TEX.items():
    shutil.copyfile(os.path.join(IN, src), os.path.join(OUT, dst))
OUT_NAME = "drone_game"


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.data.objects if o.type == "MESH"]


def tris(m):
    return sum(len(p.vertices) - 2 for p in m.polygons)


def geom(ob):
    m = ob.data
    v = np.array([(ob.matrix_world @ x.co)[:] for x in m.vertices])
    uv = np.array([l.uv[:] for l in m.uv_layers[0].data])
    return v, uv


obs = import_fbx(os.path.join(IN, N + ".fbx"))
assert len(obs) == 1
ob = obs[0]
me = ob.data
T_IN = tris(me)
V_IN, UV_IN = geom(ob)
CORNERS_IN = [(me.loops[i].vertex_index, tuple(d.uv)) for i, d in enumerate(me.uv_layers[0].data)]
mwi = ob.matrix_world.copy()
raw_size = V_IN.max(0) - V_IN.min(0)
# pod axis = world X of the imported mesh
scale = WIDTH / raw_size[0]
R = Matrix.Rotation(np.radians(90.0), 4, "Z") @ Matrix.Scale(scale, 4)
for x in me.vertices:
    x.co = R @ (mwi @ x.co)
ob.matrix_world = Matrix.Identity(4)
ob.rotation_euler = (0, 0, 0)
ob.scale = (1, 1, 1)
ob.location = (0, 0, 0)
bpy.context.view_layer.update()
v = np.array([x.co[:] for x in me.vertices])
off = Vector((-(v[:, 0].min() + v[:, 0].max()) / 2, -(v[:, 1].min() + v[:, 1].max()) / 2, -v[:, 2].min()))
for x in me.vertices:
    x.co = x.co + off
me.name = ob.name = "SM_Drone"

decimated = None
if tris(me) > MAXT:
    md = ob.modifiers.new("dec", "DECIMATE")
    md.decimate_type = "COLLAPSE"
    md.ratio = MAXT / tris(me) * 0.98
    md.use_collapse_triangulate = True
    bpy.context.view_layer.objects.active = ob
    bpy.ops.object.modifier_apply(modifier=md.name)
    decimated = dict(ratio=round(md.ratio, 4), tris_after=tris(me))
if not KEEP_SMOOTH:
    bpy.context.view_layer.objects.active = ob
    ob.select_set(True)
    bpy.ops.mesh.customdata_custom_splitnormals_clear()      # Meshy FBX carries custom (smooth) normals: they override flat shading
    me.shade_flat()

# ---------------------------------------------------------------- material
mat = bpy.data.materials.new("meshy_drone")
mat.use_nodes = True
nt = mat.node_tree
for n in list(nt.nodes):
    if n.type != "OUTPUT_MATERIAL":
        nt.nodes.remove(n)
out_n = [n for n in nt.nodes if n.type == "OUTPUT_MATERIAL"][0]
pb = nt.nodes.new("ShaderNodeBsdfPrincipled")
nt.links.new(pb.outputs["BSDF"], out_n.inputs["Surface"])


def tex_node(key, cs):
    n = nt.nodes.new("ShaderNodeTexImage")
    img = bpy.data.images.load(os.path.join(OUT, TEX[key][1]), check_existing=False)
    img.colorspace_settings.name = cs
    n.image = img
    n.label = key
    return n


tb = tex_node("basecolor", "sRGB")
nt.links.new(tb.outputs["Color"], pb.inputs["Base Color"])
tn = tex_node("normal", "Non-Color")
nm = nt.nodes.new("ShaderNodeNormalMap")
nt.links.new(tn.outputs["Color"], nm.inputs["Color"])
nt.links.new(nm.outputs["Normal"], pb.inputs["Normal"])
tm = tex_node("metallic_roughness", "Non-Color")
sp = nt.nodes.new("ShaderNodeSeparateColor")
nt.links.new(tm.outputs["Color"], sp.inputs["Color"])
nt.links.new(sp.outputs["Green"], pb.inputs["Roughness"])      # glTF layout: G roughness, B metallic
nt.links.new(sp.outputs["Blue"], pb.inputs["Metallic"])
me.materials.clear()
me.materials.append(mat)

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
    mx_, my_ = min(a for a, b in loc), min(b for a, b in loc)
    loc = [(a - mx_, b - my_) for a, b in loc]
    island.append((loc, max(a for a, b in loc), max(b for a, b in loc)))
order = sorted(range(len(island)), key=lambda i: -island[i][2])


def pack(sc):
    x = y = PAD
    rowh = 0.0
    outp = {}
    for i in order:
        _, w, hh = island[i]
        pw, ph = w * sc, hh * sc
        if x + pw + PAD > LM:
            x = PAD
            y += rowh + PAD
            rowh = 0.0
        if x + pw + PAD > LM or y + ph + PAD > LM:
            return None
        outp[i] = (x, y)
        x += pw + PAD
        rowh = max(rowh, ph)
    return outp


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
if not KEEP_SMOOTH:
    me.shade_flat()
me.uv_layers.active_index = 0

# ---------------------------------------------------------------- export
bpy.context.scene.unit_settings.scale_length = 1.0
bpy.ops.object.select_all(action="DESELECT")
ob.select_set(True)
bpy.context.view_layer.objects.active = ob
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF" if not KEEP_SMOOTH else "FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
FBX = os.path.join(OUT, OUT_NAME + ".fbx")
bpy.ops.export_scene.fbx(filepath=FBX, **EXPORT)
b = open(FBX, "rb").read()
usf = None
j = b.find(b"UnitScaleFactor")
if j >= 0:
    k = b.find(b"D", j + 20)
    usf = struct.unpack("<d", b[k + 1:k + 9])[0]

# ---------------------------------------------------------------- read back and checks
ob2 = import_fbx(FBX)[0]
m2 = ob2.data
V2, UV2 = geom(ob2)
uvn = [u.name for u in m2.uv_layers]
lo_, hi_ = V2.min(0), V2.max(0)
size = hi_ - lo_
# texture mapping: every output corner keeps the UV0 of the nearest input vertex (exact match when nothing is decimated)
Rz = np.array([[0, -1, 0], [1, 0, 0], [0, 0, 1]], float)
Vexp = (V_IN * scale) @ Rz.T + np.array(off[:])
kd = kdtree.KDTree(len(Vexp))
for i, p in enumerate(Vexp):
    kd.insert(Vector(p), i)
kd.balance()
uv_in_by_vert = {}
for vi, uv in CORNERS_IN:
    uv_in_by_vert.setdefault(vi, []).append(uv)
dmax = 0.0
uv_bad = 0
uvl = m2.uv_layers[0].data
for p in m2.polygons:
    for j2 in p.loop_indices:
        co, vi, dist = kd.find(m2.vertices[m2.loops[j2].vertex_index].co)
        dmax = max(dmax, dist)
        cu = uvl[j2].uv
        e = min(max(abs(cu[0] - u[0]), abs(cu[1] - u[1])) for u in uv_in_by_vert[vi])
        if e > 1e-4:
            uv_bad += 1
# amber eye: faces whose base colour texel (at the UV0 centroid) is amber must lie on +X
img = bpy.data.images.load(os.path.join(OUT, TEX["basecolor"][1]), check_existing=False)
tex = np.array(img.pixels[:], dtype=np.float32).reshape(img.size[1], img.size[0], 4)
cen_uv = UV2.reshape(-1, 3, 2).mean(1)
px = tex[np.clip((cen_uv[:, 1] * img.size[1]).astype(int), 0, img.size[1] - 1), np.clip((cen_uv[:, 0] * img.size[0]).astype(int), 0, img.size[0] - 1), :3]
amber = (px[:, 0] > 0.8) & (px[:, 1] > 0.35) & (px[:, 1] < 0.8) & (px[:, 2] < 0.25)
vi_all = np.array([m2.loops[l].vertex_index for p in m2.polygons for l in p.loop_indices])
fc = V2[vi_all].reshape(-1, 3, 3).mean(1)
am_c = fc[amber].mean(0).tolist() if amber.any() else None
u1 = np.array([l.uv[:] for l in m2.uv_layers["LightMap"].data]).reshape(-1, 3, 2) * LM
bmin, bmax = u1.min(1), u1.max(1)
ov = 0
gap = 1e9
for i in range(len(u1) - 1):
    gx = np.maximum(bmin[i + 1:, 0] - bmax[i, 0], bmin[i, 0] - bmax[i + 1:, 0])
    gy = np.maximum(bmin[i + 1:, 1] - bmax[i, 1], bmin[i, 1] - bmax[i + 1:, 1])
    g = np.maximum(gx, gy)
    ov += int((g < 0).sum())
    gap = min(gap, float(g.min()))
u0 = np.array([l.uv[:] for l in m2.uv_layers[0].data])
_cn = np.array([c.vector[:] for c in m2.corner_normals])
_fn = np.zeros((len(m2.loops), 3))
for _p in m2.polygons:
    for _li in _p.loop_indices:
        _fn[_li] = _p.normal
flat_min_dot = float((_cn * _fn).sum(1).min())
rep = dict(
    inputs=dict(fbx=N + ".fbx", triangles=T_IN, vertices=int(len(V_IN)), raw_size_units=[round(float(x), 4) for x in raw_size],
                note="Meshy FBX imports with a -90 deg X object rotation (Y-up file); world axes: pods along X, camera eye on -Y, Z up"),
    scale_factor=round(scale, 6), rotation_about_z_deg=90, applied_origin_offset_cm=[round(float(x), 4) for x in off[:]],
    origin="x and y: middle of the bounding box, z = 0 at the lowest point (laser emitter tip)", decimation=decimated or "not needed (input at or below the budget)", max_tris=MAXT,
    flat_shaded=bool(flat_min_dot > 0.999), corner_normal_vs_face_normal_min_dot=round(flat_min_dot, 6), flat_shading_note="Meshy input was smooth shaded; set flat (art guide A1); pass --keep-smooth to keep it",
    export_settings=dict(**{k: (sorted(v) if isinstance(v, set) else v) for k, v in EXPORT.items()}, blender=bpy.app.version_string, fbx_unit_scale_factor_in_file=usf),
    readback=dict(triangles=tris(m2), vertices=len(m2.vertices), bounds_min_cm=[round(float(x), 3) for x in lo_], bounds_max_cm=[round(float(x), 3) for x in hi_],
                  size_cm=[round(float(x), 3) for x in size], uv_layers=uvn, materials=[x.name for x in m2.materials], objects=1),
    uv0_check=dict(range_u=[round(float(u0[:, 0].min()), 5), round(float(u0[:, 0].max()), 5)], range_v=[round(float(u0[:, 1].min()), 5), round(float(u0[:, 1].max()), 5)],
                   corners_not_matching_nearest_meshy_vertex_uv=uv_bad, max_distance_to_meshy_vertex_cm=round(dmax, 6)),
    forward_axis_check=dict(amber_eye_faces=int(amber.sum()), amber_eye_centroid_cm=[round(x, 2) for x in am_c] if am_c else None, mesh_centroid_cm=[round(float(x), 2) for x in V2.mean(0)],
                            amber_eye_on_plus_x=bool(am_c and am_c[0] > V2.mean(0)[0] + 5.0)),
    lightmap=dict(layer="LightMap", atlas_px=LM, faces=len(u1), overlapping_face_pairs_bbox=ov, min_gap_px=round(gap, 3), bounds_min=round(float(u1.min() / LM), 5),
                  bounds_max=round(float(u1.max() / LM), 5), passed=bool(ov == 0 and gap >= 1.0 and u1.min() >= 0 and u1.max() <= LM)),
    textures=sorted(v[1] for v in TEX.values()), warnings=[], seconds=round(time.time() - T0, 1))
if tris(m2) > 6000:
    rep["warnings"].append("above 6000 triangles")
if uv_bad:
    rep["warnings"].append("texture mapping differs from the Meshy UV0 on %d corners" % uv_bad)
if not rep["lightmap"]["passed"]:
    rep["warnings"].append("lightmap check failed")
if abs(size[1] - WIDTH) > 0.5:
    rep["warnings"].append("width off")
with open(os.path.join(OUT, OUT_NAME + ".report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("DONE", json.dumps({k: rep[k] for k in ("readback", "uv0_check", "forward_axis_check", "lightmap", "warnings")}))
