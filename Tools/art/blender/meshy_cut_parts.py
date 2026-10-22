"""task-157-01: cut the game-ready Meshy chaser (one fused mesh) into the 10 rigid parts of chaser-model-pipeline. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_cut_parts.py -- --in-fbx Art/Meshes/chaser-meshy/chaser_meshy_game.fbx --in-tex Art/Meshes/chaser-meshy/chaser_meshy_fixed_basecolor.png --out-dir Art/Meshes/chaser-meshy [--work <short dir>] [--no-render]
Method (every number below was found on THIS mesh: orthographic front/side close-ups, section profiles, and a minimum-cut-length scan of each joint plane around the visual guess):
  1. nine bisect planes (waist, shoulder, elbow, hip, knee; L and R mirrored through y = 0), each restricted to a zone box (hips also below the waist plane) so it only cuts the limb it is meant for;
  2. the cut edges are split, the closed section loops are filled with cap faces (holes_fill + triangulate), then the mesh is separated by face connectivity;
  3. the ten biggest connected pieces are assigned to the ten parts by nearest reference centroid; smaller fragments go to the nearest part;
  4. caps get the UV0 of the nearest existing face of the same part that touches the cap (all three corners at that face's UV centroid, so the cap samples that face's texels);
  5. each part object: origin at its joint, parent set with a plain local offset (no rotation, no scale), UV0 kept, UV1 'LightMap' (one island per face, shelf packed, no overlap), colour attribute 'PartIndex' (FLOAT, red = index / 9, alpha 1);
  6. FBX export (cm, FBX_SCALE_UNITS, axis -Z/Y, no smoothing change), read back, silhouette check against the uncut mesh in three views, tinted lit renders.
Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, bmesh, sys, os, json, math, time, shutil, struct
import numpy as np
from mathutils import Vector, Matrix
from collections import defaultdict

T0 = time.time()
TIM = {}
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
NORENDER = "--no-render" in argv
argv = [a for a in argv if a != "--no-render"]
opt = {"--in-fbx": "", "--in-tex": "", "--out-dir": "", "--height-cm": "123", "--work": "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work", "--mode": "auto", "--config": ""}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-fbx", "--in-tex", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
CFG = json.load(open(opt["--config"], encoding="utf-8")) if opt["--config"] else {}      # per-robot config (see Tools/art/blender/robot_configs/); without it every constant below keeps the chaser v1 value
HEIGHT = float(CFG.get("height_cm", opt["--height-cm"]))
MODE = opt["--mode"]      # auto = per cut, edges when its loops close else plane; plane = bisect along the plane (planar caps, +1 triangle per cut edge); edges = cut along existing mesh edges closest to the plane (no new triangles, zigzag caps)
WO = os.path.join(WORK, "parts_out")
os.makedirs(WO, exist_ok=True)
for f in os.listdir(WO):
    p_ = os.path.join(WO, f)
    shutil.rmtree(p_, ignore_errors=True) if os.path.isdir(p_) else os.remove(p_)
os.makedirs(os.path.join(WO, "renders", "parts"), exist_ok=True)
IN_FBX = os.path.join(WORK, "in_" + os.path.basename(opt["--in-fbx"]))
shutil.copyfile(opt["--in-fbx"], IN_FBX)
TEXNAME = os.path.basename(opt["--in-tex"])
WTEX = os.path.join(WO, TEXNAME)
shutil.copyfile(opt["--in-tex"], WTEX)
OUT_NAME = CFG.get("out_name", "chaser_meshy_parts")
FBX = os.path.join(WO, OUT_NAME + ".fbx")
LM = 2048


def tick(n, t):
    TIM[n] = round(time.time() - t, 2)


# ---------------------------------------------------------------- contract
PARTS = ["Pelvis", "HeadTorso", "UpperArm_L", "UpperArm_R", "ForearmHand_L", "ForearmHand_R", "Thigh_L", "Thigh_R", "ShinFoot_L", "ShinFoot_R"]
PARENT = {"Pelvis": None, "HeadTorso": "Pelvis", "UpperArm_L": "HeadTorso", "UpperArm_R": "HeadTorso", "ForearmHand_L": "UpperArm_L", "ForearmHand_R": "UpperArm_R",
          "Thigh_L": "Pelvis", "Thigh_R": "Pelvis", "ShinFoot_L": "Thigh_L", "ShinFoot_R": "Thigh_R"}
# joints found on the mesh (cm; +X front, +Y robot left, Z up); R = mirror through y = 0
J = dict(waist=(-4.0, 0.0, 88.5), hip=(-2.2, 10.0, 78.0), knee=(-6.1, 11.4, 50.9), shoulder=(-7.5, 17.0, 106.5), elbow=(-10.8, 27.4, 93.4))
J.update({k: tuple(v) for k, v in CFG.get("joints", {}).items()})
PIVOT = {"Pelvis": (J["hip"][0], 0.0, J["hip"][2]), "HeadTorso": J["waist"]}
for nm_, jn in (("UpperArm", "shoulder"), ("ForearmHand", "elbow"), ("Thigh", "hip"), ("ShinFoot", "knee")):
    PIVOT[nm_ + "_L"] = J[jn]
    PIVOT[nm_ + "_R"] = (J[jn][0], -J[jn][1], J[jn][2])
# reference centroids used only to name the connected pieces
REF = {"HeadTorso": (-1, 0, 104), "Pelvis": (-3, 0, 80), "UpperArm": (-8, 19, 105), "ForearmHand": (-9, 30, 78), "Thigh": (-3, 10, 66), "ShinFoot": (-5, 13, 28)}
REF.update({k: tuple(v) for k, v in CFG.get("refs", {}).items()})
REFP = {"HeadTorso": REF["HeadTorso"], "Pelvis": REF["Pelvis"]}
for nm_ in ("UpperArm", "ForearmHand", "Thigh", "ShinFoot"):
    REFP[nm_ + "_L"] = REF[nm_]
    REFP[nm_ + "_R"] = (REF[nm_][0], -REF[nm_][1], REF[nm_][2])


def unit(v):
    v = np.array(v, float)
    return v / np.linalg.norm(v)


# planes (left side values; mirrored for the right): point, normal, zone box, name. Offsets/tilts come from the minimum-cut scan around the visual guess.
PL = [
    dict(name="waist", P=(-4.0, 0.0, 88.0), n=unit((0.087, 0.0, 0.996)), box=((-30, -12, 70), (30, 12, 110)), side=0, pivot="HeadTorso"),
    dict(name="shoulder", P=(-8.0, 16.2, 106.0), n=unit((-0.174, 0.985, 0.0)), box=((-30, -14, 80), (30, 32, 130)), side=1, pivot="UpperArm"),
    dict(name="elbow", P=(-10.6, 26.9, 95.56), n=unit((0.062, 0.304, -0.951)), box=((-30, 10, 70), (30, 45, 112)), side=1, pivot="ForearmHand"),
    dict(name="hip", P=(-3.0, 8.6, 78.0), n=unit((0.0, 0.866, -0.5)), box=((-30, -14, 55), (30, 22, 92)), side=1, pivot="Thigh", below_waist=True),
    dict(name="knee", P=(-5.5, 12.0, 56.5), n=unit((0.0, 0.0, 1.0)), box=((-30, -2, 30), (30, 26, 70)), side=1, pivot="ShinFoot"),
]
if "planes" in CFG:
    PL = [dict(p, P=tuple(p["P"]), n=unit(p["n"]), box=(tuple(p["box"][0]), tuple(p["box"][1]))) for p in CFG["planes"]]
CUTS = []
for pl in PL:
    if pl["side"] == 0:
        CUTS.append(dict(pl, id=len(CUTS) + 1, label="waist"))
        continue
    lo, hi = pl["box"]
    CUTS.append(dict(pl, id=len(CUTS) + 1, label=pl["name"] + "_L"))
    mP = (pl["P"][0], -pl["P"][1], pl["P"][2])
    mn = np.array([pl["n"][0], -pl["n"][1], pl["n"][2]])
    CUTS.append(dict(pl, id=len(CUTS) + 1, label=pl["name"] + "_R", P=mP, n=mn, box=((lo[0], -hi[1], lo[2]), (hi[0], -lo[1], hi[2]))))
WAIST = CUTS[0]


# ---------------------------------------------------------------- helpers
def import_fbx(path, tex=None):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path, colors_type="LINEAR")
    obs = [o for o in bpy.data.objects if o.type == "MESH"]
    if tex:
        img = bpy.data.images.load(tex, check_existing=False)
        img.colorspace_settings.name = "sRGB"
        for o in obs:
            for m in o.data.materials:
                if m and m.node_tree:
                    for n in m.node_tree.nodes:
                        if n.type == "TEX_IMAGE":
                            n.image = img
    return obs


def tri_count(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


# ---------------------------------------------------------------- load
t = time.time()
obs = import_fbx(IN_FBX)
assert len(obs) == 1
SRC = obs[0]
SRC_STATS = {}
vv = np.array([(SRC.matrix_world @ v.co)[:] for v in SRC.data.vertices])
h_in = float(vv[:, 2].max() - vv[:, 2].min())
SCALE_FIX = 1.0
if abs(h_in - HEIGHT) > 0.5:
    SCALE_FIX = HEIGHT / h_in      # importer applied a unit factor: bring the mesh back to 1 unit = 1 cm
SRC_STATS = dict(height_as_imported=round(h_in, 4), scale_fix_applied=round(SCALE_FIX, 6), object_matrix_identity=bool(SRC.matrix_world == Matrix.Identity(4)),
                 tris=tri_count(SRC.data), vertices=len(SRC.data.vertices), uv_layers=[u.name for u in SRC.data.uv_layers])
SRC_MATS = list(SRC.data.materials)      # chaser v1: one material; heavy: [body, robot_eye] kept in order on every part
SRC_MAT = SRC_MATS[0]
if not CFG:
    SRC_MAT.name = "meshy_chaser"
TEX_IMG = bpy.data.images.load(WTEX, check_existing=False)
TEX_IMG.colorspace_settings.name = "sRGB"
for n in SRC_MAT.node_tree.nodes:
    if n.type == "TEX_IMAGE":
        n.image = TEX_IMG
UV0_NAME = SRC.data.uv_layers[0].name
bm = bmesh.new()
bm.from_mesh(SRC.data)
for v in bm.verts:
    v.co = (SRC.matrix_world @ v.co) * SCALE_FIX
bmesh.ops.triangulate(bm, faces=bm.faces[:])
bm.verts.ensure_lookup_table()
bm.faces.ensure_lookup_table()
for nm_ in [l.name for l in bm.loops.layers.uv if l.name == "LightMap"]:
    bm.loops.layers.uv.remove(bm.loops.layers.uv[nm_])      # the game FBX already carries a per-face LightMap: rebuilt per part below
uv0 = bm.loops.layers.uv[0]
LAY_CUT = bm.edges.layers.int.new("cutid")
LAY_CAP = bm.faces.layers.int.new("iscap")
TRIS_IN = len(bm.faces)
VERTS_IN = len(bm.verts)
SRC_OPEN_EDGES = sum(1 for e in bm.edges if len(e.link_faces) == 1)
SRC_NONMANIFOLD_EDGES = sum(1 for e in bm.edges if len(e.link_faces) > 2)
tick("load", t)


def uv_area(b):
    s = 0.0
    u = b.loops.layers.uv[0]
    for f in b.faces:
        p = [l[u].uv for l in f.loops]
        s += 0.5 * abs(sum(p[i][0] * p[(i + 1) % len(p)][1] - p[(i + 1) % len(p)][0] * p[i][1] for i in range(len(p))))
    return s


UVA_BEFORE = uv_area(bm)
AREA_BEFORE = sum(f.calc_area() for f in bm.faces)

# ---------------------------------------------------------------- 1. bisect
t = time.time()
CUTREP = {}


def wdist(co):
    return (np.array(co[:]) - np.array(WAIST["P"])) @ WAIST["n"]


WAIST_TOL = [0.01]


def zone_faces(b, c):
    lo, hi = c["box"]
    out = []
    for f in b.faces:
        ok = True
        for v in f.verts:
            co = v.co
            if not (lo[0] <= co.x <= hi[0] and lo[1] <= co.y <= hi[1] and lo[2] <= co.z <= hi[2]):
                ok = False
                break
            if c.get("below_waist") and wdist(co) > WAIST_TOL[0]:
                ok = False
                break
        if ok:
            out.append(f)
    return out


def edge_cut(b, c, faces):
    """edges between two zone faces whose centroids lie on opposite sides of the plane (a cut along existing mesh edges: no new triangle)"""
    fs = set(faces)
    P_, n_ = np.array(c["P"]), np.array(c["n"])
    sg = {f: ((np.array(f.calc_center_median()[:]) - P_) @ n_) > 0 for f in faces}
    return [e for e in {e for f in faces for e in f.edges} if len(e.link_faces) == 2 and e.link_faces[0] in fs and e.link_faces[1] in fs and sg[e.link_faces[0]] != sg[e.link_faces[1]]]


def close_cut(b, lay, cid, normal):
    """fill every boundary loop of the cut `cid` (edges with one face after the split): holes_fill first, then triangle_fill per connected edge group projected along the plane normal.
    returns (new faces, number of boundary edges left open)"""
    new = []
    bnd_ = [e for e in b.edges if e[lay] == cid and len(e.link_faces) == 1]
    if bnd_:
        new += list(bmesh.ops.holes_fill(b, edges=bnd_, sides=2000)["faces"])
    left = [e for e in b.edges if e[lay] == cid and len(e.link_faces) == 1]
    if left:
        groups = []
        seen = set()
        vmap = defaultdict(list)
        for e in left:
            for v in e.verts:
                vmap[v].append(e)
        for e0 in left:
            if e0 in seen:
                continue
            grp = []
            st = [e0]
            seen.add(e0)
            while st:
                e = st.pop()
                grp.append(e)
                for v in e.verts:
                    for e2 in vmap[v]:
                        if e2 not in seen:
                            seen.add(e2)
                            st.append(e2)
            groups.append(grp)
        for grp in groups:
            try:
                r = bmesh.ops.triangle_fill(b, use_beauty=True, use_dissolve=False, edges=grp, normal=Vector(normal))
                new += [g for g in r["geom"] if isinstance(g, bmesh.types.BMFace)]
            except Exception as ex_:
                print("triangle_fill failed", ex_)
        left = [e for e in b.edges if e[lay] == cid and len(e.link_faces) == 1]
    return new, len(left)


def separates(b, c, lay):
    """after the split and fill: no face path joins the faces well on one side of the plane (near the joint) to those well on the other side"""
    P_, n_ = np.array(c["P"]), np.array(c["n"])
    pos, neg = [], []
    for f in b.faces:
        cen = np.array(f.calc_center_median()[:])
        if np.linalg.norm(cen - P_) > 14:
            continue
        d_ = (cen - P_) @ n_
        if d_ > 3:
            pos.append(f)
        elif d_ < -3:
            neg.append(f)
    if not pos or not neg:
        return False
    seen = {pos[0]}
    st = [pos[0]]
    while st:
        f = st.pop()
        for e in f.edges:
            for g in e.link_faces:
                if g not in seen:
                    seen.add(g)
                    st.append(g)
    return not any(f in seen for f in neg) and sum(1 for f in pos if f in seen) > 0.5 * len(pos)


def edge_cut_closes(c):
    """trial on a copy: split the candidate edge cut and fill it; True when every boundary loop of this cut closes"""
    b = bm.copy()
    lay = b.edges.layers.int["cutid"]
    faces = zone_faces(b, c)
    ed = edge_cut(b, c, faces)
    if not ed:
        return False, 0
    for e in ed:
        e[lay] = c["id"]
    bmesh.ops.split_edges(b, edges=ed)
    _, nleft = close_cut(b, lay, c["id"], c["n"])
    left = [None] * nleft
    if not left:
        left = [] if separates(b, c, lay) else [None]
    n_ = len(ed)
    b.free()
    return (len(left) == 0), n_


for c in CUTS:
    if c["name"] == "hip":
        WAIST_TOL[0] = 1.5 if CUTREP[WAIST["id"]]["method"] == "edges" else 0.01
    method = "plane"
    if MODE in ("edges", "auto"):
        ok_ = False
        for off_ in (0.0, 0.7, -0.7, 1.4, -1.4, 2.1, -2.1, 2.8, -2.8):      # shift of the plane along its normal (cm): first one whose edge cut closes and separates
            c_try = dict(c, P=tuple(np.array(c["P"]) + off_ * np.array(c["n"])))
            ok_, n_try = edge_cut_closes(c_try)
            if ok_:
                c = c_try
                c["edge_offset_cm"] = off_
                break
        method = "edges" if ok_ else "plane"
        if MODE == "edges" and not ok_:
            print("WARN edge cut does not close for", c["label"])
    faces = zone_faces(bm, c)
    if method == "plane":
        geom = list(faces) + list({e for f in faces for e in f.edges}) + list({v for f in faces for v in f.verts})
        r = bmesh.ops.bisect_plane(bm, geom=geom, dist=1e-4, plane_co=Vector(c["P"]), plane_no=Vector(c["n"]), clear_inner=False, clear_outer=False)
        edges = [g for g in r["geom_cut"] if isinstance(g, bmesh.types.BMEdge)]
    else:
        edges = edge_cut(bm, c, faces)
    for e in edges:
        e[LAY_CUT] = c["id"]
    CUTREP[c["id"]] = dict(label=c["label"], method=method, edge_plane_offset_cm=c.get("edge_offset_cm"), zone_faces=len(faces), cut_edges=len(edges), section_length_cm=round(sum(e.calc_length() for e in edges), 2))
bm.verts.ensure_lookup_table()
bm.faces.ensure_lookup_table()
UVA_AFTER = uv_area(bm)
bmesh.ops.triangulate(bm, faces=bm.faces[:])
tick("bisect", t)

# ---------------------------------------------------------------- 2. split + caps
t = time.time()
cut_edges = [e for e in bm.edges if e[LAY_CUT] > 0]
bmesh.ops.split_edges(bm, edges=cut_edges)
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.faces.ensure_lookup_table()
if os.environ.get("DBGCUT"):
    for c in CUTS:
        lo, hi = c["box"]
        br = []
        for e in bm.edges:
            a, b2 = e.verts
            da = (np.array(a.co[:]) - np.array(c["P"])) @ c["n"]
            db = (np.array(b2.co[:]) - np.array(c["P"])) @ c["n"]
            if da * db < 0 and abs(da) > 1e-3 and abs(db) > 1e-3:
                mid = np.array(((a.co + b2.co) / 2)[:])
                if np.linalg.norm(mid - np.array(c["P"])) < 14:
                    br.append(([round(x, 1) for x in mid], round(e.calc_length(), 2), len(e.link_faces)))
        print("DBG", c["label"], len(br), br[:6])
for f in bm.faces:
    f[LAY_CAP] = 0
bnd = [e for e in bm.edges if e[LAY_CUT] > 0 and len(e.link_faces) == 1]
tris_before_caps = len(bm.faces)
VCUT = {}
for e in bnd:
    for v in e.verts:
        VCUT[v] = e[LAY_CUT]
new_faces = []
for c in CUTS:
    nf_, nleft_ = close_cut(bm, LAY_CUT, c["id"], c["n"])
    new_faces += nf_
if CFG.get("final_fill"):      # config key (default off): boundary loops left open where two cuts meet (hip cut ending on the waist cut) are filled together, whatever their cut id
    left_ = [e for e in bm.edges if e[LAY_CUT] > 0 and len(e.link_faces) == 1]
    for e in left_:
        e[LAY_CUT] = 999
    for nrm_ in ((0.0, 0.0, 1.0), (0.0, 1.0, 0.0), (1.0, 0.0, 0.0)):
        if not left_:
            break
        nf_, nleft_ = close_cut(bm, LAY_CUT, 999, nrm_)
        new_faces += nf_
        print("FINAL_FILL", len(left_), "edges ->", nleft_, "left, normal", nrm_)
        left_ = [e for e in bm.edges if e[LAY_CUT] == 999 and len(e.link_faces) == 1]
    if left_:      # what is left: coincident duplicate vertices of a sliver; weld them and fill again
        bmesh.ops.remove_doubles(bm, verts=list({v for e in left_ for v in e.verts}), dist=1e-4)
        bm.edges.ensure_lookup_table()
        left_ = [e for e in bm.edges if e[LAY_CUT] == 999 and len(e.link_faces) == 1]
        if left_:
            new_faces += list(bmesh.ops.holes_fill(bm, edges=left_, sides=2000)["faces"])
        left_ = [e for e in bm.edges if e[LAY_CUT] == 999 and len(e.link_faces) == 1]
        print("FINAL_FILL after weld:", len(left_), "open")
new_faces = [f for f in new_faces if f.is_valid]      # faces consumed by the final weld are gone
for f in new_faces:
    f[LAY_CAP] = 1
bmesh.ops.triangulate(bm, faces=new_faces)
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.faces.ensure_lookup_table()
bm.normal_update()
CAPS = [f for f in bm.faces if f[LAY_CAP] == 1]
open_after = [e for e in bm.edges if e[LAY_CUT] > 0 and len(e.link_faces) == 1]
if os.environ.get("DBGCUT"):
    for cid in range(1, 10):
        print("OPEN", cid, [(round(e.verts[0].co.x, 1), round(e.verts[0].co.y, 1), round(e.verts[0].co.z, 1)) for e in open_after if e[LAY_CUT] == cid][:8], "bnd", sum(1 for e in bnd if e[LAY_CUT] == cid))
tick("caps", t)

# ---------------------------------------------------------------- 3. connectivity and assignment
t = time.time()
bm.faces.index_update()
NF = len(bm.faces)
comp = -np.ones(NF, int)
ncomp = 0
for f0 in bm.faces:
    if comp[f0.index] >= 0:
        continue
    st = [f0]
    comp[f0.index] = ncomp
    while st:
        f = st.pop()
        for e in f.edges:
            for g in e.link_faces:
                if comp[g.index] < 0:
                    comp[g.index] = ncomp
                    st.append(g)
    ncomp += 1
fa = np.array([f.calc_area() for f in bm.faces])
fc = np.array([f.calc_center_median()[:] for f in bm.faces])
carea = np.bincount(comp, weights=fa, minlength=ncomp)
ccent = np.array([(fc[comp == k] * fa[comp == k, None]).sum(0) / carea[k] for k in range(ncomp)])
cfaces = np.bincount(comp, minlength=ncomp)
order = np.argsort(-carea)
big = order[:10]
cost = []
for k in big:
    for p in PARTS:
        cost.append((float(np.linalg.norm(ccent[k] - np.array(REFP[p]))), int(k), p))
cost.sort()
assign = {}
usedp = set()
for d, k, p in cost:
    if k in assign or p in usedp:
        continue
    assign[k] = p
    usedp.add(p)
FRAG = []
for k in order[10:]:
    p = min(PARTS, key=lambda q: np.linalg.norm(ccent[k] - np.array(REFP[q])))
    if CFG.get("fragment_rule") == "nearest_face":      # a split-off sliver goes to the part owning the closest face (the panel it hangs on), not to the nearest reference centroid
        big_faces = np.where(np.isin(comp, [int(b) for b in big]))[0]
        d_ = np.linalg.norm(fc[big_faces][None, :, :] - fc[comp == k][:, None, :], axis=2)
        p = assign[int(comp[big_faces[int(np.argmin(d_.min(0)))]])]
    assign[int(k)] = p
    FRAG.append(dict(faces=int(cfaces[k]), area_cm2=round(float(carea[k]), 3), centroid_cm=[round(float(x), 1) for x in ccent[k]], assigned_to=p))
partof = np.array([PARTS.index(assign[int(comp[i])]) for i in range(NF)])
COMP_REP = [dict(part=assign[int(k)], faces=int(cfaces[k]), area_cm2=round(float(carea[k]), 2), centroid_cm=[round(float(x), 1) for x in ccent[k]]) for k in big]
tick("components", t)
print("COMP", ncomp, [(int(k), int(cfaces[k]), round(float(carea[k]),1), [round(float(x),1) for x in ccent[k]], assign[int(k)], [round(float(x)) for x in fc[comp == k].min(0)], [round(float(x)) for x in fc[comp == k].max(0)]) for k in order[:14]])
print("CUTREP", CUTREP, len(bnd), len(open_after))

# ---------------------------------------------------------------- 4. cap orientation and UV
t = time.time()
part_cent = {}
for pi, p in enumerate(PARTS):
    m = partof == pi
    part_cent[p] = (fc[m] * fa[m, None]).sum(0) / fa[m].sum()
CAP_INFO = defaultdict(lambda: dict(faces=0, area=0.0))
flipped = 0
cap_uv_src = []
for f in CAPS:
    pi = partof[f.index]
    pc = part_cent[PARTS[pi]]
    cen = np.array(f.calc_center_median()[:])
    best = None
    for c in CUTS:
        dmax = max(abs((np.array(v.co[:]) - np.array(c["P"])) @ c["n"]) for v in f.verts)
        if dmax < 1e-3 and (best is None or dmax < best[0]):
            best = (dmax, c)
    cid = best[1]["id"] if best else 0
    if not best:
        cid = max(VCUT.get(v, 0) for v in f.verts)
        best = (0.0, [c for c in CUTS if c["id"] == cid][0]) if cid else None
    if best:
        sgn = 1.0 if (pc - np.array(best[1]["P"])) @ best[1]["n"] < 0 else -1.0      # outward = away from the part's side
        if np.array(f.normal[:]) @ (sgn * best[1]["n"]) < 0:
            f.normal_flip()
            flipped += 1
    ci = CAP_INFO[(cid, PARTS[pi])]
    ci["faces"] += 1
    ci["area"] += f.calc_area()
    cands = {g for v in f.verts for g in v.link_faces if g[LAY_CAP] == 0 and partof[g.index] == pi}
    if not cands:
        cands = {g for g in bm.faces if g[LAY_CAP] == 0 and partof[g.index] == pi}
    src = min(cands, key=lambda g: (np.array(g.calc_center_median()[:]) - cen).dot(np.array(g.calc_center_median()[:]) - cen))
    u = sum((l[uv0].uv for l in src.loops), Vector((0, 0))) / len(src.loops)
    for l in f.loops:
        l[uv0].uv = u
    cap_uv_src.append(src.index)
tick("cap_uv", t)

# ---------------------------------------------------------------- 5. parts
t = time.time()
bm.faces.index_update()
bm.normal_update()
bpy.context.scene.unit_settings.scale_length = 1.0
objs = {}
PART_REP = {}


def lightmap(b, nm):
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


for pi, name in enumerate(PARTS):
    b = bm.copy()
    b.faces.ensure_lookup_table()
    kill = [f for f in b.faces if partof[f.index] != pi]
    bmesh.ops.delete(b, geom=kill, context="FACES")
    piv = Vector(PIVOT[name])
    for v in b.verts:
        v.co = v.co - piv
    b.edges.layers.int.remove(b.edges.layers.int["cutid"])
    b.faces.layers.int.remove(b.faces.layers.int["iscap"])
    b.faces.ensure_lookup_table()
    b.faces.index_update()
    sc_ = lightmap(b, name)
    me = bpy.data.meshes.new(name)
    b.to_mesh(me)
    b.free()
    me.shade_flat()
    pid = me.uv_layers.new(name="PartId")      # task-159: third UV channel (Unreal UV2), every corner = ((index + 0.5) / 10, same); vertex colours do not survive the Unreal import
    pid.data.foreach_set("uv", np.full(len(me.loops) * 2, (pi + 0.5) / 10.0, np.float32))
    for m_ in SRC_MATS:
        me.materials.append(m_)
    ca = me.color_attributes.new("PartIndex", "FLOAT_COLOR", "CORNER")
    col = np.tile(np.array([pi / 9.0, 0.0, 0.0, 1.0], np.float32), (len(me.loops), 1))
    ca.data.foreach_set("color", col.ravel())
    me.uv_layers.active_index = 0
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    objs[name] = o
    PART_REP[name] = dict(pivot_cm=[round(x, 3) for x in PIVOT[name]], tris=tri_count(me), vertices=len(me.vertices), lightmap_px_per_cm=round(sc_, 4))
for name in PARTS:
    par = PARENT[name]
    if par:
        objs[name].parent = objs[par]
        objs[name].matrix_parent_inverse = Matrix.Identity(4)
        objs[name].location = Vector(PIVOT[name]) - Vector(PIVOT[par])
    else:
        objs[name].location = Vector(PIVOT[name])
bpy.context.view_layer.update()
tick("parts", t)

# ---------------------------------------------------------------- lightmap check and open edges per part
for name, o in objs.items():
    me = o.data
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
    ec = defaultdict(int)
    for p in me.polygons:
        for k in p.edge_keys:
            ec[k] += 1
    PART_REP[name]["lightmap"] = dict(overlapping_pairs_bbox=ov, min_gap_px=round(gap, 3), in_range=bool(u1.min() >= 0 and u1.max() <= LM), passed=bool(ov == 0 and gap >= 1.0))
    PART_REP[name]["open_edges"] = sum(1 for k, v in ec.items() if v == 1)
    PART_REP[name]["uv_layers"] = [u.name for u in me.uv_layers]
    PART_REP[name]["cap_faces"] = sum(v["faces"] for (cid, pn), v in CAP_INFO.items() if pn == name)

# ---------------------------------------------------------------- joint clearance (min distance between the two parts, away from the cut)
t = time.time()
JOINTS = []
for c in CUTS:
    pair = sorted({pn for (cid, pn) in CAP_INFO if cid == c["id"]})
    rec = dict(label=c["label"], plane_point_cm=[round(x, 2) for x in c["P"]], plane_normal=[round(float(x), 3) for x in c["n"]], zone_box_cm=[list(c["box"][0]), list(c["box"][1])],
               parts=pair, section_length_cm=CUTREP[c["id"]]["section_length_cm"], cut_edges=CUTREP[c["id"]]["cut_edges"],
               caps={pn: dict(faces=v["faces"], area_cm2=round(v["area"], 2)) for (cid, pn), v in CAP_INFO.items() if cid == c["id"]},
               pivot_to_plane_distance_cm=round(abs(float((np.array(PIVOT[c["pivot"] + ("_R" if c["label"].endswith("_R") else ("_L" if c["side"] else ""))]) - np.array(c["P"])) @ c["n"])), 2))
    if len(pair) == 2:
        A = np.array([(objs[pair[0]].matrix_world @ v.co)[:] for v in objs[pair[0]].data.vertices])
        B = np.array([(objs[pair[1]].matrix_world @ v.co)[:] for v in objs[pair[1]].data.vertices])
        A = A[np.abs((A - np.array(c["P"])) @ c["n"]) > 0.6]
        B = B[np.abs((B - np.array(c["P"])) @ c["n"]) > 0.6]
        d = np.linalg.norm(A[:, None, :] - B[None, :, :], axis=2)
        rec["min_vertex_distance_between_parts_beyond_0.6cm_of_plane_cm"] = round(float(d.min()), 2)
    JOINTS.append(rec)
tick("clearance", t)

# ---------------------------------------------------------------- export
t = time.time()
bpy.ops.object.select_all(action="DESELECT")
for o in objs.values():
    o.select_set(True)
bpy.context.view_layer.objects.active = objs["Pelvis"]
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
bpy.ops.export_scene.fbx(filepath=FBX, **EXPORT)
b_ = open(FBX, "rb").read()
usf = None
j = b_.find(b"UnitScaleFactor")
if j >= 0:
    k = b_.find(b"D", j + 20)
    usf = struct.unpack("<d", b_[k + 1:k + 9])[0]
tick("export", t)

# ---------------------------------------------------------------- read back
t = time.time()
obs2 = import_fbx(FBX, WTEX)
by = {o.name: o for o in obs2}
RB = dict(objects=sorted(by), count=len(obs2), parents={n: (o.parent.name if o.parent else None) for n, o in by.items()}, per_part={})
allv = []
tot_tris = tot_v = 0
colour_ok = True
quant_ok = True
PARTID_RB = {}
for pi, name in enumerate(PARTS):
    o = by.get(name)
    if o is None:
        colour_ok = False
        continue
    me = o.data
    W = np.array([(o.matrix_world @ v.co)[:] for v in me.vertices])
    allv.append(W)
    tr = tri_count(me)
    tot_tris += tr
    tot_v += len(me.vertices)
    cav = None
    if "PartIndex" in me.color_attributes:
        cav = np.array([c.color[:] for c in me.color_attributes["PartIndex"].data])
    red = np.unique(np.round(cav[:, 0], 6)).tolist() if cav is not None else None
    alpha = np.unique(np.round(cav[:, 3], 6)).tolist() if cav is not None else None
    exact = bool(cav is not None and np.abs(cav[:, 0] - pi / 9.0).max() < 0.5 / 255 + 1e-6 and np.abs(cav[:, 3] - 1).max() < 1e-5)
    q8 = int(round(pi / 9.0 * 255))
    q_ok = int(round(q8 / 255.0 * 9)) == pi
    quant_ok &= q_ok
    colour_ok &= exact
    pidv = None
    if "PartId" in me.uv_layers:
        a_ = np.array([l.uv[:] for l in me.uv_layers["PartId"].data], np.float64)
        e_ = float(np.float32((pi + 0.5) / 10.0))
        pidv = dict(corners=len(a_), expected=e_, unique_u=np.unique(a_[:, 0]).tolist(), unique_v=np.unique(a_[:, 1]).tolist(), max_abs_error=float(np.abs(a_ - e_).max()), exact_float32=bool(np.abs(a_ - e_).max() < 1e-7))
    PARTID_RB[name] = pidv
    RB["per_part"][name] = dict(tris=tr, vertices=len(me.vertices), world_origin_cm=[round(x, 3) for x in o.matrix_world.translation], local_location_cm=[round(x, 3) for x in o.location],
                                bounds_min=[round(float(x), 2) for x in W.min(0)], bounds_max=[round(float(x), 2) for x in W.max(0)], uv_layers=[u.name for u in me.uv_layers],
                                colour_attribute=(me.color_attributes["PartIndex"].data_type if cav is not None else None), partindex_red_values=red, partindex_alpha_values=alpha,
                                red_equals_index_over_9=exact, int8_value=q8, int8_roundtrip_index_ok=q_ok, materials=[m.name for m in me.materials])
# exact values stored in the file (doubles), read with Blender's own FBX parser
FILE_COLOURS = {}
try:
    from io_scene_fbx import parse_fbx
    root_ = parse_fbx.parse(FBX)[0]
    for el in root_.elems:
        if el.id == b"Objects":
            for g in el.elems:
                if g.id == b"Geometry":
                    gname = g.props[1].split(bytes(1))[0].decode()
                    for le in g.elems:
                        if le.id == b"LayerElementColor":
                            nm_ = [x for x in le.elems if x.id == b"Name"]
                            for x in le.elems:
                                if x.id == b"Colors":
                                    arr = np.array(x.props[0], dtype=np.float64).reshape(-1, 4)
                                    FILE_COLOURS[gname] = dict(layer=(nm_[0].props[0].decode() if nm_ else None), values=len(arr), red_unique=[float(v) for v in np.unique(arr[:, 0])],
                                                               alpha_unique=[float(v) for v in np.unique(arr[:, 3])], green_blue_max=float(arr[:, 1:3].max()))
except Exception as ex_:
    FILE_COLOURS["error"] = str(ex_)
file_exact = bool(len(FILE_COLOURS) == 10 and all(abs(FILE_COLOURS[n]["red_unique"][0] - i / 9.0) < 1e-6 and len(FILE_COLOURS[n]["red_unique"]) == 1 and FILE_COLOURS[n]["alpha_unique"] == [1.0] for i, n in enumerate(PARTS) if n in FILE_COLOURS))
allv = np.concatenate(allv)
RB["total_tris"] = tot_tris
RB["total_vertices"] = tot_v
RB["bounds_min_cm"] = [round(float(x), 3) for x in allv.min(0)]
RB["bounds_max_cm"] = [round(float(x), 3) for x in allv.max(0)]
RB["height_cm"] = round(float(allv[:, 2].max() - allv[:, 2].min()), 3)
tick("readback", t)


# ---------------------------------------------------------------- renders
def setup_workbench(white=True):
    sc = bpy.context.scene
    sc.render.engine = "BLENDER_WORKBENCH"
    sc.display.shading.light = "FLAT"
    sc.display.shading.color_type = "SINGLE"
    sc.display.shading.single_color = (1, 1, 1)
    sc.display.render_aa = "OFF"
    sc.view_settings.view_transform = "Standard"
    sc.render.film_transparent = False
    w = bpy.data.worlds.new("W")
    w.color = (0, 0, 0)
    sc.world = w
    cam = bpy.data.cameras.new("cam")
    cam.type = "ORTHO"
    cam.clip_start, cam.clip_end = 1, 4000
    co = bpy.data.objects.new("cam", cam)
    sc.collection.objects.link(co)
    sc.camera = co
    return sc, co


def aim(co, d, centre, size, px, up=(0, 0, 1)):
    d = Vector(d).normalized()
    fwd = -d
    r = fwd.cross(Vector(up)).normalized()
    u = r.cross(fwd).normalized()
    co.location = Vector(centre) + d * 500
    co.rotation_euler = Matrix((r, u, -fwd)).transposed().to_euler()
    co.data.ortho_scale = size
    bpy.context.view_layer.update()
    bpy.context.scene.render.resolution_x = bpy.context.scene.render.resolution_y = px
    return fwd, r, u


def load_rgb(path):
    img = bpy.data.images.load(path, check_existing=False)
    w, h = img.size
    a = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, 4)[..., :3]
    bpy.data.images.remove(img)
    return a


def save_rgb(path, a):
    h, w = a.shape[:2]
    img = bpy.data.images.new("o", w, h, alpha=False)
    img.colorspace_settings.name = "sRGB"
    z = np.ones((h, w, 4), np.float32)
    z[..., :3] = a
    img.pixels = z.ravel()
    img.filepath_raw = path
    img.file_format = "PNG"
    img.save()
    bpy.data.images.remove(img)


VIEWS = {"front": (1, 0, 0), "side": (0, -1, 0), "back": (-1, 0, 0)}
CENTRE = (0, 0, HEIGHT / 2)
SIL = {}
if True:
    t = time.time()
    masks = {}
    for tag, path in (("uncut", IN_FBX), ("parts", FBX)):
        import_fbx(path)
        sc, co = setup_workbench()
        if tag == "uncut" and SCALE_FIX != 1.0:
            for o in bpy.data.objects:
                if o.type == "MESH":
                    o.scale = (SCALE_FIX,) * 3
        for vn, d in VIEWS.items():
            aim(co, d, CENTRE, CFG.get("view_frame_cm", 150), 2000)
            p = os.path.join(WORK, "sil_%s_%s.png" % (tag, vn))
            sc.render.filepath = p
            bpy.ops.render.render(write_still=True)
            masks[(tag, vn)] = load_rgb(p)[..., 0] > 0.5
    for vn in VIEWS:
        a, b2 = masks[("uncut", vn)], masks[("parts", vn)]
        diff = int((a ^ b2).sum())
        SIL[vn] = dict(uncut_px=int(a.sum()), parts_px=int(b2.sum()), differing_px=diff, only_uncut_px=int((a & ~b2).sum()), only_parts_px=int((b2 & ~a).sum()),
                       difference_percent_of_uncut_area=round(100.0 * diff / a.sum(), 4), px_per_cm=round(2000 / 150, 3), passed=bool(100.0 * diff / a.sum() <= 1.0))
    tick("silhouette", t)

# tinted lit renders
RENDERS = []
if not NORENDER:
    t = time.time()

    def setup_lit():
        sc = bpy.context.scene
        for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
            try:
                sc.render.engine = name
                break
            except TypeError:
                pass
        sc.view_settings.view_transform = "Standard"
        sc.render.film_transparent = False
        try:
            sc.eevee.use_raytracing = True
            sc.eevee.fast_gi_method = "AMBIENT_OCCLUSION_ONLY"
            sc.eevee.fast_gi_distance = 6.0
        except Exception as ex_:
            print("lit: no raytracing/AO", ex_)
        w = bpy.data.worlds.new("W")
        sc.world = w
        w.use_nodes = True
        bgn = w.node_tree.nodes["Background"]
        bgn.inputs["Color"].default_value = (0.62, 0.62, 0.62, 1)
        bgn.inputs["Strength"].default_value = 0.55
        sun = bpy.data.lights.new("sun", "SUN")
        sun.energy = 4.5
        sun.angle = math.radians(14)
        so = bpy.data.objects.new("sun", sun)
        sc.collection.objects.link(so)
        cam = bpy.data.cameras.new("cam")
        cam.type = "ORTHO"
        cam.clip_start, cam.clip_end = 1, 4000
        co = bpy.data.objects.new("cam", cam)
        sc.collection.objects.link(co)
        sc.camera = co
        return sc, co, so

    def lit_shot(sc, co, so, d, path):
        fwd, r, u = aim(co, d, CENTRE, CFG.get("lit_frame_cm", 140), 1100)
        key = (fwd + r * 0.75 + Vector((0, 0, -1.1))).normalized()
        so.rotation_euler = key.to_track_quat("-Z", "Y").to_euler()
        sc.render.filepath = path
        bpy.ops.render.render(write_still=True)

    import colorsys
    for tag in ("before", "after"):
        o2 = import_fbx(IN_FBX if tag == "before" else FBX, WTEX if tag == "before" else None)
        sc, co, so = setup_lit()
        if tag == "before" and SCALE_FIX != 1.0:
            for o in o2:
                o.scale = (SCALE_FIX,) * 3
        if tag == "after":
            for pi, name in enumerate(PARTS):
                o = [x for x in o2 if x.name == name][0]
                m = bpy.data.materials.new("tint_" + name)
                m.use_nodes = True
                pb = m.node_tree.nodes["Principled BSDF"]
                rgb = colorsys.hsv_to_rgb((pi * 0.381966) % 1.0, 0.75 if pi % 2 == 0 else 0.55, 0.95)
                pb.inputs["Base Color"].default_value = (*rgb, 1)
                pb.inputs["Roughness"].default_value = 0.7
                o.data.materials.clear()
                o.data.materials.append(m)
        for vn in ("front", "side"):
            p = os.path.join(WO, "renders", "parts", "parts_%s%s.png" % (vn, "_before" if tag == "before" else ""))
            lit_shot(sc, co, so, VIEWS[vn], p)
            RENDERS.append("renders/parts/" + os.path.basename(p))
    tick("renders", t)

# ---------------------------------------------------------------- report
WARN = []
TRIS_LIMIT = CFG.get("tris_limit", 3300)
if tot_tris > TRIS_LIMIT:
    WARN.append("total triangles above %d" % TRIS_LIMIT)
if not all(s["passed"] for s in SIL.values()):
    WARN.append("silhouette difference above 1 percent in a view")
if not file_exact:
    WARN.append("PartIndex red channel in the file is not exactly index/9")
if open_after:
    WARN.append("%d cap boundary edges still open (loop not closed)" % len(open_after))
if any(not v["lightmap"]["passed"] for v in PART_REP.values()):
    WARN.append("lightmap check failed on a part")
if FRAG:
    WARN.append("%d small connected fragments assigned to the nearest part" % len(FRAG))
cap_total = len(CAPS)
rep = dict(
    task=CFG.get("task", "task-157-01-chaser-run-attack-animation"),
    inputs=dict(fbx=os.path.basename(opt["--in-fbx"]), texture=TEXNAME, source=SRC_STATS, source_tris_triangulated=TRIS_IN, source_vertices=VERTS_IN, source_open_edges=SRC_OPEN_EDGES, source_nonmanifold_edges=SRC_NONMANIFOLD_EDGES),
    outputs=dict(fbx=OUT_NAME + ".fbx", renders=RENDERS, texture_referenced=TEXNAME, embedded=False),
    joints_cm=dict(found_on_mesh={k: list(v) for k, v in J.items()}, note=CFG.get("joint_note", "left side (+Y); right = mirror through y = 0. waist = narrowest section of the neck (red ring, z 84-90), hip = centre of the dark upper thigh block, knee = centre of the dark knee ball (side view z 50.9), shoulder = centre of the dark shoulder pod, elbow = centre of the dark elbow ball (front and side views agree on y 27.4, z 93.4)")),
    pivots_cm={k: [round(x, 3) for x in v] for k, v in PIVOT.items()},
    hierarchy=PARENT,
    cut_planes=JOINTS,
    plane_method="visual guess from orthographic close-ups, then a minimum section-length scan over offset (cm) and tilt (deg) around it inside the zone box; hips only count faces below the waist plane",
    uv_interpolation_check=dict(uv_area_before=round(UVA_BEFORE, 6), uv_area_after_bisect=round(UVA_AFTER, 6), surface_area_cm2_before=round(AREA_BEFORE, 2)),
    triangles=dict(total=tot_tris, limit=TRIS_LIMIT, source=TRIS_IN, cap_faces_added=cap_total, cut_triangles_added_by_bisect_and_split=int(tris_before_caps - TRIS_IN), vertices_total=tot_v, vertices_source=VERTS_IN,
                   per_part={n: dict(tris=v["tris"], vertices=v["vertices"], cap_faces=v["cap_faces"], open_edges=v["open_edges"]) for n, v in PART_REP.items()}),
    cap_boundary_edges_before_fill=len(bnd), cap_boundary_edges_still_open=len(open_after), cap_faces_flipped_to_outward=flipped,
    cap_uv_method="every cap triangle gets all three UV0 corners at the UV centroid of the nearest existing (non-cap) face of the same part that shares a vertex with the cap, so it samples that face's texels (flat colour); texture file untouched",
    connected_pieces=dict(total_after_cut=int(ncomp), ten_largest=COMP_REP, small_fragments=FRAG),
    parts=PART_REP,
    silhouette=dict(views=SIL, limit_percent=1.0, method="orthographic 150 cm frame, 2000 px (13.33 px/cm), flat white on black, no anti-aliasing; uncut = input FBX, parts = exported FBX re-imported; difference = XOR of the masks over the uncut area"),
    vertex_colour_check=dict(attribute="PartIndex", domain="CORNER", type="FLOAT_COLOR written; exported with colors_type LINEAR (no gamma); the FBX exporter stores the values as float32 (error below 1e-7)", expected_red="index / 9", all_parts_read_back_exact_tol_1e5=bool(colour_ok), file_float_values_exact_red_index_over_9=file_exact, file_values_per_part=FILE_COLOURS,
                             int8_roundtrip_all_ok=bool(quant_ok), int8_note="if a reader quantises to 8 bits: round(index/9*255) values " + str([int(round(i / 9 * 255)) for i in range(10)]) + " read back to index via round(v/255*9)"),
    export_settings=dict(**{k: (sorted(v) if isinstance(v, set) else v) for k, v in EXPORT.items()}, blender=bpy.app.version_string, scene_unit_scale_length=1.0, fbx_unit_scale_factor_in_file=usf),
    partid_uv_check=dict(layer="PartId", rule="both coordinates of every corner = (index + 0.5) / 10", order_expected=["UVMap", "LightMap", "PartId"], read_from_exported_fbx=PARTID_RB,
                         all_exact=bool(len(PARTID_RB) == 10 and all(v and v["exact_float32"] and len(v["unique_u"]) == 1 and len(v["unique_v"]) == 1 for v in PARTID_RB.values())),
                         uv_layer_order_per_part={n: v["uv_layers"] for n, v in RB["per_part"].items()}),
    readback=RB, warnings=WARN, timings_s=dict(TIM, total=round(time.time() - T0, 2)),
)
with open(os.path.join(WO, OUT_NAME + ".report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
os.makedirs(OUT, exist_ok=True)
for root, _, files in os.walk(WO):
    rel = os.path.relpath(root, WO)
    os.makedirs(os.path.join(OUT, rel), exist_ok=True)
    for fn in files:
        dst = os.path.join(OUT, rel, fn)
        if fn == TEXNAME and os.path.exists(dst):
            continue
        shutil.copyfile(os.path.join(root, fn), dst)
print("DONE", json.dumps(dict(tris=tot_tris, caps=cap_total, silhouette={k: v["difference_percent_of_uncut_area"] for k, v in SIL.items()}, warnings=WARN, usf=usf)))
