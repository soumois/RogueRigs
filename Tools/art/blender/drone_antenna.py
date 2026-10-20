"""task-252-02: model the drone comms dish (SM_DroneAntenna) as a separate rigid part, then assemble a viewing file. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/drone_antenna.py -- --out-dir Art/Meshes/drone
Part (cm, +X front, Z up, object at identity, FBX_SCALE_UNITS): origin = foot of the mount on the Z spin axis (the part spins around its own Z through the origin).
Look from vault/assets/drone-turnaround.webp: off-white slightly dented dish (20 cm, 5 cm deep, rusty rim) on a short dark bent mount arm, a thin dark feed rod with a small tip out of the dish centre,
the dish axis tilted 30 degrees above horizontal, facing +X. Flat shaded, one material meshy_drone_antenna, per-face flat colours stored as vertex colours (palette below), UV0 planar, UV1 LightMap packed.
Assembled file SM_Drone_all.fbx: SM_Drone_assembled.fbx (base + laser) plus the antenna at ATTACH (the stem head top found by drone_cut_parts.py, base frame). SM_DroneBase and SM_DroneLaser are not touched."""
import bpy, bmesh, sys, os, json, math, struct
import numpy as np
from mathutils import Vector, Matrix
from collections import defaultdict

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out-dir": "Art/Meshes/drone"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out-dir"])
ATTACH = Vector((-4.257, 0.837, 28.752))   # stem head top, base frame (drone_parts.report.json stem_top.head_top_base_frame_cm)
LM = 256
TILT = math.radians(30.0)
R_DISH, DEPTH, WALL = 10.0, 5.0, 0.6
N = 14                                      # dish segments
SEG = 8                                     # mount tube sides

# palette: sRGB (what the viewer shows) -> linear
PAL_SRGB = {"dish": (0.80, 0.77, 0.70), "mount": (0.16, 0.17, 0.19), "rust": (0.42, 0.20, 0.08)}


def lin(c):
    return tuple(round(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92, 4) for x in c)


PAL_LIN = {k: lin(v) for k, v in PAL_SRGB.items()}
PAL_IDX = {"dish": 0, "mount": 1, "rust": 2}

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.context.scene.unit_settings.scale_length = 1.0
bm = bmesh.new()
LAY_C = bm.faces.layers.int.new("pal")


def paint(faces, key):
    for f in faces:
        f[LAY_C] = PAL_IDX[key]


def basis(d):
    d = Vector(d).normalized()
    t = Vector((0, 1, 0)) if abs(d.y) < 0.9 else Vector((1, 0, 0))
    if abs(d.dot(t)) > 0.9:
        t = Vector((0, 0, 1))
    u = (t - d * d.dot(t)).normalized()
    return d, u, d.cross(u)


def loft(rings, d, n, key, caps=True, rot=0.0):
    """closed tube: rings = [(centre Vector, radius)], all perpendicular to d"""
    d, u, w = basis(d)
    vs = []
    for c, r in rings:
        vs.append([bm.verts.new(c + (u * math.cos(2 * math.pi * j / n + rot) + w * math.sin(2 * math.pi * j / n + rot)) * r) for j in range(n)])
    fs = []
    for k in range(len(vs) - 1):
        for j in range(n):
            fs.append(bm.faces.new((vs[k][j], vs[k][(j + 1) % n], vs[k + 1][(j + 1) % n], vs[k + 1][j])))
    if caps:
        fs.append(bm.faces.new(vs[0][::-1]))
        fs.append(bm.faces.new(vs[-1]))
    paint(fs, key)
    return fs


V = Vector
A = V((math.cos(TILT), 0, math.sin(TILT)))           # dish axis (face direction)
UP = V((-math.sin(TILT), 0, math.cos(TILT)))         # dish up in its own frame
Y = V((0, 1, 0))
P0 = V((2.5, 0, 10.8))                               # dish apex (centre of the back of the bowl)
ELBOW = V((0, 0, 7.5))

# ---- mount: flange on the stem top, post, elbow, bent arm, collar behind the dish
loft([(V((0, 0, 0)), 2.2), (V((0, 0, 0.9)), 2.2)], V((0, 0, 1)), SEG, "mount", rot=math.pi / SEG)
loft([(V((0, 0, 0.9)), 1.2), (V((0, 0, 7.0)), 0.9)], V((0, 0, 1)), SEG, "mount")
loft([(V((0, 0, 6.7)), 0.8), (V((0, 0, 7.1)), 1.5), (V((0, 0, 7.9)), 1.5), (V((0, 0, 8.3)), 0.8)], V((0, 0, 1)), SEG, "mount", rot=math.pi / SEG)
arm_end = P0 - A * 0.4
loft([(ELBOW, 0.8), (arm_end, 0.8)], arm_end - ELBOW, SEG, "mount")
loft([(P0 - A * 1.2, 1.5), (P0 + A * 0.4, 1.5)], A, 6, "mount")

# ---- dish: closed shell (inner surface, outer surface, rim band); rings at 0, .4, .75, 1.0 of the radius, paraboloid depth
FR = [0.0, 0.4, 0.75, 1.0]


def dish_pt(s, ang, t_off=0.0, s_scale=1.0, dt=0.0):
    t = DEPTH * (s / R_DISH) ** 2
    return P0 + A * (t + t_off + dt) + (UP * math.cos(ang) + Y * math.sin(ang)) * (s * s_scale)


# dents: (segment, radial factor, axial shift) on the rim ring and a softer one on ring 2
DENT = {3: (0.86, 0.5), 4: (0.93, 0.25), 9: (0.95, -0.6), 10: (0.9, -0.3)}
inner, outer = [], []
for k, fr in enumerate(FR):
    s = R_DISH * fr
    if k == 0:
        inner.append([bm.verts.new(dish_pt(0, 0))])
        outer.append([bm.verts.new(dish_pt(0, 0, -WALL))])
        continue
    ri, ro = [], []
    for j in range(N):
        ang = 2 * math.pi * j / N
        sc, dz = 1.0, 0.0
        if j in DENT and k >= 2:
            f_, dz_ = DENT[j]
            sc = 1 - (1 - f_) * (1.0 if k == 3 else 0.4)
            dz = dz_ * (1.0 if k == 3 else 0.4)
        ri.append(bm.verts.new(dish_pt(s, ang, 0.0, sc, dz)))
        ro.append(bm.verts.new(dish_pt(s * (0.96 if k == 3 else 1.0), ang, -WALL, sc, dz)))
    inner.append(ri)
    outer.append(ro)
dish_faces, rim_faces, rim_outer_faces = [], [], []
for surf, flip in ((inner, False), (outer, True)):
    for j in range(N):
        a, b = surf[1][j], surf[1][(j + 1) % N]
        tri = (surf[0][0], a, b)
        # inner faces point along +A (concave side), outer along -A
        dish_faces.append(bm.faces.new(tri[::-1] if flip else tri))
    for k in range(1, len(FR) - 1):
        for j in range(N):
            q = (surf[k][j], surf[k][(j + 1) % N], surf[k + 1][(j + 1) % N], surf[k + 1][j])
            f = bm.faces.new(q[::-1] if flip else q)
            dish_faces.append(f)
            if flip and k == len(FR) - 2 and j % 3 == 0:
                rim_outer_faces.append(f)
for j in range(N):
    rim_faces.append(bm.faces.new((inner[-1][j], inner[-1][(j + 1) % N], outer[-1][(j + 1) % N], outer[-1][j])))
paint(dish_faces, "dish")
paint(rim_outer_faces, "rust")
paint(rim_faces, "rust")

# ---- feed rod from the dish centre along the axis, small tip
ROD_L = 9.0
loft([(P0 + A * 0.2, 0.35), (P0 + A * ROD_L, 0.35)], A, 6, "mount")
loft([(P0 + A * (ROD_L - 0.2), 0.7), (P0 + A * (ROD_L + 0.7), 0.7)], A, 6, "rust")
loft([(P0 + A * 3.0, 0.55), (P0 + A * 3.8, 0.55)], A, 6, "mount")

bmesh.ops.recalc_face_normals(bm, faces=bm.faces[:])
bmesh.ops.triangulate(bm, faces=bm.faces[:])
bm.faces.ensure_lookup_table()
bm.normal_update()

# ---- UV0 (planar by dominant axis, normalised into 0..1) and UV1 lightmap (one island per face, packed)
uv0 = bm.loops.layers.uv.new("UVMap")
co = np.array([v.co[:] for v in bm.verts])
lo_, hi_ = co.min(0), co.max(0)
span = np.maximum(hi_ - lo_, 1e-6)
for f in bm.faces:
    ax = int(np.argmax([abs(f.normal.x), abs(f.normal.y), abs(f.normal.z)]))
    a_, b_ = [i for i in range(3) if i != ax]
    for l in f.loops:
        p = l.vert.co
        l[uv0].uv = ((p[a_] - lo_[a_]) / span[a_], (p[b_] - lo_[b_]) / span[b_])

uv1 = bm.loops.layers.uv.new("LightMap")
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
PAD = 2.0


def pack(sc):
    x = y = PAD
    rowh = 0.0
    res = {}
    for i in order:
        _, w, h = island[i]
        pw, ph = w * sc, h * sc
        if x + pw + PAD > LM:
            x = PAD
            y += rowh + PAD
            rowh = 0.0
        if x + pw + PAD > LM or y + ph + PAD > LM:
            return None
        res[i] = (x, y)
        x += pw + PAD
        rowh = max(rowh, ph)
    return res


lo2, hi2 = 1e-4, 50.0
for _ in range(50):
    mid = (lo2 + hi2) / 2
    if pack(mid):
        lo2 = mid
    else:
        hi2 = mid
placed = pack(lo2)
for f in bm.faces:
    x, y = placed[f.index]
    for l, (lx, ly) in zip(f.loops, island[f.index][0]):
        l[uv1].uv = ((x + lx * lo2) / LM, (y + ly * lo2) / LM)

# ---- mesh, material, vertex colours
me = bpy.data.meshes.new("SM_DroneAntenna")
pal_face = [f[LAY_C] for f in bm.faces]
bm.to_mesh(me)
bm.free()
ca = me.color_attributes.new("Col", "FLOAT_COLOR", "CORNER")
keys = list(PAL_IDX)
for p in me.polygons:
    c = PAL_LIN[keys[pal_face[p.index]]]
    for li in p.loop_indices:
        ca.data[li].color = (c[0], c[1], c[2], 1.0)
mat = bpy.data.materials.new("meshy_drone_antenna")
mat.use_nodes = True
pb = [n for n in mat.node_tree.nodes if n.type == "BSDF_PRINCIPLED"][0]
pb.inputs["Base Color"].default_value = (*PAL_LIN["dish"], 1.0)
pb.inputs["Roughness"].default_value = 0.7
mat.diffuse_color = (*PAL_LIN["dish"], 1.0)
vc = mat.node_tree.nodes.new("ShaderNodeVertexColor")
vc.layer_name = "Col"
mat.node_tree.links.new(vc.outputs["Color"], pb.inputs["Base Color"])
me.materials.append(mat)
me.uv_layers.active_index = 0
ob = bpy.data.objects.new("SM_DroneAntenna", me)
bpy.context.scene.collection.objects.link(ob)
bpy.context.view_layer.objects.active = ob
ob.select_set(True)
me.shade_flat()

EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
ANT = os.path.join(OUT, "SM_DroneAntenna.fbx")
bpy.ops.export_scene.fbx(filepath=ANT, **EXPORT)


def unit_scale(path):
    b_ = open(path, "rb").read()
    j = b_.find(b"UnitScaleFactor")
    k = b_.find(b"D", j + 20)
    return struct.unpack("<d", b_[k + 1:k + 9])[0]


def tri_count(m):
    return sum(len(p.vertices) - 2 for p in m.polygons)


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.data.objects if o.type == "MESH"]


# ---- read back the antenna and check
o = import_fbx(ANT)[0]
m = o.data
W = np.array([(o.matrix_world @ v.co)[:] for v in m.vertices])
ec = defaultdict(int)
for p in m.polygons:
    for k in p.edge_keys:
        ec[k] += 1
u0 = np.array([l.uv[:] for l in m.uv_layers[0].data])
u1 = np.array([l.uv[:] for l in m.uv_layers["LightMap"].data]).reshape(-1, 3, 2) * LM
bmin, bmax = u1.min(1), u1.max(1)
ov, gap = 0, 1e9
for i in range(len(u1) - 1):
    gx = np.maximum(bmin[i + 1:, 0] - bmax[i, 0], bmin[i, 0] - bmax[i + 1:, 0])
    gy = np.maximum(bmin[i + 1:, 1] - bmax[i, 1], bmin[i, 1] - bmax[i + 1:, 1])
    g = np.maximum(gx, gy)
    ov += int((g < 0).sum())
    gap = min(gap, float(g.min()))
cn = np.array([c.vector[:] for c in m.corner_normals])
fn = np.zeros((len(m.loops), 3))
for p in m.polygons:
    for li in p.loop_indices:
        fn[li] = p.normal
flat = float((cn * fn).sum(1).min())
vol = 0.0   # signed volume > 0 = outward normals
for p in m.polygons:
    a, b, c = [np.array(m.vertices[i].co[:]) for i in p.vertices]
    vol += np.dot(a, np.cross(b, c)) / 6
cols = [tuple(round(x, 3) for x in ca2.color[:3]) for ca2 in m.color_attributes[0].data] if m.color_attributes else []
cols_used = sorted(set(cols))
axis_r = np.hypot(W[:, 0], W[:, 1])
foot = W[W[:, 2] < 0.01]
rep = dict(
    task="task-252-02-drone-mesh-parts", part="SM_DroneAntenna", frame="cm, front +X, Z up, FBX_SCALE_UNITS (UnitScaleFactor %s), object at identity" % unit_scale(ANT),
    tris=tri_count(m), vertices=len(m.vertices), limit=500,
    bounds_min_cm=[round(float(x), 3) for x in W.min(0)], bounds_max_cm=[round(float(x), 3) for x in W.max(0)], size_cm=[round(float(x), 3) for x in (W.max(0) - W.min(0))],
    pivot="object origin = foot of the mount: centre of the flange bottom disc (z = 0) on the vertical Z spin axis; flange radius 2.2 cm sits on the stem top; spin = rotation about the object Z through the origin",
    foot_centre_cm=[round(float(foot[:, 0].mean()), 4), round(float(foot[:, 1].mean()), 4), round(float(foot[:, 2].mean()), 4)], foot_radius_cm=round(float(np.hypot(foot[:, 0], foot[:, 1]).max()), 3),
    spin_radius_cm=round(float(axis_r.max()), 3),
    dish=dict(diameter_cm=R_DISH * 2, depth_cm=DEPTH, wall_cm=WALL, axis_tilt_above_horizontal_deg=30.0, face_direction="+X and up (rotates with the spin)", apex_cm=[round(x, 3) for x in P0], segments=N, dents="segments 3, 4, 9, 10 of the rim pushed in or out"),
    attach_point_base_frame_cm=[ATTACH.x, ATTACH.y, ATTACH.z],
    materials=[mt.name for mt in m.materials], uv_layers=[u.name for u in m.uv_layers],
    colours=dict(note="flat colours as vertex colour attribute Col (linear RGB stored in the FBX), material meshy_drone_antenna; per face palette", linear_rgb=PAL_LIN, srgb=PAL_SRGB,
                 use={"dish": "bowl inner and outer faces (off-white, as the base white)", "mount": "flange, post, elbow, arm, collar, feed rod (dark gunmetal)", "rust": "rim band, every third outer rim facet, feed tip"},
                 readback_distinct_corner_colours=[list(c) for c in cols_used]),
    checks=dict(open_edges=sum(1 for v in ec.values() if v == 1), nonmanifold_edges=sum(1 for v in ec.values() if v > 2), signed_volume_cm3=round(float(vol), 2),
                uv0_range=dict(u=[round(float(u0[:, 0].min()), 5), round(float(u0[:, 0].max()), 5)], v=[round(float(u0[:, 1].min()), 5), round(float(u0[:, 1].max()), 5)]),
                uv1_lightmap=dict(overlapping_pairs_bbox=ov, min_gap_px=round(gap, 3), in_range=bool(u1.min() >= 0 and u1.max() <= LM), lightmap_res=LM, passed=bool(ov == 0 and gap >= 1.0 and u1.min() >= 0 and u1.max() <= LM)),
                flat_min_dot=round(flat, 6)),
    warnings=[])
if rep["tris"] > 500:
    rep["warnings"].append("above 500 triangles")
if rep["checks"]["open_edges"] or rep["checks"]["nonmanifold_edges"]:
    rep["warnings"].append("open or non-manifold edges")
if vol <= 0:
    rep["warnings"].append("normals point inward")
if abs(rep["foot_centre_cm"][0]) > 1e-3 or abs(rep["foot_centre_cm"][1]) > 1e-3 or abs(W[:, 2].min()) > 1e-3:
    rep["warnings"].append("foot not centred on the origin")

# ---- assembled viewing file: base + laser (SM_Drone_assembled.fbx) + antenna at ATTACH
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.join(OUT, "SM_Drone_assembled.fbx"))
base_objs = [x for x in bpy.data.objects if x.type == "MESH"]
bpy.ops.import_scene.fbx(filepath=ANT)
ant = [x for x in bpy.data.objects if x.type == "MESH" and x not in base_objs][0]
ant.location = ATTACH
bpy.context.view_layer.update()
base_name = [x for x in base_objs if "Base" in x.name][0]
Bw = np.array([(base_name.matrix_world @ v.co)[:] for v in base_name.data.vertices])
Aw = np.array([(ant.matrix_world @ v.co)[:] for v in ant.data.vertices])
near = Bw[(np.hypot(Bw[:, 0] - ATTACH.x, Bw[:, 1] - ATTACH.y) < 2.2) & (Bw[:, 2] > 25.0) & (Bw[:, 2] < ATTACH.z + 0.6)]
head_top = float(near[:, 2].max()) if len(near) else None
nub_in = Bw[(np.hypot(Bw[:, 0] - ATTACH.x, Bw[:, 1] - ATTACH.y) < 2.2) & (Bw[:, 2] > ATTACH.z)]
nub_r = float(np.hypot(nub_in[:, 0] - ATTACH.x, nub_in[:, 1] - ATTACH.y).max()) if len(nub_in) else 0.0
bpy.ops.object.select_all(action="SELECT")
bpy.ops.export_scene.fbx(filepath=os.path.join(OUT, "SM_Drone_all.fbx"), **EXPORT)
Aall = np.concatenate([Bw, Aw] + [np.array([(x.matrix_world @ v.co)[:] for v in x.data.vertices]) for x in base_objs if x is not base_name])
rep["assembly"] = dict(file="SM_Drone_all.fbx", antenna_location_cm=[ATTACH.x, ATTACH.y, ATTACH.z], antenna_foot_z=round(float(Aw[:, 2].min()), 4),
                       base_head_top_z_near_foot=None if head_top is None else round(head_top, 4), foot_to_stem_top_gap_cm=None if head_top is None else round(abs(float(Aw[:, 2].min()) - head_top), 4),
                       stem_part_above_foot_max_radius_from_axis_cm=round(nub_r, 3), flange_radius_cm=2.2,
                       bounds_min_cm=[round(float(x), 3) for x in Aall.min(0)], bounds_max_cm=[round(float(x), 3) for x in Aall.max(0)],
                       note="the Meshy nub (tip z 30.17) ends inside the flange and post; base and laser files unchanged")
with open(os.path.join(OUT, "drone_antenna.report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("DONE", json.dumps(dict(tris=rep["tris"], size=rep["size_cm"], warnings=rep["warnings"], open=rep["checks"]["open_edges"], gap=rep["assembly"]["foot_to_stem_top_gap_cm"], vol=rep["checks"]["signed_volume_cm3"])))
