"""task-284-04: robots whose rigid parts are already separate connected components (kamikaze): cut the game FBX into the parts of a config, bake a leg sway loop, check.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/robot_component_parts_bake.py -- --in-fbx <game.fbx> --in-tex <basecolor.png> --out-dir <dir> --config Tools/art/blender/robot_configs/kamikaze.json [--work <dir>]
Companion of meshy_cut_parts.py / meshy_bake_anim.py (which hard-code the ten chaser parts): same part object convention (origin = pivot, plain parent offset, UV0 kept, UV1 LightMap one island per face, UV2 PartId = (index + 0.5) / 10, colour attribute PartIndex = index / 9, all material slots kept in order),
same texture layout (20 columns, texel 2 i = quaternion xyzw, texel 2 i + 1 = translation cm, new = R v + t, frame f = f-th scanline from the TOP of the EXR, RGBA half float, Non-Color) and same part transform formulas, so MF_RobotAnimWPO works unchanged.
No bisect: a part is a set of whole connected components (config: leg parts name the component nearest to `ref`; the first part takes the rest), so no cap and no new triangle.
Sway: leg i rotates about its pivot, Y axis, angle = amp x sin(2 pi f / frames) x (+1 L, -1 R); the body stays at rest (the hover bob is C++).
Writes <out>/<out_name>.fbx, .report.json, <tex_name>, <report_name> (with walk_poses for robot_walk_preview.py) and cut_check.report.json. Blender only; nothing is imported in Unreal."""
import bpy, bmesh, sys, os, json, math, time, shutil, struct, itertools
import numpy as np
from mathutils import Vector, Matrix, Euler

argv = sys.argv[sys.argv.index("--") + 1:]
opt = {"--in-fbx": "", "--in-tex": "", "--out-dir": "", "--config": "", "--work": "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work_k"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-fbx", "--in-tex", "--out-dir", "--config"):
    if not opt[k]:
        raise SystemExit("missing " + k)
CFG = json.load(open(opt["--config"], encoding="utf-8"))
SW = CFG["sway"]
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
os.makedirs(WORK, exist_ok=True)
os.makedirs(OUT, exist_ok=True)
PD = CFG["parts"]
NAMES = [p["name"] for p in PD]
IDX = {p["name"]: p["index"] for p in PD}
PARENT = {p["name"]: p["parent"] for p in PD}
PIVOT = {p["name"]: np.array(p["pivot"], float) for p in PD}
NF = int(SW["frames"])
NFT = int(SW["tex_rows"])
TEXW = 20
OUT_NAME = CFG["out_name"]
FBX = os.path.join(WORK, OUT_NAME + ".fbx")
LM = 2048
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
WARN = []


def tri_count(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path, colors_type="LINEAR")
    return [o for o in bpy.data.objects if o.type == "MESH"]


# ---------------------------------------------------------------- load and components
obs = import_fbx(opt["--in-fbx"])
assert len(obs) == 1
SRC = obs[0]
SRC_MATS = list(SRC.data.materials)
SRC_MAT_NAMES = [m.name for m in SRC_MATS]
bm = bmesh.new()
bm.from_mesh(SRC.data)
for v in bm.verts:
    v.co = SRC.matrix_world @ v.co
bmesh.ops.triangulate(bm, faces=bm.faces[:])
for nm_ in [l.name for l in bm.loops.layers.uv if l.name == "LightMap"]:
    bm.loops.layers.uv.remove(bm.loops.layers.uv[nm_])
bm.verts.ensure_lookup_table()
bm.faces.ensure_lookup_table()
bm.faces.index_update()
SRC_V = np.array([v.co[:] for v in bm.verts])
TRIS_IN = len(bm.faces)
SRC_OPEN = sum(1 for e in bm.edges if len(e.link_faces) == 1)
HEIGHT = float(SRC_V[:, 2].max() - SRC_V[:, 2].min())
comp = -np.ones(TRIS_IN, int)
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
partof = np.zeros(TRIS_IN, int)           # index into PD; the first part takes every component not claimed
CLAIM = {}
for pi, p in enumerate(PD):
    if "ref" not in p:
        continue
    ok = [k for k in range(nc) if carea[k] > 500]
    k = min(ok, key=lambda k: float(np.linalg.norm(ccent[k] - np.array(p["ref"]))))
    assert k not in CLAIM.values(), "component claimed twice"
    CLAIM[p["name"]] = k
    partof[comp == k] = pi
COMPS = sorted([dict(component=int(k), faces=int((comp == k).sum()), area_cm2=round(float(carea[k]), 1), centroid_cm=[round(float(x), 1) for x in ccent[k]],
                     bbox_min=[round(float(x), 1) for x in fc[comp == k].min(0)], bbox_max=[round(float(x), 1) for x in fc[comp == k].max(0)],
                     part=PD[int(partof[comp == k][0])]["name"]) for k in range(nc)], key=lambda r: -r["area_cm2"])
print("COMPONENTS", nc, [(c["faces"], c["part"]) for c in COMPS[:8]])


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
        out = {}
        for i in od:
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

    lo_, hi_ = 1e-4, 50.0
    for _ in range(50):
        mid = (lo_ + hi_) / 2
        if pack(mid):
            lo_ = mid
        else:
            hi_ = mid
    placed = pack(lo_)
    for f in b.faces:
        x, y = placed[f.index]
        for l, (lx, ly) in zip(f.loops, island[f.index][0]):
            l[uv1].uv = ((x + lx * lo_) / LM, (y + ly * lo_) / LM)
    return lo_


# ---------------------------------------------------------------- parts
objs = {}
PART_REP = {}
for pi, p in enumerate(PD):
    name = p["name"]
    b = bm.copy()
    b.faces.ensure_lookup_table()
    bmesh.ops.delete(b, geom=[f for f in b.faces if partof[f.index] != pi], context="FACES")
    for v in b.verts:
        v.co = v.co - Vector(PIVOT[name])
    b.faces.ensure_lookup_table()
    b.faces.index_update()
    sc_ = lightmap(b)
    me = bpy.data.meshes.new(name)
    b.to_mesh(me)
    b.free()
    me.shade_flat()
    pid = me.uv_layers.new(name="PartId")
    pid.data.foreach_set("uv", np.full(len(me.loops) * 2, (p["index"] + 0.5) / 10.0, np.float32))
    for m_ in SRC_MATS:
        me.materials.append(m_)
    ca = me.color_attributes.new("PartIndex", "FLOAT_COLOR", "CORNER")
    ca.data.foreach_set("color", np.tile(np.array([p["index"] / 9.0, 0.0, 0.0, 1.0], np.float32), (len(me.loops), 1)).ravel())
    me.uv_layers.active_index = 0
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    objs[name] = o
    PART_REP[name] = dict(index=p["index"], pivot_cm=[round(float(x), 3) for x in PIVOT[name]], tris=tri_count(me), vertices=len(me.vertices), lightmap_px_per_cm=round(sc_, 4),
                          uv_layers=[u.name for u in me.uv_layers], materials=[m.name for m in me.materials])
for p in PD:
    o = objs[p["name"]]
    if p["parent"]:
        o.parent = objs[p["parent"]]
        o.matrix_parent_inverse = Matrix.Identity(4)
        o.location = Vector(PIVOT[p["name"]]) - Vector(PIVOT[p["parent"]])
    else:
        o.location = Vector(PIVOT[p["name"]])
bpy.context.view_layer.update()
bpy.ops.object.select_all(action="DESELECT")
for o in objs.values():
    o.select_set(True)
bpy.context.view_layer.objects.active = objs[NAMES[0]]
bpy.ops.export_scene.fbx(filepath=FBX, **EXPORT)

# ---------------------------------------------------------------- read back the exported FBX
obs2 = import_fbx(FBX)
OB = {o.name: o for o in obs2}
assert sorted(OB) == sorted(NAMES), sorted(OB)
bpy.context.view_layer.update()
REST_LOC, REST_P, REST_V = {}, {}, {}
for n in NAMES:
    o = OB[n]
    o.rotation_mode = "XYZ"
    m = o.matrix_world
    assert max(abs(m[i][j] - (1.0 if i == j else 0.0)) for i in range(3) for j in range(3)) < 1e-5, "rest matrix not a pure translation: " + n
    REST_LOC[n] = o.location.copy()
    REST_P[n] = np.array(m.translation[:], float)
    REST_V[n] = np.array([v.co[:] for v in o.data.vertices], float) + REST_P[n]


def apply_pose(pose):
    for n in NAMES:
        p = pose.get(n, {})
        OB[n].rotation_euler = Euler([math.radians(a) for a in p.get("rot", (0, 0, 0))], "XYZ")
        OB[n].location = REST_LOC[n] + Vector(p.get("loc", (0, 0, 0)))
    bpy.context.view_layer.update()


def world_verts(n):
    m = np.array(OB[n].matrix_world)
    return np.array([v.co[:] for v in OB[n].data.vertices], float) @ m[:3, :3].T + m[:3, 3]


def pose_at(f):
    s = math.sin(2 * math.pi * f / NF)
    A = float(SW["amp_deg"])
    pose = {}
    for p in PD:
        if p["name"].endswith("_L"):
            pose[p["name"]] = dict(rot=(0, A * s, 0))
        elif p["name"].endswith("_R"):
            pose[p["name"]] = dict(rot=(0, -A * s, 0))
    return pose


POSES, WORLD, VERTS = [], [], []
for f in range(NF + 1):          # frame NF = frame 0 again, for the seam check
    pose = pose_at(f)
    apply_pose(pose)
    POSES.append(pose)
    WORLD.append({n: (np.array(OB[n].matrix_world.to_3x3())[:], np.array(OB[n].matrix_world.translation[:], float)) for n in NAMES})
    VERTS.append({n: world_verts(n) for n in NAMES})
loop_err = max(float(np.linalg.norm(VERTS[NF][n] - VERTS[0][n], axis=1).max()) for n in NAMES)
rest_err = max(float(np.linalg.norm(VERTS[0][n] - REST_V[n], axis=1).max()) for n in NAMES)


def quat_from(R):
    t = np.trace(R)
    if t > 0:
        s = math.sqrt(t + 1) * 2
        q = [(R[2, 1] - R[1, 2]) / s, (R[0, 2] - R[2, 0]) / s, (R[1, 0] - R[0, 1]) / s, 0.25 * s]
    else:
        i = int(np.argmax(np.diag(R)))
        j, k = (i + 1) % 3, (i + 2) % 3
        s = math.sqrt(R[i, i] - R[j, j] - R[k, k] + 1) * 2
        q = [0.0] * 4
        q[i] = 0.25 * s
        q[j] = (R[j, i] + R[i, j]) / s
        q[k] = (R[k, i] + R[i, k]) / s
        q[3] = (R[k, j] - R[j, k]) / s
    q = np.array(q)
    return q / np.linalg.norm(q)


def qmat(q, norm=True):
    x, y, z, w = q / np.linalg.norm(q) if norm else q
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def half_nb(x):
    h = np.float16(x)
    return [float(np.nextafter(h, np.float16(-np.inf))), float(h), float(np.nextafter(h, np.float16(np.inf)))]


Q = np.zeros((NF, len(NAMES), 4))
T = np.zeros((NF, len(NAMES), 3))
for f in range(NF):
    for pi, n in enumerate(NAMES):
        R, q = WORLD[f][n]
        Q[f, pi] = quat_from(R)
        T[f, pi] = q - R @ REST_P[n]
for pi in range(len(NAMES)):
    if Q[0, pi, 3] < 0:
        Q[0, pi] *= -1
    for f in range(1, NF):
        if np.dot(Q[f, pi], Q[f - 1, pi]) < 0:
            Q[f, pi] *= -1
loop_dot_min = min(float(np.dot(Q[NF - 1, pi], Q[0, pi])) for pi in range(len(NAMES)))
Qh, Th = np.zeros_like(Q), np.zeros_like(T)
for f in range(NF):
    for pi, n in enumerate(NAMES):
        tgt = VERTS[f][n]
        best = None
        for c in itertools.product(*[half_nb(Q[f, pi, k]) for k in range(4)]):
            qc = np.array(c, float)
            br, bn = REST_V[n] @ qmat(qc, False).T, REST_V[n] @ qmat(qc).T
            for tl in ((tgt - br).mean(0), (tgt - bn).mean(0), (tgt - 0.5 * (br + bn)).mean(0)):
                tq = np.array([float(np.float16(x)) for x in tl])
                err = max(float(np.linalg.norm(br + tq - tgt, axis=1).max()), float(np.linalg.norm(bn + tq - tgt, axis=1).max()))
                if best is None or err < best[0]:
                    best = (err, qc, tq)
        Qh[f, pi], Th[f, pi] = best[1], best[2]
tex = np.zeros((NFT, TEXW, 4), np.float32)
tex[:, 0::2, 3] = 1.0              # every column and row defaults to the identity transform (quaternion 0 0 0 1, translation 0): unused part indices, rows after the sway
tex[:, 1::2, :] = 0.0
for pi, n in enumerate(NAMES):
    c = IDX[n]
    tex[:NF, 2 * c, :] = Qh[:, pi, :]
    tex[:NF, 2 * c + 1, :3] = Th[:, pi, :]
    tex[:NF, 2 * c + 1, 3] = 0.0

# ---------------------------------------------------------------- EXR write and read back
TEX_NAME = SW["tex_name"]
EXR_W = os.path.join(WORK, TEX_NAME)
sc = bpy.context.scene
ims = sc.render.image_settings
ims.file_format, ims.color_depth, ims.color_mode, ims.exr_codec = "OPEN_EXR", "16", "RGBA", "NONE"
img = bpy.data.images.new(os.path.splitext(TEX_NAME)[0], TEXW, NFT, alpha=True, float_buffer=True)
img.colorspace_settings.name = "Non-Color"
img.pixels.foreach_set(np.flipud(tex).astype(np.float32).ravel())
img.pixels.update()
img.save_render(EXR_W, scene=sc)
img2 = bpy.data.images.load(EXR_W, check_existing=False)
img2.colorspace_settings.name = "Non-Color"
assert tuple(img2.size) == (TEXW, NFT), tuple(img2.size)
back = np.flipud(np.array(img2.pixels[:], np.float32).reshape(NFT, TEXW, 4))      # back[frame, x], frame 0 = top scanline
exr_vs_half = float(np.abs(back - tex.astype(np.float16).astype(np.float32)).max())
# vertex error of the baked texture (as read from the saved EXR) against the keyed hierarchy, quaternion normalised or not
e_norm, e_raw = [], []
for f in range(NF):
    en = er = 0.0
    for n in NAMES:
        c = IDX[n]
        q, t = back[f, 2 * c, :4].astype(float), back[f, 2 * c + 1, :3].astype(float)
        en = max(en, float(np.linalg.norm(REST_V[n] @ qmat(q).T + t - VERTS[f][n], axis=1).max()))
        er = max(er, float(np.linalg.norm(REST_V[n] @ qmat(q, False).T + t - VERTS[f][n], axis=1).max()))
    e_norm.append(en)
    e_raw.append(er)
frame_step = [max(float(np.linalg.norm(VERTS[f + 1][n] - VERTS[f][n], axis=1).max()) for n in NAMES) for f in range(NF)]      # f = NF-1 is the wrap step (frame NF-1 to frame 0)
ANG = {n: max(abs(POSES[f].get(n, {}).get("rot", (0, 0, 0))[1]) for f in range(NF)) for n in NAMES}
MAXANG = max(ANG.values())
bake_ok = max(e_norm) < 0.05 and max(e_raw) < 0.05 and loop_err < 1e-4 and exr_vs_half < 1e-6
if not bake_ok:
    WARN.append("bake verification above 0.05 cm or seam above 1e-4 cm")
if MAXANG > 8.0 + 1e-6:
    WARN.append("max angle above 8 degrees")

# ---------------------------------------------------------------- cut check (subset of meshy_cut_check.py: that script asserts the ten chaser part names)
anom = []
tot_tris = sum(PART_REP[n]["tris"] for n in NAMES)
if tot_tris != TRIS_IN:
    anom.append("triangle count %d != source %d" % (tot_tris, TRIS_IN))
if tot_tris > CFG["tris_limit"]:
    anom.append("tris above limit")
allw = np.concatenate([REST_V[n] for n in NAMES])
ordr = lambda a: a[np.lexsort(np.round(a, 3).T[::-1])]
rest_vs_src = float(np.abs(ordr(np.unique(np.round(allw, 3), axis=0)) - ordr(np.unique(np.round(SRC_V, 3), axis=0))).max()) if len(np.unique(np.round(allw, 3), axis=0)) == len(np.unique(np.round(SRC_V, 3), axis=0)) else 1e9
if rest_vs_src > 2.5e-3:      # positions compared after rounding to 0.001 cm: one rounding step is float32 noise of the FBX
    anom.append("rest positions differ from the source (%g)" % rest_vs_src)
open_parts = 0
for n in NAMES:
    me = OB[n].data
    ec = {}
    for pl in me.polygons:
        for k in pl.edge_keys:
            ec[k] = ec.get(k, 0) + 1
    PART_REP[n]["open_edges"] = sum(1 for v in ec.values() if v == 1)
    PART_REP[n]["nonmanifold_edges"] = sum(1 for v in ec.values() if v > 2)
    open_parts += PART_REP[n]["open_edges"]
if open_parts != SRC_OPEN:
    anom.append("open edges %d != source %d" % (open_parts, SRC_OPEN))
shared = 0
keys = [set(map(tuple, np.round(REST_V[n], 3))) for n in NAMES]
for a, b in itertools.combinations(range(len(NAMES)), 2):
    shared += len(keys[a] & keys[b])
if shared:
    anom.append("%d vertex positions shared between parts" % shared)
PARTID = {}
for n in NAMES:
    me = OB[n].data
    a_ = np.array([l.uv[:] for l in me.uv_layers["PartId"].data], np.float64)
    e_ = float(np.float32((IDX[n] + 0.5) / 10.0))
    cav = np.array([c.color[:] for c in me.color_attributes["PartIndex"].data])
    PARTID[n] = dict(index=IDX[n], expected_float32=e_, unique_u=np.unique(a_[:, 0]).tolist(), unique_v=np.unique(a_[:, 1]).tolist(), max_abs_error=float(np.abs(a_ - e_).max()),
                     partindex_red_max_error=float(np.abs(cav[:, 0] - IDX[n] / 9.0).max()))
    if PARTID[n]["max_abs_error"] > 1e-7 or len(PARTID[n]["unique_u"]) != 1 or PARTID[n]["partindex_red_max_error"] > 1e-6:
        anom.append("PartId not exact on " + n)
    if [u.name for u in me.uv_layers] != ["UVMap", "LightMap", "PartId"] and [u.name for u in me.uv_layers][1:] != ["LightMap", "PartId"]:
        anom.append("UV layer order on " + n + ": " + str([u.name for u in me.uv_layers]))
    if [m.name for m in me.materials] != SRC_MAT_NAMES:
        anom.append("material slots on " + n)
    u1 = np.array([l.uv[:] for l in me.uv_layers["LightMap"].data]).reshape(-1, 3, 2) * LM
    if u1.min() < 0 or u1.max() > LM:
        anom.append("LightMap out of range on " + n)
if any(PART_REP[n]["tris"] == 0 for n in NAMES):
    anom.append("empty part")
CHECK = dict(task=CFG["task"], tool="robot_component_parts_bake.py (subset of meshy_cut_check.py: that script asserts the ten chaser part names)", anomalies=len(anom), anomaly_list=anom,
             checks=dict(tris_total=tot_tris, tris_source=TRIS_IN, rest_vertices_vs_source_max_cm=rest_vs_src, open_edges_parts=open_parts, open_edges_source=SRC_OPEN, shared_vertex_positions=shared,
                         partid=PARTID, caps_added=0, bisect_planes=0, bake_vs_keyed_poses_saved_exr_max_cm=dict(normalised=max(e_norm), raw=max(e_raw))))
json.dump(CHECK, open(os.path.join(WORK, "cut_check.report.json"), "w"), indent=1)

# ---------------------------------------------------------------- reports
PREP = dict(task=CFG["task"], input=os.path.basename(opt["--in-fbx"]), source_tris=TRIS_IN, height_cm=round(HEIGHT, 3), components=COMPS, claimed=CLAIM, hierarchy=PARENT,
            parts=PART_REP, pivots_cm={n: [round(float(x), 3) for x in PIVOT[n]] for n in NAMES}, export=dict({k: (sorted(v) if isinstance(v, set) else v) for k, v in EXPORT.items()}, blender=bpy.app.version_string),
            bounds_min_cm=[round(float(x), 3) for x in allw.min(0)], bounds_max_cm=[round(float(x), 3) for x in allw.max(0)], triangles=tot_tris, warnings=WARN)
json.dump(PREP, open(os.path.join(WORK, OUT_NAME + ".report.json"), "w"), indent=1)
run_rate = float(SW["run_rate"])
AREP = dict(
    task=CFG["task"], robot=CFG["robot"],
    parts=[dict(index=IDX[n], name=n, parent=PARENT[n], rest_pivot_cm=[round(float(x), 3) for x in REST_P[n]]) for n in NAMES],
    clips=[dict(id=0, name="sway", frames=NF, rows="0..%d" % (NF - 1), loop=True, fps=30, run_rate_baked_for=run_rate, cycle_s=round(NF / (30 * run_rate), 3),
                note="shader plays frame = fmod(t x RunRate x 30, %d) (MF_RobotAnimWPO, clip 0); the kamikaze instance RunRate must be %s; rows %d..%d are the identity (rest) pose" % (NF, run_rate, NF, NFT - 1))],
    joints_cm={n: [round(float(x), 2) for x in PIVOT[n]] for n in NAMES if n != NAMES[0]},
    sway=dict(amp_deg=float(SW["amp_deg"]), axis="Y (positive = backward swing), L = +sin, R = -sin (antiphase)", max_angle_deg_per_part={n: round(v, 4) for n, v in ANG.items()}, max_angle_deg=round(MAXANG, 4), limit_deg=8,
              limit_note="provisional, owner 2026-10-22: at most 8 degrees", knee_height_cm=None, foot_slide_cm_per_cycle=None, foot_note="hover: no ground contact, no knee, no foot slide"),
    seam=dict(loop_max_vertex_error_cm=loop_err, frame_NF_equals_frame_0=True, quaternion_dot_last_vs_first=loop_dot_min, max_vertex_step_cm_per_frame=round(max(frame_step), 4), wrap_step_cm=round(frame_step[NF - 1], 4),
              steps_cm=[round(s, 4) for s in frame_step]),
    texture=dict(file=TEX_NAME, width=TEXW, height=NFT, format="OpenEXR RGBA half float, compression NONE, Non-Color", layout="texel 2i = quaternion xyzw, texel 2i+1 = translation cm (i = PartId index), frame f = scanline f from the top, v = (f + 0.5) / %d" % NFT,
                 used_columns={n: [2 * IDX[n], 2 * IDX[n] + 1] for n in NAMES}, reload_vs_half_rounded_max_abs=exr_vs_half),
    bake_verification=dict(saved_exr_normalised_max_cm=max(e_norm), saved_exr_raw_quaternion_max_cm=max(e_raw), limit_cm=0.05, rest_pose_vs_part_origin_max_cm=rest_err),
    frames=NF, rows=NFT, walk_poses=[POSES[f] for f in range(NF)], warnings=WARN)
json.dump(AREP, open(os.path.join(WORK, SW["report_name"]), "w"), indent=1)
for src, dst in ((FBX, OUT_NAME + ".fbx"), (os.path.join(WORK, OUT_NAME + ".report.json"), OUT_NAME + ".report.json"), (EXR_W, TEX_NAME),
                 (os.path.join(WORK, SW["report_name"]), SW["report_name"]), (os.path.join(WORK, "cut_check.report.json"), "cut_check.report.json")):
    shutil.copyfile(src, os.path.join(OUT, dst))
print("DONE", json.dumps(dict(tris=tot_tris, anomalies=anom, max_angle=MAXANG, loop_err=loop_err, exr_err=[max(e_norm), max(e_raw)], wrap_step=frame_step[NF - 1], max_step=max(frame_step), warnings=WARN)))
