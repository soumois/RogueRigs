"""task-367-01: cut Art/Meshes/extractor/SM_Extractor.fbx into a fixed SM_Extractor_Body.fbx and four rotor meshes
SM_Extractor_Rotor_FL|FR|RL|RR.fbx (blades plus hub) that spin about their own vertical axis; cap every cut on both sides;
sweep each rotor 360 degrees against the body; reimport the five files in clean scenes; write extractor_rotors.report.json.

Usage: blender -b --factory-startup -P Tools/art/blender/extractor_cut_rotors.py -- [--out-dir Art/Meshes/extractor] [--scan-only]
Units: the scene is in metres (FBX import), every number in the report is centimetres. +X nose, +Y left, +Z up.
Selection rule per rotor (all logged in the report):
  1. spin axis = vertical line through the centre of the sphere fitted to the hub dome (vertices within 8 cm of the rough
     centre, above the neck plane); the rotor origin is that axis at the blade plane height (area-weighted mean z of the
     blade faces, r > 10 cm);
  2. neck plane z = zc: the horizontal plane through the hub underside, above the stator layer below the blades;
  3. blade cylinder radius Rc (scan 22.0 to 24.5 cm, step 0.5): the blade tips are fused to the thin dark duct lining (r about 24 cm), so
     the rotor = faces whose centroid is inside the cylinder of radius Rc around the axis and above the neck plane, edge-connected
     to the hub top; the cut at the tips follows the triangle edges (no split, no new vertex); Rc = the value of the scan with the
     fewest rotor-body intersections in a 36-angle sweep (ties: the largest Rc below the first jump of the rotor face count);
  4. bisect the hub faces on the neck plane (UV interpolated); everything not in the rotor stays on the body (ducts, arms, motors);
  5. cap both sides of every cut with centre-fan triangles in the same material slot (UV on a dark flat texel of the rotor
     faces), flat shaded; rotor origin on the axis at the blade plane, body origin unchanged (bottom centre of SM_Extractor).
"""
import bpy
import bmesh
import json
import math
import os
import sys
import numpy as np
from mathutils import Vector
from mathutils.bvhtree import BVHTree
from mathutils.kdtree import KDTree

sys.path.insert(0, os.path.join(os.getcwd(), "Tools", "art", "blender"))
import rr_blender as rr  # noqa: E402

ROOT = os.getcwd()
A = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []


def opt(name, default):
    return A[A.index("--" + name) + 1] if ("--" + name) in A else default


OUT = os.path.join(ROOT, opt("out-dir", "Art/Meshes/extractor"))
RENDERS = os.path.join(ROOT, "Saved", "ExtractorMesh", "rotors")
IN_FBX = os.path.join(ROOT, "Art/Meshes/extractor/SM_Extractor.fbx")
BASECOLOR = os.path.join(ROOT, "Art/Meshes/extractor/T_Extractor_BaseColor.png")
BUDGET = 50000
S = 0.01  # cm to scene metres
RC_SCAN = [22.0, 22.5, 23.0, 23.5, 24.0, 24.5]
MIN_CLEAR = 0.2
# rough hub centres (task-366 hand-over, refined by the sphere fit) and neck plane heights read on the z profiles
ROUGH = {"FL": (33.6, 66.3, 57.5), "FR": (33.7, -66.7, 57.5), "RL": (-64.85, 62.1, 71.0), "RR": (-64.9, -62.4, 71.0)}
SCAN_ONLY = "--scan-only" in A
warnings = []
REPORT = {"task": "task-367-01-extractor-rotors-cut", "source": "Art/Meshes/extractor/SM_Extractor.fbx"}


# ------------------------------------------------------------------ helpers
def sphere_fit(p):
    M = np.c_[2 * p, np.ones(len(p))]
    b = (p ** 2).sum(1)
    s = np.linalg.lstsq(M, b, rcond=None)[0]
    c = s[:3]
    R = math.sqrt(max(s[3] + c @ c, 0))
    res = np.linalg.norm(p - c, axis=1) - R
    return c, R, float(res.std())


def circle_fit(p):
    M = np.c_[p[:, 0], p[:, 1], np.ones(len(p))]
    b = p[:, 0] ** 2 + p[:, 1] ** 2
    s = np.linalg.lstsq(M, b, rcond=None)[0]
    cx, cy = s[0] / 2, s[1] / 2
    R = math.sqrt(max(s[2] + cx * cx + cy * cy, 0))
    return cx, cy, R, float((np.hypot(p[:, 0] - cx, p[:, 1] - cy) - R).std())


def arrays(me):
    nv, nf = len(me.vertices), len(me.polygons)
    V = np.empty(nv * 3)
    me.vertices.foreach_get("co", V)
    F = np.empty(nf * 3, int)
    me.polygons.foreach_get("vertices", F)
    return V.reshape(-1, 3) * 100.0, F.reshape(-1, 3)


def bm_arrays(bm):
    bm.verts.index_update()
    bm.faces.index_update()
    V = np.array([v.co[:] for v in bm.verts]) * 100.0
    F = np.array([[v.index for v in f.verts] for f in bm.faces], dtype=int)
    return V, F


def welded_sites(V):
    _, s = np.unique(np.round(V, 3), axis=0, return_inverse=True)
    return s.ravel()


def boundary_edges(F, site):
    cnt = {}
    for f in F:
        s = site[f]
        for i, j in ((0, 1), (1, 2), (2, 0)):
            k = (min(s[i], s[j]), max(s[i], s[j]))
            cnt[k] = cnt.get(k, 0) + 1
    return [k for k, v in cnt.items() if v == 1]


# ------------------------------------------------------------------ cutting
def cut_bmesh(bm, ax, zc, rc):
    """Bisect bm (metres) on the neck plane z = zc, only the hub faces (all vertices within 10.5 cm of the axis). The blade tips are not
    cut: the rotor boundary there follows the triangle edges (no new vertex, no split)."""
    bm.verts.ensure_lookup_table()
    bm.faces.ensure_lookup_table()
    nv_hub = {v for v in bm.verts if math.hypot(v.co.x * 100 - ax[0], v.co.y * 100 - ax[1]) < 10.5 and v.co.z * 100 > zc - 12.0}
    nv_big = set()

    def run(fs, co, no, tracked):
        geom = list(fs)
        for f in fs:
            geom += list(f.edges) + list(f.verts)
        geom = list({id(g): g for g in geom}.values())
        res = bmesh.ops.bisect_plane(bm, geom=geom, plane_co=co, plane_no=no, dist=1e-7, use_snap_center=False,
                                     clear_outer=False, clear_inner=False)
        for g in res["geom"]:
            if isinstance(g, bmesh.types.BMVert) and g.is_valid:
                for t in tracked:
                    t.add(g)
        quads = list({f for v in res["geom"] if isinstance(v, bmesh.types.BMVert) and v.is_valid for f in v.link_faces if len(f.verts) > 3})
        if quads:
            bmesh.ops.triangulate(bm, faces=quads, quad_method="BEAUTY", ngon_method="BEAUTY")
        for t in tracked:
            for v in [v for v in t if not v.is_valid]:
                t.discard(v)

    fs = {f for v in nv_hub if v.is_valid for f in v.link_faces if all((w in nv_hub) for w in f.verts)}
    fs = [f for f in fs if min(w.co.z for w in f.verts) * 100 < zc < max(w.co.z for w in f.verts) * 100]
    run(fs, (0, 0, zc * S), (0, 0, 1), [nv_hub])


def classify(bm, ax, zc, rc, hub_top_xy_r=4.0):
    """Face labels: True for the rotor = faces whose centroid is above the neck plane and inside the cylinder of radius rc
    around the axis, edge-connected to the hub top. The tip cut follows the triangle edges (no new vertices, no split)."""
    V, F = bm_arrays(bm)
    T = V[F]
    C = T.mean(1)
    inside = (C[:, 2] > zc) & (np.hypot(C[:, 0] - ax[0], C[:, 1] - ax[1]) < rc) & (T[:, :, 2].min(1) >= zc - 6.0)
    near_axis = inside & (np.hypot(C[:, 0] - ax[0], C[:, 1] - ax[1]) < hub_top_xy_r)
    seed = int(np.argmax(np.where(near_axis, C[:, 2], -1e9)))
    bm.faces.ensure_lookup_table()
    sel = np.zeros(len(F), bool)
    stack = [bm.faces[seed]]
    sel[seed] = True
    while stack:
        f = stack.pop()
        for e in f.edges:
            for g in e.link_faces:
                if not sel[g.index] and inside[g.index]:
                    sel[g.index] = True
                    stack.append(g)
    return sel, inside, V, F


def clip_xy(P, ax, rc):
    """Radial projection onto the cylinder r = rc around the vertical axis of the points beyond it (cm, in place); returns how many moved."""
    d = np.hypot(P[:, 0] - ax[0], P[:, 1] - ax[1])
    m = d > rc
    P[m, 0] = ax[0] + (P[m, 0] - ax[0]) * rc / d[m]
    P[m, 1] = ax[1] + (P[m, 1] - ax[1]) * rc / d[m]
    return int(m.sum())


# ------------------------------------------------------------------ caps
def dark_texel(rotor_faces_uv_tris):
    """UV of the darkest flat texel among the rotor face centroids (blades are flat dark metal)."""
    try:
        img = bpy.data.images.load(BASECOLOR, check_existing=True)
    except Exception as e:  # pragma: no cover
        warnings.append("cap texel: atlas not loaded (%s), UV (0.5, 0.5)" % e)
        return (0.5, 0.5), {}
    W, H = img.size
    px = np.empty(W * H * 4, np.float32)
    img.pixels.foreach_get(px)
    px = px.reshape(H, W, 4)[:, :, :3]
    lum = px.mean(2)
    best = None
    for uv in rotor_faces_uv_tris:
        x, y = int(uv[0] * W), int(uv[1] * H)
        if 4 <= x < W - 4 and 4 <= y < H - 4:
            win = lum[y - 3:y + 4, x - 3:x + 4]
            sc = float(win.mean() + 2 * win.std())
            if best is None or sc < best[0]:
                best = (sc, uv, float(win.mean()), float(win.std()), x, y)
    uv = best[1]
    return (float(uv[0]), float(uv[1])), {"pixel_xy": [best[4], best[5]], "atlas_px": [W, H], "mean_luminance": round(best[2], 4),
                                          "luminance_std": round(best[3], 4), "rule": "darkest 7x7 window (mean + 2 std) at a rotor face centroid UV"}


def loops_of(edges):
    adj = {}
    for e in edges:
        for v in e.verts:
            adj.setdefault(v, []).append(e)
    seen = set()
    out = []
    for e in edges:
        if e in seen:
            continue
        comp = []
        st = [e]
        seen.add(e)
        while st:
            x = st.pop()
            comp.append(x)
            for v in x.verts:
                for y in adj[v]:
                    if y not in seen:
                        seen.add(y)
                        st.append(y)
        out.append(comp)
    return out


def cap_piece(b, uv_xy, label):
    """Close every open loop of bmesh b with a centre fan, UV on one texel, flat shaded. Returns the number of triangles added and a per-loop record (edges, triangles, planarity deviation)."""
    b.edges.ensure_lookup_table()
    uvl = b.loops.layers.uv.active
    bedges = [e for e in b.edges if e.is_boundary]
    info = []
    n_new = 0
    for comp in loops_of(bedges):
        vs = list({v for e in comp for v in e.verts})
        P = np.array([v.co[:] for v in vs]) * 100.0
        c = P.mean(0)
        u, sv, vt = np.linalg.svd(P - c)
        dev = float(np.abs((P - c) @ vt[2]).max())
        rec = {"edges": len(comp), "planarity_dev_cm": round(dev, 3), "centre_cm": [round(float(x), 2) for x in c]}
        # fan of triangles around a centre vertex, winding opposite to the neighbouring face of each boundary edge (no diagonal between loop
        # vertices: a diagonal would duplicate an existing edge where the blade is thin); works for planar rings and ragged tip loops alike
        rec["method"] = "centre fan"
        cv = b.verts.new(Vector((float(c[0]), float(c[1]), float(c[2]))) * S)
        faces = []
        for e in comp:
            f0 = e.link_faces[0]
            lp = [x for x in f0.loops if x.edge == e][0]
            faces.append(b.faces.new((lp.link_loop_next.vert, lp.vert, cv)))
        for f in faces:
            f.smooth = False
            f.material_index = 0
            for l in f.loops:
                l[uvl].uv = uv_xy
        rec["triangles"] = len(faces)
        n_new += len(faces)
        info.append(rec)
    left = sum(1 for e in b.edges if e.is_boundary)
    nonman = sum(1 for e in b.edges if len(e.link_faces) > 2)
    return n_new, info, left, nonman


# ------------------------------------------------------------------ sweep
def contact_masks(V, Fr, Fb):
    """Contact faces: faces of either side that have a vertex shared with the other side (the cut rim, the caps), world frame cm."""
    rv = np.unique(Fr.ravel())
    bv = np.unique(Fb.ravel())
    kd = KDTree(len(bv))
    for k, i in enumerate(bv):
        kd.insert(tuple(V[i]), k)
    kd.balance()
    shared_r = np.zeros(len(V), bool)
    shared_b = np.zeros(len(V), bool)
    for i in rv:
        co, k, d = kd.find(tuple(V[i]))
        if d < 0.005:
            shared_r[i] = True
            shared_b[bv[k]] = True
    return shared_r[Fr].any(1), shared_b[Fb].any(1)


def tri_overlap_depth(Vr, Fr, Vb, Fb, pairs):
    """Penetration estimate per overlapping pair: how far the rotor triangle pokes through the body triangle plane (smaller side)."""
    out = []
    for i, j in pairs:
        a, b, c = Vb[Fb[j]]
        n = np.cross(b - a, c - a)
        ln = np.linalg.norm(n)
        if ln < 1e-12:
            out.append(0.0)
            continue
        d = (Vr[Fr[i]] - a) @ (n / ln)
        out.append(float(min(d.max(), -d.min())) if d.max() > 0 > d.min() else 0.0)
    return out


def run_sweep(Vw, Fw_rot, Fw_body, ax, zc, rc, step=10):
    rot_c, body_c = contact_masks(Vw, Fw_rot, Fw_body)
    Fr = Fw_rot[~rot_c]
    Fb = Fw_body[~body_c]
    bvh_b = BVHTree.FromPolygons([tuple(v) for v in Vw], [tuple(f) for f in Fb])
    rv = np.unique(Fr.ravel())
    rc_pts = np.concatenate([Vw[rv], Vw[Fr].mean(1)])
    out = []
    bv = np.unique(Fb.ravel())
    near_b = bv[np.hypot(Vw[bv, 0] - ax[0], Vw[bv, 1] - ax[1]) < rc + 8.0]
    body_pts = np.concatenate([Vw[near_b], Vw[Fb].mean(1)[np.hypot(*(Vw[Fb].mean(1)[:, :2] - ax).T) < rc + 8.0]])
    for ang in range(0, 360, step):
        a = math.radians(ang)
        ca, sa = math.cos(a), math.sin(a)
        R = np.array([[ca, -sa, 0], [sa, ca, 0], [0, 0, 1]])
        Vrot = (Vw - np.array([ax[0], ax[1], 0])) @ R.T + np.array([ax[0], ax[1], 0])
        bvh_r = BVHTree.FromPolygons([tuple(v) for v in Vrot], [tuple(f) for f in Fr])
        pairs = bvh_b.overlap(bvh_r)
        pts = (rc_pts - np.array([ax[0], ax[1], 0])) @ R.T + np.array([ax[0], ax[1], 0])
        d1 = min((bvh_b.find_nearest(tuple(p))[3] for p in pts), default=99.0)
        d2 = min((bvh_r.find_nearest(tuple(p))[3] for p in body_pts), default=99.0)
        d1 = 99.0 if d1 is None else d1
        d2 = 99.0 if d2 is None else d2
        rec = {"angle_deg": ang, "min_clearance_cm": round(float(min(d1, d2)), 3), "intersecting_triangle_pairs": len(pairs)}
        if pairs:
            bi = [p[0] for p in pairs]
            ri = [p[1] for p in pairs]
            dep = tri_overlap_depth(Vrot, Fr, Vw, Fb, [(ri[k], bi[k]) for k in range(len(pairs))])
            k = int(np.argmax(dep))
            cen = Vrot[Fr[ri[k]]].mean(0)
            rec["max_penetration_estimate_cm"] = round(float(max(dep)), 3)
            rec["place_cm_body_frame"] = [round(float(x), 1) for x in cen]
            rec["place_radius_from_axis_cm"] = round(float(math.hypot(cen[0] - ax[0], cen[1] - ax[1])), 2)
            rec["place_height_cm"] = round(float(cen[2]), 2)
        out.append(rec)
    return out


# ------------------------------------------------------------------ 1. load, measure axes
objs = rr.import_fbx(IN_FBX)
ob0 = objs[0]
mat0 = ob0.data.materials[0]
me0 = ob0.data
V0, F0 = arrays(me0)
REPORT["input"] = {"tris": int(len(F0)), "vertices": int(len(V0)), "materials": [m.name for m in me0.materials],
                   "uv_layers": [u.name for u in me0.uv_layers], "bbox_min_cm": [round(float(x), 3) for x in V0.min(0)],
                   "bbox_max_cm": [round(float(x), 3) for x in V0.max(0)]}
AX = {}
rng = np.random.default_rng(1)


def hub_points(cx, cy, zc):
    """Dense surface samples of the faces of the hub dome: all vertices within 9 cm of (cx, cy) and above zc + 1.5."""
    T = V0[F0]
    m = (np.hypot(T[:, :, 0] - cx, T[:, :, 1] - cy).max(1) < 9.0) & (T[:, :, 2].min(1) > zc + 1.5)
    T = T[m]
    w = rng.random((len(T), 40, 2))
    w = np.where(w.sum(2, keepdims=True) > 1, 1 - w, w)
    return (T[:, None, 0] + (T[:, None, 1] - T[:, None, 0]) * w[:, :, 0:1] + (T[:, None, 2] - T[:, None, 0]) * w[:, :, 1:2]).reshape(-1, 3), int(m.sum())


for name, (rx, ry, zc) in ROUGH.items():
    cx, cy = rx, ry
    for it in range(3):
        pts, nf_ = hub_points(cx, cy, zc)
        for trim in range(3):  # trimmed fit: drop the 15 % worst points (blade roots and flanges)
            c, Rh, sd = sphere_fit(pts)
            res = np.abs(np.linalg.norm(pts - c, axis=1) - Rh)
            pts = pts[res <= np.quantile(res, 0.85)]
        cx, cy = float(c[0]), float(c[1])
    AX[name] = {"ax": (cx, cy), "zc": zc, "sphere_centre_cm": [round(float(x), 3) for x in c], "sphere_radius_cm": round(Rh, 3),
                "sphere_fit_radial_std_cm": round(sd, 3), "hub_faces_sampled": nf_, "hub_samples_kept": int(len(pts)),
                "rough_centre_cm": [rx, ry]}
    print("AXIS", name, AX[name])
REPORT["axes"] = {k: {kk: vv for kk, vv in v.items() if kk != "ax"} for k, v in AX.items()}
# duct lining circle (for information): vertices of the lining ring around the blade plane
for name, a in AX.items():
    ax = a["ax"]
    d = np.hypot(V0[:, 0] - ax[0], V0[:, 1] - ax[1])
    zlo = a["zc"] + 2
    ring = V0[(d > 23.5) & (d < 26.5) & (V0[:, 2] > zlo) & (V0[:, 2] < zlo + 8)]
    if len(ring) > 20:
        cf = circle_fit(ring[:, :2])
        a["lining_circle_centre_cm"] = [round(cf[0], 2), round(cf[1], 2)]
        a["lining_circle_radius_cm"] = round(cf[2], 2)
        a["lining_centre_to_axis_cm"] = round(math.hypot(cf[0] - ax[0], cf[1] - ax[1]), 2)
        REPORT["axes"][name].update({k: a[k] for k in ("lining_circle_centre_cm", "lining_circle_radius_cm", "lining_centre_to_axis_cm")})

# ------------------------------------------------------------------ 2. scan Rc per rotor
src_bm = bmesh.new()
src_bm.from_mesh(me0)
scan = {}
CHOSEN = {}
for name, a in AX.items():
    ax, zc = a["ax"], a["zc"]
    scan[name] = []
    for rc in RC_SCAN:
        b = src_bm.copy()
        cut_bmesh(b, ax, zc, rc)
        sel, inside, V, F = classify(b, ax, zc, rc)
        Fr, Fb = F[sel], F[~sel]
        Vr_ = V.copy()
        moved = clip_xy(Vr_, ax, rc)
        sw = run_sweep(np.concatenate([V, Vr_]), Fr + len(V), Fb, ax, zc, rc)
        inter = sum(s["intersecting_triangle_pairs"] for s in sw)
        mc = min(s["min_clearance_cm"] for s in sw)
        rv = Vr_[np.unique(Fr.ravel())]
        rec = {"rc_cm": rc, "rotor_faces_after_cut": int(sel.sum()), "faces_total_after_cut": int(len(F)), "sweep_intersecting_pairs_total": int(inter),
               "sweep_min_clearance_cm": mc, "vertices_clipped": moved, "rotor_max_radius_cm": round(float(np.hypot(rv[:, 0] - ax[0], rv[:, 1] - ax[1]).max()), 2),
               "rotor_z_range_cm": [round(float(rv[:, 2].min()), 2), round(float(rv[:, 2].max()), 2)]}
        scan[name].append(rec)
        print("SCAN", name, rec)
        b.free()
    sc = scan[name]
    # plateau: rc values before the first jump (> 8 %) of the rotor face count (a jump = the duct lining is picked up)
    plateau = [sc[0]]
    for prev, cur in zip(sc, sc[1:]):
        if cur["rotor_faces_after_cut"] > prev["rotor_faces_after_cut"] * 1.08:
            break
        plateau.append(cur)
    best = min(plateau, key=lambda r: (r["sweep_intersecting_pairs_total"], -r["rc_cm"]))
    CHOSEN[name] = best["rc_cm"]
    for r in sc:
        r["on_plateau"] = r in plateau
    if best["sweep_intersecting_pairs_total"]:
        warnings.append("%s: every Rc on the plateau (%s) shows rotor-body intersections in the sweep; Rc %.1f kept (fewest: %d pairs over 36 angles)" % (
            name, ",".join(str(r["rc_cm"]) for r in plateau), best["rc_cm"], best["sweep_intersecting_pairs_total"]))
REPORT["scan"] = scan
REPORT["chosen_rc_cm"] = CHOSEN
print("CHOSEN", CHOSEN)
if SCAN_ONLY:
    sys.exit(0)

# ------------------------------------------------------------------ 3. cut all four on one mesh
bm = src_bm
for name, a in AX.items():
    cut_bmesh(bm, a["ax"], a["zc"], CHOSEN[name])
V, F = bm_arrays(bm)
label = np.zeros(len(F), int)
names = list(AX)
for k, name in enumerate(names, 1):
    sel, inside, _, _ = classify(bm, AX[name]["ax"], AX[name]["zc"], CHOSEN[name])
    if (label[sel] != 0).any():
        warnings.append("%s: rotor selection overlaps another" % name)
    label[sel] = k
REPORT["cut"] = {"faces_after_cuts": int(len(F)), "faces_added_by_cuts": int(len(F) - len(F0)),
                 "rotor_faces": {n: int((label == k).sum()) for k, n in enumerate(names, 1)}}

# dark texel for the caps: among rotor faces
uvl = bm.loops.layers.uv.active
bm.faces.ensure_lookup_table()
cent_uv = []
for i in np.where(label > 0)[0]:
    f = bm.faces[int(i)]
    u = sum(l[uvl].uv[0] for l in f.loops) / 3
    v = sum(l[uvl].uv[1] for l in f.loops) / 3
    cent_uv.append((u, v))
UVC, uvinfo = dark_texel(cent_uv)
REPORT["cap"] = {"uv": [round(UVC[0], 5), round(UVC[1], 5)], "texel": uvinfo, "slot": "M_Extractor (index 0), flat shaded"}


def piece(lab, tag):
    p = bm.copy()
    p.faces.ensure_lookup_table()
    kill = [p.faces[i] for i in range(len(p.faces)) if label[i] != lab]
    bmesh.ops.delete(p, geom=kill, context="FACES")
    # drop loose vertices
    loose = [v for v in p.verts if not v.link_faces]
    if loose:
        bmesh.ops.delete(p, geom=loose, context="VERTS")
    if lab > 0:
        # the rotor is clipped to its cylinder: vertices beyond Rc are projected radially onto it (the body keeps the original vertices)
        ax_ = AX[tag]["ax"]
        P = np.array([v.co[:] for v in p.verts]) * 100.0
        moved = clip_xy(P, ax_, CHOSEN[tag])
        for v, q in zip(p.verts, P):
            v.co = Vector((float(q[0]), float(q[1]), float(q[2]))) * S
        REPORT["cut"].setdefault("tip_vertices_clipped_to_cylinder", {})[tag] = moved
    return p


pieces = {}
for lab, tag in [(0, "Body")] + [(k, n) for k, n in enumerate(names, 1)]:
    p = piece(lab, tag)
    n_new, info, left, nonman = cap_piece(p, UVC, tag)
    pieces[tag] = p
    REPORT["cap"][tag] = {"cap_triangles": n_new, "loops": info, "open_boundary_edges_left_after_caps": left, "edges_with_more_than_2_faces": nonman}
    print("CAP", tag, n_new, len(info), "loops", "left", left, "nonmanifold", nonman)

# ------------------------------------------------------------------ 4. objects, origins
objects = {}
ORIGIN = {}
# rotor origin: axis at the blade plane (area-weighted mean z of faces with r > 10 cm)
for name in names:
    p = pieces[name]
    ax = AX[name]["ax"]
    num = den = 0.0
    for f in p.faces:
        if f.material_index == 0 and not any(abs(v.co.z * 100 - AX[name]["zc"]) < 1e-3 for v in f.verts):
            c = f.calc_center_median()
            if math.hypot(c.x * 100 - ax[0], c.y * 100 - ax[1]) > 10.0:
                ar = f.calc_area()
                num += ar * c.z * 100
                den += ar
    zp = num / den if den else AX[name]["zc"] + 4
    ORIGIN[name] = (ax[0], ax[1], zp)
    bmesh.ops.translate(p, vec=Vector((-ax[0] * S, -ax[1] * S, -zp * S)), verts=list(p.verts))
    REPORT["axes"][name]["blade_plane_z_cm"] = round(zp, 3)
    REPORT["axes"][name]["rotor_origin_in_body_frame_cm"] = [round(ax[0], 3), round(ax[1], 3), round(zp, 3)]
for tag, p in pieces.items():
    me = bpy.data.meshes.new("SM_Extractor_" + ("Body" if tag == "Body" else "Rotor_" + tag))
    p.to_mesh(me)
    p.free()
    me.materials.append(mat0)
    me.update()
    o = bpy.data.objects.new(me.name, me)
    bpy.context.scene.collection.objects.link(o)
    objects[tag] = o
bpy.data.objects.remove(ob0)
os.makedirs(OUT, exist_ok=True)
FILES = {tag: os.path.join(OUT, o.name + ".fbx") for tag, o in objects.items()}
for tag, o in objects.items():
    rr.export_fbx(FILES[tag], [o])
REPORT["files"] = {t: os.path.relpath(p, ROOT).replace("\\", "/") for t, p in FILES.items()}

# ------------------------------------------------------------------ 5. reimport checks
kd0 = KDTree(len(V0))
for i, p_ in enumerate(V0):
    kd0.insert(tuple(p_), i)
kd0.balance()
ob_in = rr.import_fbx(IN_FBX)[0]
uv_in = np.empty(len(ob_in.data.loops) * 2)
ob_in.data.uv_layers.active.data.foreach_get("uv", uv_in)
uv_in = uv_in.reshape(-1, 2)
lv_in = np.empty(len(ob_in.data.loops), int)
ob_in.data.loops.foreach_get("vertex_index", lv_in)
uvs_by_v = {}
for li, vi in enumerate(lv_in):
    uvs_by_v.setdefault(int(vi), []).append(uv_in[li])

re = {}
for tag in ["Body"] + names:
    o = rr.import_fbx(FILES[tag])[0]
    m = o.data
    Vt, Ft = arrays(m)
    nl = len(m.loops)
    uv = np.empty(nl * 2)
    m.uv_layers.active.data.foreach_get("uv", uv)
    uv = uv.reshape(-1, 2)
    lv = np.empty(nl, int)
    m.loops.foreach_get("vertex_index", lv)
    mi = np.empty(len(m.polygons), int)
    m.polygons.foreach_get("material_index", mi)
    flat = int(sum(1 for p_ in m.polygons if not p_.use_smooth))
    site = welded_sites(Vt)
    be = boundary_edges(Ft, site)
    re[tag] = {"V": Vt, "F": Ft, "uv": uv, "lv": lv, "mats": [x.name for x in m.materials], "uvl": [u.name for u in m.uv_layers],
               "scale": [float(x) for x in o.scale], "boundary": len(be), "flat": flat, "obj_loc": [float(x) for x in o.location], "mi": mi}
tot = sum(len(r["F"]) for r in re.values())
off = {n: np.array(ORIGIN[n]) for n in names}
REPORT["reimport"] = {}
for tag, r in re.items():
    Vt = r["V"]
    mn, mx = Vt.min(0), Vt.max(0)
    d = {"scale": [round(x, 4) for x in r["scale"]], "bbox_min_cm": [round(float(x), 3) for x in mn], "bbox_max_cm": [round(float(x), 3) for x in mx],
         "dimensions_cm": [round(float(x), 2) for x in (mx - mn)], "tris": int(len(r["F"])), "vertices": int(len(Vt)), "materials": r["mats"],
         "uv_layers": r["uvl"], "flat_faces": r["flat"], "boundary_edges_total": r["boundary"], "slots_used": sorted(set(int(x) for x in r["mi"]))}
    if tag == "Body":
        d["origin_check"] = {"bbox_centre_xy_cm": [round(float((mn[0] + mx[0]) / 2), 4), round(float((mn[1] + mx[1]) / 2), 4)], "bottom_z_cm": round(float(mn[2]), 4),
                             "source_bbox_centre_xy_cm": [round(float((V0.min(0)[0] + V0.max(0)[0]) / 2), 4), round(float((V0.min(0)[1] + V0.max(0)[1]) / 2), 4)],
                             "source_bottom_z_cm": round(float(V0.min(0)[2]), 4)}
    else:
        zc_local = AX[tag]["zc"] - ORIGIN[tag][2]
        T = Vt[r["F"]]
        m = (np.hypot(T[:, :, 0], T[:, :, 1]).max(1) < 9.0) & (T[:, :, 2].min(1) > zc_local + 1.5)
        T = T[m]
        w = rng.random((len(T), 40, 2))
        w = np.where(w.sum(2, keepdims=True) > 1, 1 - w, w)
        pts = (T[:, None, 0] + (T[:, None, 1] - T[:, None, 0]) * w[:, :, 0:1] + (T[:, None, 2] - T[:, None, 0]) * w[:, :, 1:2]).reshape(-1, 3)
        for trim in range(3):
            c, Rh, sd = sphere_fit(pts)
            res = np.abs(np.linalg.norm(pts - c, axis=1) - Rh)
            pts = pts[res <= np.quantile(res, 0.85)]
        rr_max = float(np.hypot(Vt[:, 0], Vt[:, 1]).max())
        d["origin_check"] = {"hub_sphere_centre_in_file_cm": [round(float(x), 3) for x in c], "origin_to_refitted_axis_cm": round(float(math.hypot(c[0], c[1])), 4),
                             "estimator": "same as the measurement: trimmed sphere fit on dense samples of the hub dome faces",
                             "max_radius_from_origin_cm": round(rr_max, 2), "chosen_rc_cm": CHOSEN[tag]}
    REPORT["reimport"][tag] = d
REPORT["tris_total"] = int(tot)
REPORT["budget"] = BUDGET
REPORT["tris_input"] = int(len(F0))
print("TRIS", {t: len(r["F"]) for t, r in re.items()}, tot)

# vertex and uv comparison against SM_Extractor (rotors moved back to the body frame)
cmp = {}
all_new_ok = True
for tag, r in re.items():
    Vb = r["V"] + (off[tag] if tag != "Body" else 0.0)
    unchanged = new_on_cut = other_new = 0
    maxd = 0.0
    nearest = np.zeros(len(Vb), int)
    isunch = np.zeros(len(Vb), bool)
    cap_uv = np.array(UVC)
    lv_uv_cap = np.zeros(len(Vb), bool)
    only_cap = np.ones(len(Vb), bool)
    iscap_corner = (np.abs(r["uv"] - cap_uv) < 2e-5).all(1)
    for li in range(len(r["uv"])):
        if not iscap_corner[li]:
            only_cap[r["lv"][li]] = False
    clipped = 0
    fan_centres = 0
    for i, p_ in enumerate(Vb):
        co, idx, dd = kd0.find(tuple(p_))
        nearest[i] = idx
        if dd < 0.02:
            unchanged += 1
            isunch[i] = True
            maxd = max(maxd, dd)
        else:
            on = False
            for n_, a in AX.items():
                axy = a["ax"]
                if abs(p_[2] - a["zc"]) < 0.02 and math.hypot(p_[0] - axy[0], p_[1] - axy[1]) < 10.6:
                    on = True
            if on:
                new_on_cut += 1
            elif only_cap[i]:
                fan_centres += 1
            elif tag != "Body" and abs(math.hypot(p_[0] - AX[tag]["ax"][0], p_[1] - AX[tag]["ax"][1]) - CHOSEN[tag]) < 0.02:
                clipped += 1
            else:
                other_new += 1
    bad = n = skipped = 0
    for li in range(len(r["uv"])):
        vi = r["lv"][li]
        if iscap_corner[li]:
            skipped += 1
        elif isunch[vi]:
            n += 1
            if not any(abs(r["uv"][li][0] - u[0]) < 1e-4 and abs(r["uv"][li][1] - u[1]) < 1e-4 for u in uvs_by_v[int(nearest[vi])]):
                bad += 1
    cmp[tag] = {"vertices": int(len(Vb)), "identical_to_SM_Extractor": int(unchanged), "new_on_neck_plane_cut": int(new_on_cut), "new_cap_fan_centres": int(fan_centres), "moved_to_clip_cylinder": int(clipped), "new_elsewhere": int(other_new),
                "max_position_difference_cm": round(float(maxd), 5), "corners_compared_for_uv": int(n), "cap_corners_skipped": int(skipped), "corners_with_uv_difference": int(bad)}
REPORT["vertex_uv_check_vs_SM_Extractor"] = {"per_file": cmp, "note": "vertices mapped back to the body frame (rotor offset added) and looked up in SM_Extractor.fbx within 0.02 cm; uv compared per corner within 1e-4; cap vertices carry the cap texel UV and are not compared"}

# boundary edges at the cuts: every file closed (welded boundary count); the neck cap and the tip caps are in the files
REPORT["boundary_edges_at_cuts"] = {t: r["boundary"] for t, r in re.items()}

# sweep on the reimported files, all four rotors against the body (+ the other three rotors left in place)
Vbody = re["Body"]["V"]
sweep_rep = {}
for name in names:
    ax = AX[name]["ax"]
    zc = AX[name]["zc"]
    rc = CHOSEN[name]
    Vr = re[name]["V"] + off[name]
    # world arrays: body + other rotors as the fixed side
    parts_V = [Vbody]
    parts_F = [re["Body"]["F"]]
    base = len(Vbody)
    for o_ in names:
        if o_ != name:
            parts_V.append(re[o_]["V"] + off[o_])
            parts_F.append(re[o_]["F"] + base)
            base += len(re[o_]["V"])
    Vw = np.concatenate(parts_V + [Vr])
    Fb = np.concatenate(parts_F)
    Fr = re[name]["F"] + base
    sw = run_sweep(Vw, Fr, Fb, ax, zc, rc)
    inter = sum(s["intersecting_triangle_pairs"] for s in sw)
    sweep_rep[name] = {"rc_cm": rc, "angles": sw, "min_clearance_cm_over_all_angles": min(s["min_clearance_cm"] for s in sw),
                       "intersecting_pairs_over_all_angles": inter,
                       "angles_with_collision": [s["angle_deg"] for s in sw if s["intersecting_triangle_pairs"]]}
    print("SWEEP", name, "pairs", inter, "min clearance", sweep_rep[name]["min_clearance_cm_over_all_angles"])
REPORT["sweep"] = {"method": "rotor rotated about its vertical axis every 10 degrees over 360 (36 angles); BVH triangle overlap between the rotor and the rest of the "
                             "assembly (body and the other rotors); faces with a vertex on a cut surface (neck plane or prism plane: the planar contact of the caps and their rim) "
                             "are excluded; minimum distance from vertices and face centres of each to the other; penetration = how far a rotor triangle pokes through the body triangle plane",
                   "per_rotor": sweep_rep}

# ------------------------------------------------------------------ 6. report
REPORT["warnings"] = warnings
REPORT["done_checks"] = {"total_tris_under_budget": bool(tot <= BUDGET), "total_tris": int(tot),
                         "boundary_edges": REPORT["boundary_edges_at_cuts"],
                         "sweep_intersections": {n: sweep_rep[n]["intersecting_pairs_over_all_angles"] for n in names}}
with open(os.path.join(OUT, "extractor_rotors.report.json"), "w", encoding="utf-8") as fh:
    json.dump(REPORT, fh, indent=2)
print("DONE", json.dumps(REPORT["done_checks"]))
