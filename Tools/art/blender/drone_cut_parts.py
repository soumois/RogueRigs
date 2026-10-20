"""task-252-01: cut the game-ready Meshy drone (one fused mesh, drone_game.fbx) into two rigid parts. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/drone_cut_parts.py -- --in-dir Art/Meshes/drone/stage --out-dir Art/Meshes/drone
Parts: SM_DroneBase (shell, side pods and arms, camera, rear box, cables, dish stem) and SM_DroneLaser (the laser assembly hanging under the front-bottom: pitch yoke, block, emitter cage).
Cut (every number found on THIS mesh, stage frame of drone_game.fbx: cm, +X front, +Y left, z = 0 at the emitter tip): one horizontal plane z = 19.0 through the neck of the laser mount, restricted to the zone
box |x| < 9, |y| < 9.6 (the section there is stable from z 17 to 19.5: 46 crossing edges, neck + the two yoke arms, nothing else of the drone). The cut edges are split, every closed loop is filled with cap faces
(holes_fill + triangulate), the mesh is separated by face connectivity. Caps copy the UV0 of the nearest face of the same part touching them (flat colour, texture untouched).
Pivots: SM_DroneBase = centre of the body (x, y = middle of the bounding box, z = middle of the pod height range); SM_DroneLaser = the top of the cut, centre of the neck section (the gimbal point: pitch = rotation about Y).
Each part keeps UV0 (Meshy) and gets its own UV1 'LightMap'. Exports (FBX_SCALE_UNITS, cm): SM_DroneBase.fbx, SM_DroneLaser.fbx (mesh origin = pivot, object at identity), SM_Drone_assembled.fbx (both, laser at its pivot offset),
drone_parts.report.json. Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, bmesh, sys, os, json, time, shutil, struct
import numpy as np
from mathutils import Vector, Matrix
from mathutils.bvhtree import BVHTree
from collections import defaultdict

T0 = time.time()
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--in-dir": "", "--out-dir": "", "--plane-z": "19.0"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-dir", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
IN = os.path.abspath(opt["--in-dir"])
OUT = os.path.abspath(opt["--out-dir"])
PLANE_Z = float(opt["--plane-z"])
ZONE = ((-9.0, -9.6, 8.0), (9.0, 9.6, 30.0))
LM = 2048
TEXS = ["drone_basecolor.png", "drone_normal.png", "drone_metallic_roughness.png"]
os.makedirs(OUT, exist_ok=True)
for t_ in TEXS:
    if not os.path.exists(os.path.join(OUT, t_)):
        shutil.copyfile(os.path.join(IN, t_), os.path.join(OUT, t_))
PARTS = ["SM_DroneBase", "SM_DroneLaser"]


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.data.objects if o.type == "MESH"]


def tri_count(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


def flat_dot(me):
    """smallest dot product between a corner normal and its face normal (1.0 = flat shaded)"""
    cn = np.array([c.vector[:] for c in me.corner_normals])
    fn = np.zeros((len(me.loops), 3))
    for p in me.polygons:
        for li in p.loop_indices:
            fn[li] = p.normal
    return round(float((cn * fn).sum(1).min()), 6)


def make_material():
    mat = bpy.data.materials.new("meshy_drone")
    mat.use_nodes = True
    nt = mat.node_tree
    for n in list(nt.nodes):
        if n.type != "OUTPUT_MATERIAL":
            nt.nodes.remove(n)
    out_n = [n for n in nt.nodes if n.type == "OUTPUT_MATERIAL"][0]
    pb = nt.nodes.new("ShaderNodeBsdfPrincipled")
    nt.links.new(pb.outputs["BSDF"], out_n.inputs["Surface"])

    def tex(fn, cs):
        n = nt.nodes.new("ShaderNodeTexImage")
        img = bpy.data.images.load(os.path.join(OUT, fn), check_existing=False)
        img.colorspace_settings.name = cs
        n.image = img
        return n
    tb = tex(TEXS[0], "sRGB")
    nt.links.new(tb.outputs["Color"], pb.inputs["Base Color"])
    tn = tex(TEXS[1], "Non-Color")
    nm = nt.nodes.new("ShaderNodeNormalMap")
    nt.links.new(tn.outputs["Color"], nm.inputs["Color"])
    nt.links.new(nm.outputs["Normal"], pb.inputs["Normal"])
    tm = tex(TEXS[2], "Non-Color")
    sp = nt.nodes.new("ShaderNodeSeparateColor")
    nt.links.new(tm.outputs["Color"], sp.inputs["Color"])
    nt.links.new(sp.outputs["Green"], pb.inputs["Roughness"])
    nt.links.new(sp.outputs["Blue"], pb.inputs["Metallic"])
    return mat


# ---------------------------------------------------------------- load
obs = import_fbx(os.path.join(IN, "drone_game.fbx"))
assert len(obs) == 1
SRC = obs[0]
bm = bmesh.new()
bm.from_mesh(SRC.data)
for v in bm.verts:
    v.co = SRC.matrix_world @ v.co
bmesh.ops.triangulate(bm, faces=bm.faces[:])
for nm_ in [l.name for l in bm.loops.layers.uv if l.name == "LightMap"]:
    bm.loops.layers.uv.remove(bm.loops.layers.uv[nm_])
uv0 = bm.loops.layers.uv[0]
LAY_CUT = bm.edges.layers.int.new("cutid")
LAY_CAP = bm.faces.layers.int.new("iscap")
bm.verts.ensure_lookup_table()
bm.faces.ensure_lookup_table()
TRIS_IN = len(bm.faces)
SRC_OPEN = sum(1 for e in bm.edges if len(e.link_faces) == 1)
SRC_NONMAN = sum(1 for e in bm.edges if len(e.link_faces) > 2)
V_ALL = np.array([v.co[:] for v in bm.verts])
STAGE_MIN, STAGE_MAX = V_ALL.min(0), V_ALL.max(0)
SRC_BVH = BVHTree.FromBMesh(bm)
SRC_TRIS = [(tuple(l.vert.co.copy() for l in f.loops), tuple(l[uv0].uv.copy() for l in f.loops)) for f in bm.faces]
SRC_VERTS_COPY = V_ALL.copy()


def uv_area(b):
    s = 0.0
    u = b.loops.layers.uv[0]
    for f in b.faces:
        p = [l[u].uv for l in f.loops]
        s += 0.5 * abs(sum(p[i][0] * p[(i + 1) % len(p)][1] - p[(i + 1) % len(p)][0] * p[i][1] for i in range(len(p))))
    return s


UVA_BEFORE = uv_area(bm)
AREA_BEFORE = sum(f.calc_area() for f in bm.faces)

# pivots from the mesh
pods = V_ALL[np.abs(V_ALL[:, 1]) > 22.5]
POD_Z0, POD_Z1 = float(pods[:, 2].min()), float(pods[:, 2].max())
BASE_PIVOT = Vector(((STAGE_MIN[0] + STAGE_MAX[0]) / 2, (STAGE_MIN[1] + STAGE_MAX[1]) / 2, (POD_Z0 + POD_Z1) / 2))

# ---------------------------------------------------------------- 1. bisect at the plane inside the zone box
lo_, hi_ = ZONE
zone = [f for f in bm.faces if all(lo_[0] <= v.co.x <= hi_[0] and lo_[1] <= v.co.y <= hi_[1] and lo_[2] <= v.co.z <= hi_[2] for v in f.verts)]
geom = list(zone) + list({e for f in zone for e in f.edges}) + list({v for f in zone for v in f.verts})
r = bmesh.ops.bisect_plane(bm, geom=geom, dist=1e-4, plane_co=Vector((0, 0, PLANE_Z)), plane_no=Vector((0, 0, 1)), clear_inner=False, clear_outer=False)
cut_edges = [g for g in r["geom_cut"] if isinstance(g, bmesh.types.BMEdge)]
for e in cut_edges:
    e[LAY_CUT] = 1
sec_pts = np.array([v.co[:] for e in cut_edges for v in e.verts])
SEC_MIN, SEC_MAX = sec_pts.min(0), sec_pts.max(0)
SEC_LEN = sum(e.calc_length() for e in cut_edges)
bm.verts.ensure_lookup_table()
bm.faces.ensure_lookup_table()
UVA_AFTER = uv_area(bm)
bmesh.ops.triangulate(bm, faces=bm.faces[:])

# ---------------------------------------------------------------- 2. split and caps
cut_e = [e for e in bm.edges if e[LAY_CUT] > 0]
bmesh.ops.split_edges(bm, edges=cut_e)
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.faces.ensure_lookup_table()
for f in bm.faces:
    f[LAY_CAP] = 0
BND_BEFORE = [e for e in bm.edges if e[LAY_CUT] > 0 and len(e.link_faces) == 1]
TRIS_AFTER_BISECT = len(bm.faces)
new_faces = []
bnd_ = [e for e in bm.edges if e[LAY_CUT] == 1 and len(e.link_faces) == 1]
new_faces += list(bmesh.ops.holes_fill(bm, edges=bnd_, sides=2000)["faces"])
left = [e for e in bm.edges if e[LAY_CUT] == 1 and len(e.link_faces) == 1]
if left:
    raise SystemExit("cap loops not closed: %d edges" % len(left))
for f in new_faces:
    f[LAY_CAP] = 1
bmesh.ops.triangulate(bm, faces=new_faces)
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.faces.ensure_lookup_table()
bm.normal_update()
CAPS = [f for f in bm.faces if f[LAY_CAP] == 1]
OPEN_AFTER = [e for e in bm.edges if e[LAY_CUT] > 0 and len(e.link_faces) == 1]

# ---------------------------------------------------------------- 3. connectivity: laser = below the plane
bm.faces.index_update()
NF = len(bm.faces)
comp = -np.ones(NF, int)
nc = 0
for f0 in bm.faces:
    if comp[f0.index] >= 0:
        continue
    st = [f0]
    comp[f0.index] = nc
    while st:
        f = st.pop()
        for e in f.edges:
            for g in e.link_faces:
                if comp[g.index] < 0:
                    comp[g.index] = nc
                    st.append(g)
    nc += 1
fa = np.array([f.calc_area() for f in bm.faces])
fc = np.array([f.calc_center_median()[:] for f in bm.faces])
carea = np.bincount(comp, weights=fa, minlength=nc)
ccent = np.array([(fc[comp == k] * fa[comp == k, None]).sum(0) / carea[k] for k in range(nc)])
cfaces = np.bincount(comp, minlength=nc)
order = np.argsort(-carea)
big = [int(k) for k in order[:2]]
laser_k = min(big, key=lambda k: ccent[k][2])
base_k = [k for k in big if k != laser_k][0]
partof = np.zeros(NF, int)      # 0 base, 1 laser
FRAG = []
for k in range(nc):
    if k == base_k:
        p = 0
    elif k == laser_k:
        p = 1
    else:
        p = 1 if ccent[k][2] < PLANE_Z else 0
        FRAG.append(dict(faces=int(cfaces[k]), area_cm2=round(float(carea[k]), 3), centroid_cm=[round(float(x), 1) for x in ccent[k]], assigned_to=PARTS[p]))
    partof[comp == k] = p
COMP_REP = dict(total_after_cut=int(nc), base=dict(faces=int(cfaces[base_k]), area_cm2=round(float(carea[base_k]), 2)), laser=dict(faces=int(cfaces[laser_k]), area_cm2=round(float(carea[laser_k]), 2)), fragments=FRAG)

# ---------------------------------------------------------------- 4. cap orientation and UV
flipped = 0
for f in CAPS:
    pi = partof[f.index]
    want = 1.0 if pi == 1 else -1.0      # laser cap faces up (+Z), base cap faces down
    if f.normal.z * want < 0:
        f.normal_flip()
        flipped += 1
    cen = np.array(f.calc_center_median()[:])
    cands = {g for v in f.verts for g in v.link_faces if g[LAY_CAP] == 0 and partof[g.index] == pi}
    if not cands:
        cands = {g for g in bm.faces if g[LAY_CAP] == 0 and partof[g.index] == pi}
    src = min(cands, key=lambda g: float(((np.array(g.calc_center_median()[:]) - cen) ** 2).sum()))
    u = sum((l[uv0].uv for l in src.loops), Vector((0, 0))) / len(src.loops)
    for l in f.loops:
        l[uv0].uv = u
bm.faces.index_update()
bm.normal_update()

# stage-frame facts of the parts
VB = np.array([v.co[:] for f in bm.faces if partof[f.index] == 0 for v in f.verts])
VL = np.array([v.co[:] for f in bm.faces if partof[f.index] == 1 for v in f.verts])
LASER_PIVOT = Vector(((SEC_MIN[0] + SEC_MAX[0]) / 2, 0.0 if abs((SEC_MIN[1] + SEC_MAX[1]) / 2) < 0.5 else (SEC_MIN[1] + SEC_MAX[1]) / 2, PLANE_Z))
PIV = [BASE_PIVOT, LASER_PIVOT]
tip = VL[VL[:, 2] < VL[:, 2].min() + 1.0]
TIP = np.array([tip[:, 0].mean(), tip[:, 1].mean(), VL[:, 2].min()])
# dish stem: highest part of the base, above the rear box (z > 56.5)
stem = VB[VB[:, 2] > 56.5]
slices = []
for z0 in np.arange(56.5, VB[:, 2].max() + 0.01, 0.5):
    m = (stem[:, 2] >= z0) & (stem[:, 2] < z0 + 0.5)
    if m.sum():
        w = stem[m]
        slices.append((round(float(z0), 2), round(float((w[:, 0].max() - w[:, 0].min())), 2), round(float(w[:, 0].mean()), 2), round(float(w[:, 1].mean()), 2)))
STEM_TOP_Z = float(VB[:, 2].max())
widest = max(slices, key=lambda s: s[1])
# head = the widest slice and the ones above it down to the first slice narrower than 60 percent; the nub is the narrow top
head_slices = [s for s in slices if s[0] >= widest[0] - 2.5 and s[1] >= 0.6 * widest[1]]
HEAD_TOP_Z = max(s[0] for s in head_slices) + 0.5
head_v = stem[(stem[:, 2] >= HEAD_TOP_Z - 3.5) & (stem[:, 2] <= HEAD_TOP_Z)]
STEM_AXIS = [float(head_v[:, 0].mean()), float(head_v[:, 1].mean())]
top_v = stem[stem[:, 2] > STEM_TOP_Z - 1.0]
STEM_TOP_XY = [float(top_v[:, 0].mean()), float(top_v[:, 1].mean())]

# ---------------------------------------------------------------- 5. parts
MAT = make_material()
bpy.context.scene.unit_settings.scale_length = 1.0
objs = {}
PART_REP = {}


def lightmap(b):
    uv1 = b.loops.layers.uv.new("LightMap")
    b.faces.ensure_lookup_table()
    b.normal_update()
    island = []
    for f in b.faces:
        nn = f.normal.normalized()
        tt = Vector((1, 0, 0)) if abs(nn.z) > 0.99 else Vector((-nn.y, nn.x, 0)).normalized()
        ss = nn.cross(tt).normalized()
        loc = [(l.vert.co.dot(tt), l.vert.co.dot(ss)) for l in f.loops]
        mx_, my_ = min(a for a, b_ in loc), min(b_ for a, b_ in loc)
        loc = [(a - mx_, b_ - my_) for a, b_ in loc]
        island.append((loc, max(a for a, b_ in loc), max(b_ for a, b_ in loc)))
    od = sorted(range(len(island)), key=lambda i: -island[i][2])
    PAD = 2.0

    def pack(sc):
        x = y = PAD
        rowh = 0.0
        outp = {}
        for i in od:
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
    lo2, hi2 = 1e-4, 50.0
    for _ in range(50):
        mid = (lo2 + hi2) / 2
        if pack(mid):
            lo2 = mid
        else:
            hi2 = mid
    placed = pack(lo2)
    for f in b.faces:
        x, y = placed[f.index]
        for l, (lx, ly) in zip(f.loops, island[f.index][0]):
            l[uv1].uv = ((x + lx * lo2) / LM, (y + ly * lo2) / LM)
    return lo2


CAPFACES = {0: 0, 1: 0}
for f in CAPS:
    CAPFACES[int(partof[f.index])] += 1
NONCAP_AREA = {0: 0.0, 1: 0.0}
for f in bm.faces:
    if f[LAY_CAP] == 0:
        NONCAP_AREA[int(partof[f.index])] += f.calc_area()
for pi, name in enumerate(PARTS):
    b = bm.copy()
    b.faces.ensure_lookup_table()
    kill = [f for f in b.faces if partof[f.index] != pi]
    bmesh.ops.delete(b, geom=kill, context="FACES")
    for v in b.verts:
        v.co = v.co - PIV[pi]
    b.edges.layers.int.remove(b.edges.layers.int["cutid"])
    b.faces.layers.int.remove(b.faces.layers.int["iscap"])
    b.faces.ensure_lookup_table()
    b.faces.index_update()
    sc_ = lightmap(b)
    me = bpy.data.meshes.new(name)
    b.to_mesh(me)
    b.free()
    me.materials.append(MAT)
    me.uv_layers.active_index = 0
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    bpy.ops.object.select_all(action="DESELECT")
    bpy.context.view_layer.objects.active = o
    o.select_set(True)
    try:
        bpy.ops.mesh.customdata_custom_splitnormals_clear()      # drop any custom (smooth) normals carried over from the Meshy FBX
    except RuntimeError:
        pass
    me.shade_flat()
    objs[name] = o
    PART_REP[name] = dict(pivot_stage_cm=[round(x, 3) for x in PIV[pi]], tris=tri_count(me), vertices=len(me.vertices), cap_faces=CAPFACES[pi], lightmap_px_per_cm=round(sc_, 4))
bpy.context.view_layer.update()

# ---------------------------------------------------------------- export
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)


def export(path, names, locs):
    bpy.ops.object.select_all(action="DESELECT")
    for n in names:
        objs[n].location = locs[n]
        objs[n].select_set(True)
    bpy.context.view_layer.objects.active = objs[names[0]]
    bpy.ops.export_scene.fbx(filepath=path, **EXPORT)


Z3 = Vector((0, 0, 0))
LASER_OFFSET = LASER_PIVOT - BASE_PIVOT
export(os.path.join(OUT, "SM_DroneBase.fbx"), ["SM_DroneBase"], {"SM_DroneBase": Z3})
export(os.path.join(OUT, "SM_DroneLaser.fbx"), ["SM_DroneLaser"], {"SM_DroneLaser": Z3})
export(os.path.join(OUT, "SM_Drone_assembled.fbx"), PARTS, {"SM_DroneBase": Z3, "SM_DroneLaser": LASER_OFFSET})
usf = []
for fn in ("SM_DroneBase.fbx", "SM_DroneLaser.fbx", "SM_Drone_assembled.fbx"):
    b_ = open(os.path.join(OUT, fn), "rb").read()
    j = b_.find(b"UnitScaleFactor")
    k = b_.find(b"D", j + 20)
    usf.append(struct.unpack("<d", b_[k + 1:k + 9])[0])

# ---------------------------------------------------------------- read back and checks
RB = {}
WORLD = {}
for fn in ("SM_DroneBase.fbx", "SM_DroneLaser.fbx", "SM_Drone_assembled.fbx"):
    ob2 = import_fbx(os.path.join(OUT, fn))
    d = {}
    for o in ob2:
        me = o.data
        W = np.array([(o.matrix_world @ v.co)[:] for v in me.vertices])
        ec = defaultdict(int)
        for p in me.polygons:
            for k in p.edge_keys:
                ec[k] += 1
        u0 = np.array([l.uv[:] for l in me.uv_layers[0].data])
        u1 = np.array([l.uv[:] for l in me.uv_layers["LightMap"].data]).reshape(-1, 3, 2) * LM
        bmin, bmax = u1.min(1), u1.max(1)
        ov = 0
        gap = 1e9
        for i in range(len(u1) - 1):
            gx = np.maximum(bmin[i + 1:, 0] - bmax[i, 0], bmin[i, 0] - bmax[i + 1:, 0])
            gy = np.maximum(bmin[i + 1:, 1] - bmax[i, 1], bmin[i, 1] - bmax[i + 1:, 1])
            g = np.maximum(gx, gy)
            ov += int((g < 0).sum())
            gap = min(gap, float(g.min()))
        d[o.name] = dict(tris=tri_count(me), vertices=len(me.vertices), world_origin_cm=[round(x, 3) for x in o.matrix_world.translation],
                         bounds_min_cm=[round(float(x), 3) for x in W.min(0)], bounds_max_cm=[round(float(x), 3) for x in W.max(0)],
                         size_cm=[round(float(x), 3) for x in (W.max(0) - W.min(0))], uv_layers=[u.name for u in me.uv_layers],
                         materials=[m.name for m in me.materials], open_edges=sum(1 for v in ec.values() if v == 1), nonmanifold_edges=sum(1 for v in ec.values() if v > 2),
                         uv0_range=dict(u=[round(float(u0[:, 0].min()), 5), round(float(u0[:, 0].max()), 5)], v=[round(float(u0[:, 1].min()), 5), round(float(u0[:, 1].max()), 5)]),
                         lightmap=dict(overlapping_pairs_bbox=ov, min_gap_px=round(gap, 3), in_range=bool(u1.min() >= 0 and u1.max() <= LM), passed=bool(ov == 0 and gap >= 1.0 and u1.min() >= 0 and u1.max() <= LM)),
                         flat_min_dot=flat_dot(me))
        WORLD[(fn, o.name)] = (o, W)
        if fn == "SM_Drone_assembled.fbx":
            WORLD[(fn, o.name)] = (None, W)
    RB[fn] = d

# reassembly: assembled read-back (base at the base pivot) back to the stage frame
Wb = WORLD[("SM_Drone_assembled.fbx", "SM_DroneBase")][1] + np.array(BASE_PIVOT[:])
Wl = WORLD[("SM_Drone_assembled.fbx", "SM_DroneLaser")][1] + np.array(BASE_PIVOT[:])
U = np.concatenate([Wb, Wl])
from mathutils import kdtree
kd = kdtree.KDTree(len(U))
for i, p in enumerate(U):
    kd.insert(Vector(p), i)
kd.balance()
miss = 0.0
for p in SRC_VERTS_COPY:
    miss = max(miss, kd.find(Vector(p))[2])
# union vertices must lie on the original surface (BVH on the uncut stage mesh)
far = 0.0
for p in U:
    hit = SRC_BVH.find_nearest(Vector(p))
    far = max(far, hit[3] if hit[3] is not None else 1e9)
# the independent exports of the parts must equal the parts inside the assembled file (shifted by the pivots)
sepB = WORLD[("SM_DroneBase.fbx", "SM_DroneBase")][1] + np.array(BASE_PIVOT[:])
sepL = WORLD[("SM_DroneLaser.fbx", "SM_DroneLaser")][1] + np.array(LASER_PIVOT[:])
sep_err = max(float(np.abs(sepB - Wb).max()), float(np.abs(sepL - Wl).max()))

# texture mapping: random points on the non-cap faces of the parts: UV of the part vs UV interpolated on the original surface at the nearest point
rng = np.random.default_rng(252)
uv_err = []
sample_n = 0
NB = 0
for pi in (0, 1):
    fl = [f for f in bm.faces if partof[f.index] == pi and f[LAY_CAP] == 0]
    ar = np.array([f.calc_area() for f in fl])
    idx = rng.choice(len(fl), size=3000, p=ar / ar.sum())
    for i in idx:
        f = fl[i]
        a, b2 = rng.random(2)
        if a + b2 > 1:
            a, b2 = 1 - a, 1 - b2
        w = (1 - a - b2, a, b2)
        P = sum((l.vert.co * ww for l, ww in zip(f.loops, w)), Vector((0, 0, 0)))
        U_out = sum((l[uv0].uv * ww for l, ww in zip(f.loops, w)), Vector((0, 0)))
        hit = SRC_BVH.find_nearest(P)
        loc, nrm, fi, dist = hit
        if dist is None or dist > 1e-3:
            NB += 1
            continue
        (p0, p1, p2), (t0, t1, t2) = SRC_TRIS[fi]
        from mathutils.geometry import barycentric_transform
        U_src = Vector((0, 0))
        v0_, v1_, v2_ = p1 - p0, p2 - p0, loc - p0
        d00, d01, d11 = v0_.dot(v0_), v0_.dot(v1_), v1_.dot(v1_)
        d20, d21 = v2_.dot(v0_), v2_.dot(v1_)
        den = d00 * d11 - d01 * d01
        bv = (d11 * d20 - d01 * d21) / den
        bw = (d00 * d21 - d01 * d20) / den
        bu = 1 - bv - bw
        U_src = t0 * bu + t1 * bv + t2 * bw
        uv_err.append(max(abs(U_out[0] - U_src[0]), abs(U_out[1] - U_src[1])))
        sample_n += 1
uv_err = np.array(uv_err)

tot_tris = RB["SM_DroneBase.fbx"]["SM_DroneBase"]["tris"] + RB["SM_DroneLaser.fbx"]["SM_DroneLaser"]["tris"]
A = RB["SM_Drone_assembled.fbx"]
aw_min = np.minimum(np.array(A["SM_DroneBase"]["bounds_min_cm"]), np.array(A["SM_DroneLaser"]["bounds_min_cm"]))
aw_max = np.maximum(np.array(A["SM_DroneBase"]["bounds_max_cm"]), np.array(A["SM_DroneLaser"]["bounds_max_cm"]))
rel = lambda p: [round(float(x), 3) for x in (np.array(p) - np.array(BASE_PIVOT[:]))]
relL = lambda p: [round(float(x), 3) for x in (np.array(p) - np.array(LASER_PIVOT[:]))]
WARN = []
if tot_tris > 6000:
    WARN.append("total triangles above 6000")
if OPEN_AFTER:
    WARN.append("%d cap boundary edges still open" % len(OPEN_AFTER))
if nc != 2:
    WARN.append("%d connected pieces after the cut (expected 2): fragments assigned by height" % nc)
if RB["SM_DroneBase.fbx"]["SM_DroneBase"]["open_edges"] + RB["SM_DroneLaser.fbx"]["SM_DroneLaser"]["open_edges"] != SRC_OPEN:
    WARN.append("open edges of the parts differ from the Meshy mesh (%d)" % SRC_OPEN)
if miss > 1e-3 or far > 1e-3:
    WARN.append("reassembly outside tolerance")
if len(uv_err) and float(np.percentile(uv_err, 99.5)) > 1e-3:
    WARN.append("texture mapping differs on more than 0.5 percent of the samples")
for fn, d in RB.items():
    for n, v in d.items():
        if v["flat_min_dot"] < 0.999:
            WARN.append("%s in %s is not flat shaded (min dot %s)" % (n, fn, v["flat_min_dot"]))
        if not v["lightmap"]["passed"]:
            WARN.append("lightmap check failed on " + n)
rep = dict(
    task="task-252-01-drone-mesh-parts",
    inputs=dict(fbx="drone_game.fbx (stage of drone_game_ready.py)", triangles=TRIS_IN, source_open_edges=SRC_OPEN, source_nonmanifold_edges=SRC_NONMAN, stage_bounds_cm=[[round(float(x), 3) for x in STAGE_MIN], [round(float(x), 3) for x in STAGE_MAX]]),
    frame="cm, front +X, left +Y, Z up, FBX_SCALE_UNITS (UnitScaleFactor in files: %s)" % usf,
    cut=dict(plane="z = %.2f (stage frame)" % PLANE_Z, zone_box_cm=[list(ZONE[0]), list(ZONE[1])], zone_faces=len(zone), cut_edges=len(cut_edges), section_length_cm=round(SEC_LEN, 2),
             section_bounds_cm=[[round(float(x), 3) for x in SEC_MIN], [round(float(x), 3) for x in SEC_MAX]],
             why="natural joint = neck of the laser mount below the body: the section is stable from z 17 to 19.5 (neck + two yoke arms only); the hex mount and the arm top blocks stay on the body",
             tris_added_by_bisect=int(TRIS_AFTER_BISECT - TRIS_IN), cap_faces=len(CAPS), cap_boundary_edges_before_fill=len(BND_BEFORE), cap_boundary_edges_still_open=len(OPEN_AFTER), cap_faces_flipped=flipped,
             cap_uv_method="all three UV0 corners of a cap at the UV centroid of the nearest face of the same part touching it (flat colour)", connected_pieces=COMP_REP),
    parts={n: dict(PART_REP[n], readback=RB[("SM_DroneBase.fbx" if n == "SM_DroneBase" else "SM_DroneLaser.fbx")][n]) for n in PARTS},
    pivots=dict(
        SM_DroneBase="centre of the body: x, y = middle of the stage bounding box (shell), z = middle of the side pod height range (%.2f to %.2f in stage z); stage coordinates %s" % (POD_Z0, POD_Z1, [round(x, 3) for x in BASE_PIVOT]),
        SM_DroneLaser="top attachment point = centre of the neck section at the cut: stage coordinates %s; pitch = rotation about the Y axis through it" % [round(x, 3) for x in LASER_PIVOT],
        laser_pivot_in_base_frame_cm=[round(float(x), 3) for x in LASER_OFFSET], laser_pivot_in_assembled_fbx_cm=[round(float(x), 3) for x in LASER_OFFSET],
        origin_reference="assembled and base frame: z = 0 at the middle of the body; pods bottom z = %.3f, pods top z = %.3f, body shell bottom about z = %.1f, emitter tip z = %.3f" % (POD_Z0 - BASE_PIVOT.z, POD_Z1 - BASE_PIVOT.z, 21.0 - BASE_PIVOT.z, TIP[2] - BASE_PIVOT.z)),
    emitter_tip=dict(base_frame_cm=rel(TIP), laser_frame_cm=relL(TIP), note="centre of the lowest 1 cm of the cage; the beam leaves downward (-Z) from here; laser_frame = relative to the laser pivot"),
    stem_top=dict(spin_axis="Z", head_top_base_frame_cm=rel([STEM_AXIS[0], STEM_AXIS[1], HEAD_TOP_Z]), head_top_stage_cm=[round(STEM_AXIS[0], 3), round(STEM_AXIS[1], 3), round(HEAD_TOP_Z, 3)],
                  nub_tip_base_frame_cm=rel([STEM_TOP_XY[0], STEM_TOP_XY[1], STEM_TOP_Z]),
                  note="stem = ring at z 56 to 58 (stage), tapered post, knuckle head (axis x, y at the head), then a thin nub; the dish mounts on the head top (head_top); the nub is the Meshy antenna tip and the dish may replace it; the stem leans a little (x changes along z)",
                  slices_stage_z_width_x_meanx_meany=slices),
    totals=dict(triangles=tot_tris, limit=6000, base=RB["SM_DroneBase.fbx"]["SM_DroneBase"]["tris"], laser=RB["SM_DroneLaser.fbx"]["SM_DroneLaser"]["tris"], source=TRIS_IN,
                assembled_bounds_min_cm=[round(float(x), 3) for x in aw_min], assembled_bounds_max_cm=[round(float(x), 3) for x in aw_max], assembled_size_cm=[round(float(x), 3) for x in (aw_max - aw_min)]),
    cut_check=dict(
        holes=dict(cap_boundary_edges_open_after_fill=len(OPEN_AFTER), open_edges_base_readback=RB["SM_DroneBase.fbx"]["SM_DroneBase"]["open_edges"], open_edges_laser_readback=RB["SM_DroneLaser.fbx"]["SM_DroneLaser"]["open_edges"],
                    open_edges_in_meshy_mesh=SRC_OPEN, nonmanifold_edges_readback=[RB["SM_DroneBase.fbx"]["SM_DroneBase"]["nonmanifold_edges"], RB["SM_DroneLaser.fbx"]["SM_DroneLaser"]["nonmanifold_edges"]], nonmanifold_edges_in_meshy_mesh=SRC_NONMAN),
        reassembly=dict(max_distance_original_vertex_to_nearest_part_vertex_cm=round(miss, 6), max_distance_part_vertex_to_original_surface_cm=round(far, 6), separate_vs_assembled_fbx_max_difference_cm=round(sep_err, 6),
                        surface_area_cm2_before=round(AREA_BEFORE, 3), surface_area_cm2_parts_without_caps=round(NONCAP_AREA[0] + NONCAP_AREA[1], 3),
                        surface_area_relative_difference=round(abs(NONCAP_AREA[0] + NONCAP_AREA[1] - AREA_BEFORE) / AREA_BEFORE, 8), tolerance_cm=1e-3),
        texture_mapping=dict(uv0_area_before=round(UVA_BEFORE, 6), uv0_area_after_bisect=round(UVA_AFTER, 6), samples=sample_n, samples_off_surface_skipped=NB,
                             max_uv_error=round(float(uv_err.max()), 6) if len(uv_err) else None, p99_5_uv_error=round(float(np.percentile(uv_err, 99.5)), 6) if len(uv_err) else None,
                             fraction_below_1em3=round(float((uv_err < 1e-3).mean()), 5) if len(uv_err) else None,
                             method="3000 random points per part on non-cap faces: UV of the part vs UV interpolated at the nearest point of the uncut mesh")),
    textures=TEXS, warnings=WARN, seconds=round(time.time() - T0, 1))
with open(os.path.join(OUT, "drone_parts.report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("DONE", json.dumps(dict(tris=tot_tris, caps=len(CAPS), base=rep["totals"]["base"], laser=rep["totals"]["laser"], pieces=int(nc), miss=miss, far=far, uv99=rep["cut_check"]["texture_mapping"]["p99_5_uv_error"], warnings=WARN)))
