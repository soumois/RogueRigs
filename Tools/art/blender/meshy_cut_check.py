"""task-161-01 (also robots with walk_poses in the anim report, task-284-01): check the part cut of the chaser parts FBX and render four fixed run poses (frames 0, 4, 8, 12).
Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_cut_check.py -- --in-fbx Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx --parts-report Art/Meshes/chaser-meshy/chaser_meshy_parts.report.json --in-exr Art/Meshes/chaser-meshy/T_ChaserAnim.exr --anim-report Art/Meshes/chaser-meshy/anim_report.json --in-tex Art/Meshes/chaser-meshy/chaser_meshy_fixed_basecolor.png --out-dir Art/Meshes/chaser-meshy [--work <short dir>] [--no-render]
Part 1: cut check -> <out-dir>/cut_check.report.json, renders/check/parts_isolated_*.png.
Part 2: poses from the saved EXR (line by line port of the Unreal material Custom node) and from the keyed hierarchy (same evaluation as meshy_bake_anim.py)
        -> renders/check/pose_run_{front,side}.png and pose_run_keyed_{front,side}.png, max vertex differences in the report.
Space: Blender, cm, X forward, Y left, Z up. The FBX is read back in Blender; the Unreal import mirrors Y (UE y = -Blender y), emulated for the EXR path.
Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, sys, os, json, math, shutil, colorsys, struct
import numpy as np
from mathutils import Vector, Matrix, Euler

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
NORENDER = "--no-render" in argv
argv = [a for a in argv if a != "--no-render"]
opt = {"--in-fbx": "", "--parts-report": "", "--in-exr": "", "--anim-report": "", "--in-tex": "", "--out-dir": "", "--work": "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work", "--height-cm": "123", "--frames": "0,4,8,12"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-fbx", "--parts-report", "--in-exr", "--anim-report", "--in-tex", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
os.makedirs(WORK, exist_ok=True)
HEIGHT = float(opt["--height-cm"])
FRAMES = [int(x) for x in opt["--frames"].split(",")]


def stage(src, name):
    d = os.path.join(WORK, name)
    shutil.copyfile(src, d)
    return d


IN_FBX = stage(opt["--in-fbx"], "cc_parts.fbx")
EXR = stage(opt["--in-exr"], "cc_anim.exr")
TEX = stage(opt["--in-tex"], "cc_basecolor.png")
PREP = json.load(open(opt["--parts-report"], encoding="utf-8"))
AREP = json.load(open(opt["--anim-report"], encoding="utf-8"))
PRM = AREP["parameters"]
A_THIGH, A_KNEE, A_ARM, A_ELBOW, A_LEAN, A_TWIST = (PRM[k] for k in ("thigh_swing_deg", "knee_flexion_max_deg", "arm_swing_deg", "elbow_bend_deg", "torso_lean_deg", "torso_twist_deg"))
NRUN = AREP["clips"].get("run", AREP["clips"].get("walk"))["length"]
WALK_POSES = AREP.get("walk_poses")      # robots baked with walk_style planted store their keyed poses in the report

PARTS = [p["name"] for p in AREP["parts"]]
PARENT = {p["name"]: p["parent"] for p in AREP["parts"]}
assert PARTS == ["Pelvis", "HeadTorso", "UpperArm_L", "UpperArm_R", "ForearmHand_L", "ForearmHand_R", "Thigh_L", "Thigh_R", "ShinFoot_L", "ShinFoot_R"]
WARN = []

# ---------------------------------------------------------------- import
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=IN_FBX, colors_type="LINEAR")
OB = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
assert sorted(OB) == sorted(PARTS), sorted(OB)
bpy.context.view_layer.update()
REST_LOC, REST_P, REST_V, TRIS, PARTID = {}, {}, {}, {}, {}
for n in PARTS:
    o = OB[n]
    o.rotation_mode = "XYZ"
    m = o.matrix_world
    REST_LOC[n] = o.location.copy()
    REST_P[n] = np.array(m.translation[:], float)
    REST_V[n] = np.array([v.co[:] for v in o.data.vertices], float) + REST_P[n]
    o.data.calc_loop_triangles()
    TRIS[n] = np.array([t.vertices[:] for t in o.data.loop_triangles], int)
    uvl = o.data.uv_layers.get("PartId")
    pid = np.full(len(o.data.vertices), -1, int)
    if uvl is not None:
        for lp in o.data.loops:
            u = uvl.data[lp.index].uv[0]
            pid[lp.vertex_index] = int(math.floor(u * 10.0))
    else:
        WARN.append("%s has no PartId UV layer" % n)
    PARTID[n] = pid
    exp = PARTS.index(n)
    if uvl is not None and not np.all(pid == exp):
        WARN.append("%s: PartId UV gives part index %s, expected %d" % (n, sorted(set(pid.tolist())), exp))

# ---------------------------------------------------------------- Part 1: cut check
TOL = 0.05


def merge_ids(V):
    key = np.round(V * 1e4).astype(np.int64)
    _, inv = np.unique(key, axis=0, return_inverse=True)
    return inv.reshape(-1)


def components(tris, mid):
    parent = {}

    def find(a):
        while parent.setdefault(a, a) != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a
    for t in tris:
        a, b, c = (int(mid[i]) for i in t)
        ra, rb, rc = find(a), find(b), find(c)
        parent[rb] = ra
        parent[find(rc)] = find(ra)
    lab = np.array([find(int(mid[t[0]])) for t in tris])
    uniq = {l: i for i, l in enumerate(sorted(set(lab.tolist())))}
    return np.array([uniq[l] for l in lab])


def edge_stats(tris, mid):
    cnt = {}
    for t in tris:
        a, b, c = (int(mid[i]) for i in t)
        for e in ((a, b), (b, c), (c, a)):
            k = (min(e), max(e))
            cnt[k] = cnt.get(k, 0) + 1
    return sum(1 for v in cnt.values() if v == 1), sum(1 for v in cnt.values() if v > 2)


def r3(a):
    return [round(float(x), 3) for x in a]


PLANES = {p["label"]: p for p in PREP["cut_planes"]}
PIV = {n: np.array(PREP["pivots_cm"][n], float) for n in PARTS}
for n in PARTS:
    if np.abs(PIV[n] - REST_P[n]).max() > 1e-3:
        WARN.append("pivot of %s differs from the parts report" % n)
ANOM = []
PLDEV = []
CUT = {}
CENT = {}
MAIN = {}
for n in PARTS:
    V, T = REST_V[n], TRIS[n]
    mid = merge_ids(V)
    cl = components(T, mid)
    ncomp = int(cl.max()) + 1
    cen = V[T].mean(1)
    CENT[n] = cen
    comps = []
    for c in range(ncomp):
        sel = np.where(cl == c)[0]
        vv = np.unique(T[sel].ravel())
        comps.append(dict(index=c, tris=int(len(sel)), vertices=int(len(vv)), centroid_cm=r3(cen[sel].mean(0)), bbox_min_cm=r3(V[vv].min(0)), bbox_max_cm=r3(V[vv].max(0))))
    main = max(range(ncomp), key=lambda c: comps[c]["tris"])
    MAIN[n] = np.where(cl == main)[0]
    if ncomp > 1:
        ANOM.append("%s: %d connected components, tris %s" % (n, ncomp, [c["tris"] for c in comps]))
    # triangles far from the main component (distance from the centroid to the main component bbox, cm)
    mv = np.unique(T[MAIN[n]].ravel())
    bmin, bmax = V[mv].min(0), V[mv].max(0)
    d = np.linalg.norm(np.maximum(np.maximum(bmin - cen, cen - bmax), 0.0), axis=1)
    mdist = np.linalg.norm(cen - cen[MAIN[n]].mean(0), axis=1)
    far_sel = [i for i in range(len(T)) if cl[i] != main]
    far = [dict(tri=int(i), component=int(cl[i]), centroid_cm=r3(cen[i]), distance_to_main_bbox_cm=round(float(d[i]), 3), distance_to_main_centroid_cm=round(float(mdist[i]), 3)) for i in far_sel]
    n_open, n_nm = edge_stats(T, mid)
    n_open_idx, n_nm_idx = edge_stats(T, np.arange(len(V)))
    dj = np.linalg.norm(V - REST_P[n], axis=1)
    par = PARENT[n]
    dpar = np.linalg.norm(V - REST_P[par], axis=1) if par else None
    allv = np.unique(T.ravel())
    CUT[n] = dict(
        tris=int(len(T)), vertices=int(len(V)), vertices_unreferenced=int(len(V) - len(allv)), merged_positions=int(mid.max() + 1),
        connected_components=dict(count=ncomp, per_component=comps, main_component=int(main)),
        bbox_min_cm=r3(V.min(0)), bbox_max_cm=r3(V.max(0)), bbox_size_cm=r3(V.max(0) - V.min(0)),
        pivot_cm=r3(REST_P[n]), pivot_inside_bbox=bool(np.all(REST_P[n] >= V.min(0)) and np.all(REST_P[n] <= V.max(0))),
        triangles_not_in_main_component=dict(count=len(far), list=far[:40]),
        open_edges=n_open_idx, nonmanifold_edges=n_nm_idx, open_edges_merged_by_position=n_open, nonmanifold_edges_merged_by_position=n_nm,
        vertex_distance_to_own_joint_cm=dict(min=round(float(dj.min()), 3), max=round(float(dj.max()), 3), mean=round(float(dj.mean()), 3)),
        vertex_distance_to_parent_joint_cm=(dict(parent=par, min=round(float(dpar.min()), 3), max=round(float(dpar.max()), 3)) if par else None),
        planes={})
    exp_open = PREP["parts"][n]["open_edges"]
    if n_open_idx != exp_open:
        ANOM.append("%s: open edges %d by vertex index (%d merged by position 1e-4 cm), %d in the parts report" % (n, n_open_idx, n_open, exp_open))
    elif n_open_idx:
        ANOM.append("%s: %d open edges by vertex index (same as the parts report; %d merged by position)" % (n, n_open_idx, n_open))
    if n_nm_idx:
        ANOM.append("%s: %d non-manifold edges by vertex index (%d merged by position)" % (n, n_nm_idx, n_nm))
    exp_t = PREP["readback"]["per_part"][n]["tris"]
    if len(T) != exp_t:
        ANOM.append("%s: %d triangles in the FBX, %d in the parts report readback" % (n, len(T), exp_t))
    if n_open == 0 and False:
        pass

# side of the joint plane
WAIST = PLANES["waist"]
wn = np.array(WAIST["plane_normal"], float)
wn /= np.linalg.norm(wn)
wp = np.array(WAIST["plane_point_cm"], float)


def sd(P, pl):
    nn = np.array(pl["plane_normal"], float)
    nn /= np.linalg.norm(nn)
    return (P - np.array(pl["plane_point_cm"], float)) @ nn


PLANE_SIDE = {}
for label, pl in PLANES.items():
    zb = np.array(pl["zone_box_cm"], float)
    sides = {}
    for n in pl["parts"]:
        c = CENT[n][MAIN[n]]
        s = sd(c, pl)
        sides[n] = 1.0 if s.mean() >= 0 else -1.0
    PLANE_SIDE[label] = sides
    if len(pl["parts"]) == 2 and sides[pl["parts"][0]] == sides[pl["parts"][1]]:
        ANOM.append("plane %s: both parts have their main component on the same side" % label)
    for n in pl["parts"]:
        V, T, cen = REST_V[n], TRIS[n], CENT[n]
        inbox = np.all((cen >= zb[0]) & (cen <= zb[1]), axis=1)
        if label.startswith("hip"):
            wside = PLANE_SIDE["waist"].get("Pelvis", 1.0)
            inbox &= (sd(cen, WAIST) * wside > 0.6)
        sdc = sd(cen, pl) * sides[n]
        wrong = np.where(inbox & (sdc < -TOL))[0]
        vin = np.all((V >= zb[0]) & (V <= zb[1]), axis=1)
        sdv = sd(V, pl) * sides[n]
        if label.startswith("hip"):
            vin &= (sd(V, WAIST) * PLANE_SIDE["waist"].get("Pelvis", 1.0) > 0.6)
        wv = np.where(vin & (sdv < -TOL))[0]
        rec = dict(expected_side_sign=sides[n], triangles_in_zone=int(inbox.sum()), vertices_in_zone=int(vin.sum()),
                   wrong_side_triangle_centroids=int(len(wrong)), worst_triangle_depth_cm=round(float(-sdc[wrong].min()), 3) if len(wrong) else 0.0,
                   wrong_side_vertices=int(len(wv)), worst_vertex_depth_cm=round(float(-sdv[wv].min()), 3) if len(wv) else 0.0,
                   min_signed_distance_triangle_centroid_in_zone_cm=round(float(sdc[inbox].min()), 3) if inbox.any() else None,
                   tolerance_cm=TOL)
        if len(wrong):
            rec["wrong_triangles"] = [dict(tri=int(i), centroid_cm=r3(cen[i]), depth_cm=round(float(-sdc[i]), 3)) for i in wrong[:20]]
            PLDEV.append("%s vs plane %s: %d triangle centroids on the wrong side (worst %.3f cm), %d vertices (worst %.3f cm)" % (n, label, len(wrong), -sdc[wrong].min(), len(wv), -sdv[wv].min() if len(wv) else 0.0))
        CUT[n]["planes"][label] = rec

# pivot against the plane (information)
for label, pl in PLANES.items():
    for n in pl["parts"]:
        CUT[n]["planes"][label]["pivot_signed_distance_to_plane_cm"] = round(float(sd(REST_P[n][None], pl)[0] * PLANE_SIDE[label][n]), 3)

# stray fragments (small components)
STRAY = []
for n in PARTS:
    cc = CUT[n]["connected_components"]
    for c in cc["per_component"]:
        if c["index"] != cc["main_component"]:
            STRAY.append(dict(part=n, tris=c["tris"], vertices=c["vertices"], centroid_cm=c["centroid_cm"], bbox_min_cm=c["bbox_min_cm"], bbox_max_cm=c["bbox_max_cm"],
                              distance_centroid_to_own_joint_cm=round(float(np.linalg.norm(np.array(c["centroid_cm"]) - REST_P[n])), 3),
                              distance_centroid_to_main_component_centroid_cm=round(float(np.linalg.norm(np.array(c["centroid_cm"]) - np.array(cc["per_component"][cc["main_component"]]["centroid_cm"]))), 3),
                              min_z_cm=c["bbox_min_cm"][2], max_z_cm=c["bbox_max_cm"][2]))

# ---------------------------------------------------------------- Part 2: pose maths
# keyed path (same functions as meshy_bake_anim.py)
FOOT = ["ShinFoot_L", "ShinFoot_R"]


def apply_pose(pose):
    for n in PARTS:
        o = OB[n]
        p = pose.get(n, {})
        o.rotation_euler = Euler([math.radians(a) for a in p.get("rot", (0, 0, 0))], "XYZ")
        o.location = REST_LOC[n] + Vector(p.get("loc", (0, 0, 0)))
    bpy.context.view_layer.update()


def world_verts(n):
    m = np.array(OB[n].matrix_world)
    loc = np.array([v.co[:] for v in OB[n].data.vertices], float)
    return loc @ m[:3, :3].T + m[:3, 3]


def foot_min():
    return [float(world_verts(f)[:, 2].min()) for f in FOOT]


def fit_ground(pose):
    pose.setdefault("Pelvis", {})
    pose["Pelvis"]["loc"] = tuple(pose["Pelvis"].get("loc", (0, 0, 0)))[:2] + (0.0,)
    apply_pose(pose)
    dz = -min(foot_min())
    pose["Pelvis"]["loc"] = pose["Pelvis"]["loc"][:2] + (dz,)
    apply_pose(pose)
    return dz


def run_pose(phase):
    th = 2 * math.pi * phase
    s = math.sin(th)
    pose = {}
    pose["HeadTorso"] = dict(rot=(0, A_LEAN, A_TWIST * s))
    pose["Thigh_L"] = dict(rot=(0, -A_THIGH * s, 0))
    pose["Thigh_R"] = dict(rot=(0, A_THIGH * s, 0))
    pose["ShinFoot_L"] = dict(rot=(0, A_KNEE * max(0.0, math.cos(th)), 0))
    pose["ShinFoot_R"] = dict(rot=(0, A_KNEE * max(0.0, -math.cos(th)), 0))
    pose["UpperArm_L"] = dict(rot=(0, A_ARM * s, 0))
    pose["UpperArm_R"] = dict(rot=(0, -A_ARM * s, 0))
    pose["ForearmHand_L"] = dict(rot=(0, -A_ELBOW, 0))
    pose["ForearmHand_R"] = dict(rot=(0, -A_ELBOW, 0))
    return pose


KEYED = {}
KEYED_POSE = {}
for f in range(NRUN):
    if WALK_POSES:
        pose = WALK_POSES[f]
        apply_pose(pose)
    else:
        pose = run_pose(f / NRUN)
        fit_ground(pose)
    KEYED[f] = {n: world_verts(n) for n in PARTS}
    KEYED_POSE[f] = pose
apply_pose({})

# EXR path: re-load the saved file
img_e = bpy.data.images.load(EXR, check_existing=False)
img_e.colorspace_settings.name = "Non-Color"
EW, EH = img_e.size
buf = np.array(img_e.pixels[:], np.float32).reshape(EH, EW, 4)
TEXV = np.flipud(buf).copy()       # TEXV[frame, x]; frame f = scanline f counted from the top of the file


def read_exr_raw(path):
    d = open(path, "rb").read()
    pos = 8
    attrs = {}
    while d[pos] != 0:
        e = d.index(b"\0", pos)
        name = d[pos:e].decode()
        e2 = d.index(b"\0", e + 1)
        size = struct.unpack("<i", d[e2 + 1:e2 + 5])[0]
        attrs[name] = d[e2 + 5:e2 + 5 + size]
        pos = e2 + 5 + size
    pos += 1
    ch, cd, p = [], attrs["channels"], 0
    while cd[p] != 0:
        e = cd.index(b"\0", p)
        ch.append((cd[p:e].decode(), struct.unpack("<i", cd[e + 1:e + 5])[0]))
        p = e + 1 + 16
    dw = struct.unpack("<iiii", attrs["dataWindow"])
    w, h = dw[2] - dw[0] + 1, dw[3] - dw[1] + 1
    offs = struct.unpack("<%dq" % h, d[pos:pos + 8 * h])
    rows = []
    for o in offs:
        y, sz = struct.unpack("<ii", d[o:o + 8])
        data = d[o + 8:o + 8 + sz]
        a = np.zeros((w, len(ch)), np.float32)
        q = 0
        for ci, (nm, pt) in enumerate(ch):
            if pt == 1:
                a[:, ci] = np.frombuffer(data, np.float16, w, q)
                q += 2 * w
            else:
                a[:, ci] = np.frombuffer(data, np.float32, w, q)
                q += 4 * w
        rows.append((y, a))
    names = [c[0] for c in ch]
    arr = np.stack([r[1] for r in sorted(rows, key=lambda r: r[0])])
    return arr[:, :, [names.index("R"), names.index("G"), names.index("B"), names.index("A")]]


RAWX = read_exr_raw(EXR)
exr_load_vs_raw = float(np.abs(RAWX - TEXV).max())


def f3(x):
    return np.asarray(x, np.float32)


def cross(a, b):
    return np.stack([a[..., 1] * b[..., 2] - a[..., 2] * b[..., 1], a[..., 2] * b[..., 0] - a[..., 0] * b[..., 2], a[..., 0] * b[..., 1] - a[..., 1] * b[..., 0]], axis=-1)


def material_offset(LocalPos, part, f):
    """line by line port of the Unreal Custom node (HLSL), vectorised over the vertices of one part.
    AnimTex.Load(int3(x, y, 0)) = texel x of row y; t_frame = integer frame so ra = rb = f and fr = 0."""
    q = f3(TEXV[f, 2 * part])                                          # float4 q = AnimTex.Load(int3(2*part, f, 0));
    q = q * np.float32(1.0 / math.sqrt(max(float(np.dot(q, q)), 1e-8)))  # q = q * rsqrt(max(dot(q,q),1e-8));
    q = np.array([-q[0], q[1], -q[2], q[3]], np.float32)               # q = float4(-q.x, q.y, -q.z, q.w);
    tr = f3(TEXV[f, 2 * part + 1])[:3]                                 # float3 tr = AnimTex.Load(int3(2*part+1, f, 0)).xyz;
    tr = np.array([tr[0], -tr[1], tr[2]], np.float32)                  # tr = float3(tr.x, -tr.y, tr.z);
    u = q[:3]                                                          # float3 u = q.xyz;
    c = cross(np.broadcast_to(u, LocalPos.shape), LocalPos)            # float3 c = cross(u, LocalPos);
    r = LocalPos + np.float32(2.0) * (q[3] * c + cross(np.broadcast_to(u, LocalPos.shape), c))   # float3 r = LocalPos + 2.0 * (q.w * c + cross(u, c));
    return r + tr - LocalPos                                           # return r + tr - LocalPos;


def exr_pose(f):
    out = {}
    for pi, n in enumerate(PARTS):
        V = REST_V[n]
        part = PARTID[n]
        assert np.all(part == pi)
        LocalPos = f3(np.stack([V[:, 0], -V[:, 1], V[:, 2]], axis=1))    # Unreal mesh space: y mirrored
        off = material_offset(LocalPos, pi, f)
        new = LocalPos + off
        out[n] = np.stack([new[:, 0], -new[:, 1], new[:, 2]], axis=1).astype(float)    # mirrored back for Blender
    return out


EXRV = {f: exr_pose(f) for f in range(NRUN)}

# comparison EXR path vs keyed path
DIFF = {}
for f in range(NRUN):
    per = {}
    for n in PARTS:
        dv = EXRV[f][n] - KEYED[f][n]
        i = int(np.linalg.norm(dv, axis=1).argmax())
        per[n] = dict(max_norm_cm=round(float(np.linalg.norm(dv, axis=1).max()), 5), max_abs_x_cm=round(float(np.abs(dv[:, 0]).max()), 5),
                      max_abs_y_cm=round(float(np.abs(dv[:, 1]).max()), 5), max_abs_z_cm=round(float(np.abs(dv[:, 2]).max()), 5), worst_vertex=i,
                      worst_vertex_diff_xyz_cm=r3(dv[i]))
    DIFF[f] = dict(max_cm=max(p["max_norm_cm"] for p in per.values()), per_part=per)
if any(DIFF[f]["max_cm"] > 0.1 for f in range(NRUN)):
    for f in range(NRUN):
        for n, p in DIFF[f]["per_part"].items():
            if p["max_norm_cm"] > 0.1:
                ANOM.append("EXR path vs keyed path: frame %d part %s max %.3f cm (x %.3f y %.3f z %.3f)" % (f, n, p["max_norm_cm"], p["max_abs_x_cm"], p["max_abs_y_cm"], p["max_abs_z_cm"]))

# ---------------------------------------------------------------- renders
RENDERS = []
CHK = os.path.join(OUT, "renders", "check")
if not NORENDER:
    sc = bpy.context.scene
    ims = sc.render.image_settings
    img_tex = bpy.data.images.load(TEX, check_existing=False)
    img_tex.colorspace_settings.name = "sRGB"
    for o in OB.values():
        for m in o.data.materials:
            if m and m.node_tree:
                for nd in m.node_tree.nodes:
                    if nd.type == "TEX_IMAGE":
                        nd.image = img_tex
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
    gme = bpy.data.meshes.new("floor")
    gv = [(-75, -40, -0.6), (75, -40, -0.6), (75, 40, -0.6), (-75, 40, -0.6), (-75, -40, 0), (75, -40, 0), (75, 40, 0), (-75, 40, 0)]
    gme.from_pydata(gv, [], [(0, 1, 2, 3), (4, 7, 6, 5), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)])
    gm = bpy.data.materials.new("floor")
    gm.use_nodes = True
    gm.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.08, 0.08, 0.1, 1)
    gme.materials.append(gm)
    go = bpy.data.objects.new("floor", gme)
    sc.collection.objects.link(go)
    # display objects: unparented copies at world positions
    DISP = {}
    TEXMAT = OB[PARTS[0]].data.materials[0]
    for n in PARTS:
        me2 = OB[n].data.copy()
        d = bpy.data.objects.new("D_" + n, me2)
        sc.collection.objects.link(d)
        DISP[n] = d
        OB[n].hide_render = True
    COLMAT = {}
    for i, n in enumerate(PARTS):
        mt = bpy.data.materials.new("col_" + n)
        mt.use_nodes = True
        rgb = colorsys.hsv_to_rgb(i / len(PARTS), 0.75, 0.95)
        mt.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (rgb[0], rgb[1], rgb[2], 1)
        mt.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 0.8
        COLMAT[n] = mt
    txm = bpy.data.materials.new("label")
    txm.use_nodes = True
    nt = txm.node_tree
    nt.nodes.clear()
    em = nt.nodes.new("ShaderNodeEmission")
    em.inputs["Color"].default_value = (0.02, 0.02, 0.02, 1)
    outn = nt.nodes.new("ShaderNodeOutputMaterial")
    nt.links.new(em.outputs[0], outn.inputs[0])

    def set_mats(kind):
        for n in PARTS:
            me = DISP[n].data
            if len(me.materials) == 0:
                me.materials.append(None)
            me.materials[0] = COLMAT[n] if kind == "colour" else TEXMAT

    def set_pos(vd):
        for n in PARTS:
            me = DISP[n].data
            me.vertices.foreach_set("co", np.asarray(vd[n], np.float32).ravel())
            me.update()

    def show(names):
        for n in PARTS:
            DISP[n].hide_render = n not in names

    VIEWS = {"side": (0, -1, 0), "front": (1, 0, 0)}
    PX = 480

    def aim(d, centre, size, px, up=(0, 0, 1)):
        d = Vector(d).normalized()
        fwd = -d
        r = fwd.cross(Vector(up)).normalized()
        u = r.cross(fwd).normalized()
        co.location = Vector(centre) + d * 500
        co.rotation_euler = Matrix((r, u, -fwd)).transposed().to_euler()
        co.data.ortho_scale = size
        sc.render.resolution_x = sc.render.resolution_y = px
        bpy.context.view_layer.update()
        return fwd, r, u

    def shot(d, centre, size, text, tsize):
        fwd, r, u = aim(d, centre, size, PX)
        key = (fwd + r * 0.75 + Vector((0, 0, -1.1))).normalized()
        so.rotation_euler = key.to_track_quat("-Z", "Y").to_euler()
        cu = bpy.data.curves.new("lab", "FONT")
        cu.body = text
        cu.size = tsize * size
        ob = bpy.data.objects.new("lab", cu)
        cu.materials.append(txm)
        sc.collection.objects.link(ob)
        ob.rotation_euler = co.rotation_euler
        ob.location = co.matrix_world @ Vector((-0.47 * size, 0.40 * size, -300))
        bpy.context.view_layer.update()
        path = os.path.join(WORK, "tile.png")
        sc.render.filepath = path
        ims.file_format = "PNG"
        ims.color_mode = "RGB"
        ims.color_depth = "8"
        bpy.ops.render.render(write_still=True)
        bpy.data.objects.remove(ob)
        bpy.data.curves.remove(cu)
        im = bpy.data.images.load(path, check_existing=False)
        a = np.array(im.pixels[:], np.float32).reshape(PX, PX, 4)
        bpy.data.images.remove(im)
        return a

    def save_sheet(rows, fname):
        a = np.concatenate([np.concatenate(r, axis=1) for r in rows[::-1]], axis=0)     # Blender row 0 = bottom: first row of tiles goes on top
        h, wd = a.shape[:2]
        o = bpy.data.images.new("sheet", wd, h, alpha=False)
        o.colorspace_settings.name = "sRGB"
        o.pixels.foreach_set(a.ravel())
        o.filepath_raw = os.path.join(WORK, fname)
        o.file_format = "PNG"
        o.save()
        bpy.data.images.remove(o)
        os.makedirs(CHK, exist_ok=True)
        shutil.copyfile(os.path.join(WORK, fname), os.path.join(CHK, fname))
        RENDERS.append("renders/check/" + fname)

    CENTRE = (0, 0, HEIGHT / 2)
    # poses
    go.hide_render = False
    show(PARTS)
    set_mats("tex")
    for view in ("front", "side"):
        for tag, src, fname in (("EXR", EXRV, "pose_run_%s.png" % view), ("KEYED", KEYED, "pose_run_keyed_%s.png" % view)):
            tiles = []
            for f in FRAMES:
                set_pos(src[f])
                tiles.append(shot(VIEWS[view], CENTRE, 150, "FRAME %d  %s  %s" % (f, tag, view.upper()), 0.055))
            save_sheet([tiles], fname)
    # isolated parts
    go.hide_render = True
    set_pos(REST_V)
    for kind in ("colour", "textured"):
        set_mats("colour" if kind == "colour" else "tex")
        for view in ("front", "side"):
            tiles = []
            for n in PARTS:
                show([n])
                V = REST_V[n]
                ctr = (V.min(0) + V.max(0)) / 2
                size = max(float((V.max(0) - V.min(0)).max()) * 1.45, 20.0)
                tiles.append(shot(VIEWS[view], tuple(ctr), size, "%s %s %dT" % (n, view.upper(), len(TRIS[n])), 0.07))
            save_sheet([tiles[:5], tiles[5:]], "parts_isolated_%s_%s.png" % (kind, view))
    # all ten parts together, own colours, whole body
    set_mats("colour")
    show(PARTS)
    go.hide_render = False
    for view in ("front", "side"):
        t1 = shot(VIEWS[view], CENTRE, 150, "ALL PARTS %s" % view.upper(), 0.055)
        save_sheet([[t1]], "parts_isolated_all_colour_%s.png" % view)
    # colour legend
    LEG = {n: [round(c, 3) for c in colorsys.hsv_to_rgb(i / len(PARTS), 0.75, 0.95)] for i, n in enumerate(PARTS)}

# ---------------------------------------------------------------- report
rep = dict(
    task="task-161-01-chaser-cut-pose-check",
    source=dict(fbx=os.path.basename(opt["--in-fbx"]), exr=os.path.basename(opt["--in-exr"]), blender=bpy.app.version_string,
                note="all positions in Blender space, cm (X forward, Y left, Z up), parts at their rest positions; planes and pivots from chaser_meshy_parts.report.json"),
    method=dict(components="connected components of triangles after merging vertices by position (1e-4 cm)",
                far_triangles="triangles outside the main (largest) component: distance of the centroid to the bbox of the main component and to its centroid",
                wrong_side="a triangle centroid (or vertex) inside the plane zone box on the side of the plane opposite to the part's main component by more than %.2f cm; hip planes only count triangles on the pelvis side of the waist plane (as the cut did)" % TOL,
                open_edges="triangle edges used by one triangle (merged positions)", joint_distance="vertex distance to the part's own pivot (its joint) and to the parent's pivot"),
    parts=CUT,
    stray_fragments=STRAY,
    plane_sides={k: v for k, v in PLANE_SIDE.items()},
    parts_report_open_edges={n: PREP["parts"][n]["open_edges"] for n in PARTS},
    pose_check=dict(
        frames=FRAMES, exr=dict(width=int(EW), height=int(EH), channel_layout="texel x=2*part quaternion xyzw, x=2*part+1 translation t cm; frame f = row f from the top of the file", load_vs_raw_parse_max_abs=exr_load_vs_raw),
        formula="line by line port of the Unreal Custom node: q renormalised, q=(-x,y,-z,w), tr=(x,-y,z), LocalPos=(x,-y,z) of the Blender vertex, new=LocalPos+offset, mirrored back; part index of each vertex read from the PartId UV (floor(uv*10)) and checked against the object",
        exr_vs_keyed_max_vertex_difference_cm_per_frame=[round(DIFF[f]["max_cm"], 5) for f in range(NRUN)],
        selected_frames={str(f): DIFF[f] for f in FRAMES},
        max_over_all_run_frames_cm=round(max(DIFF[f]["max_cm"] for f in range(NRUN)), 5),
        keyed_path="meshy_bake_anim.py functions: run_pose, apply_pose, fit_ground on the parent hierarchy, vertices = matrix_world @ local",
        images=RENDERS),
    anomalies=ANOM,
    plane_deviations=dict(note="deviation of the cut from the planar model: a triangle centroid / vertex of a part on the other side of the joint plane than its main component. Not an assignment error by itself when the cut followed mesh edges (zigzag cut, mode 'edges' of meshy_cut_parts.py): the depth is then the zigzag amplitude", list=PLDEV),
    warnings=WARN)
if not NORENDER:
    rep["part_colours_rgb"] = LEG
    rep["render_setup"] = "EEVEE, orthographic, lit rig of meshy_bake_anim.py (sun 4.5, world grey 0.62 strength 0.55), %d px tiles; poses: 150 cm frame centred at z=%.1f, front camera at +X (robot left = image right), side camera at -Y (robot faces image right); isolated parts: frame = 1.45 x the largest bbox extent of the part (>= 20 cm), tile label = part, view, triangles" % (PX, HEIGHT / 2)
os.makedirs(OUT, exist_ok=True)
rp = os.path.join(WORK, "cut_check.report.json")
with open(rp, "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
shutil.copyfile(rp, os.path.join(OUT, "cut_check.report.json"))
print("DONE anomalies=%d" % len(ANOM))
for a in ANOM:
    print(" -", a)
print("exr_vs_keyed_max_cm", [round(DIFF[f]["max_cm"], 4) for f in range(NRUN)])
