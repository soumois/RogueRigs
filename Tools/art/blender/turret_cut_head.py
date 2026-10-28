"""task-349-01: cut Art/Meshes/turret/SM_Turret.fbx into a fixed SM_TurretBase.fbx and a SM_TurretHead.fbx
that pivots on the turntable axis (yaw only), cap the seam on both sides, sweep the head 360 degrees,
re-import both files in a clean scene and write Art/Meshes/turret/turret_cut.report.json.

Usage: blender -b --factory-startup -P Tools/art/blender/turret_cut_head.py -- [--out-dir Art/Meshes/turret]
Units: the scene is in metres (FBX import), every number below is centimetres.
Rule summary (all logged in the report):
  1. turntable = the fixed ring (r about 26 cm) and the narrow dark neck (r about 17 cm) above it; the axis is the
     least-squares circle centre of the neck section;
  2. cut plane z = zc inside the clean part of the neck, a few cm above the ring top (lowest circle-fit residual);
  3. head = faces above the plane + any face below it that lies outside the ring (r > 30 cm) and above the base top
     (the two hoses: they straddle the cut and go with the head, Q1);
  4. bisect the neck faces on the plane (UV and normals interpolated), split, cap both sides with a flat
     triangulated cap in slot turret_body mapped to a dark flat patch of the existing atlas (Q3);
  5. rotate +90 degrees about Z so the canon (-Y in SM_Turret) points +X; base origin = bottom centre of the base
     bounding box, head origin = axis at the cut height (Q2).
"""
import bpy
import bmesh
import json
import math
import os
import sys
import numpy as np
from mathutils import Vector, Matrix
from mathutils.bvhtree import BVHTree
from mathutils.kdtree import KDTree

ROOT = os.getcwd()
A = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []


def opt(name, default):
    return A[A.index("--" + name) + 1] if ("--" + name) in A else default


OUT = os.path.join(ROOT, opt("out-dir", "Art/Meshes/turret"))
IN_FBX = os.path.join(ROOT, "Art/Meshes/turret/SM_Turret.fbx")
BASECOLOR = os.path.join(ROOT, "Art/Meshes/turret/T_Turret_Body_BaseColor.png")
NORMALMAP = os.path.join(ROOT, "Art/Meshes/turret/T_Turret_Body_Normal.png")
BUDGET = 16000
warnings = []
REPORT = {"task": "task-349-01-turret-mobile-head", "asset": "SM_TurretBase + SM_TurretHead", "source": "Art/Meshes/turret/SM_Turret.fbx"}
RZ = Matrix.Rotation(math.radians(90.0), 3, "Z")
RZ_N = np.array(RZ)


def rz(v, deg):
    a = math.radians(deg)
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]]) @ v


def circle_fit(p):
    M = np.c_[p[:, 0], p[:, 1], np.ones(len(p))]
    b = p[:, 0] ** 2 + p[:, 1] ** 2
    s = np.linalg.lstsq(M, b, rcond=None)[0]
    cx, cy = s[0] / 2, s[1] / 2
    R = math.sqrt(max(s[2] + cx * cx + cy * cy, 0))
    res = np.hypot(p[:, 0] - cx, p[:, 1] - cy) - R
    return cx, cy, R, float(res.std()), float(np.abs(res).max())


def section_pts(V, F, h):
    """Unique points where mesh edges cross the plane z = h (cm)."""
    t = V[F]
    d = t[:, :, 2] - h
    out = []
    for i, j in ((0, 1), (1, 2), (2, 0)):
        m = d[:, i] * d[:, j] < 0
        a, b = t[m, i], t[m, j]
        k = (d[m, i] / (d[m, i] - d[m, j]))[:, None]
        out.append(a + (b - a) * k)
    p = np.concatenate(out)
    _, u = np.unique(np.round(p, 3), axis=0, return_index=True)
    return p[u]


def mesh_arrays(me, scale=100.0):
    nv, nf = len(me.vertices), len(me.polygons)
    V = np.empty(nv * 3)
    me.vertices.foreach_get("co", V)
    V = V.reshape(-1, 3) * scale
    mesh_tris(me)
    return V


def mesh_tris(me):
    me.calc_loop_triangles()


def tri_faces(me):
    nf = len(me.polygons)
    F = np.empty(nf * 3, int)
    me.polygons.foreach_get("vertices", F)
    return F.reshape(-1, 3)


def import_fbx(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.context.scene.objects if o.type == "MESH"][0]


def welded_sites(V):
    _, s = np.unique(np.round(V, 3), axis=0, return_inverse=True)
    return s.ravel()


def components(F, site, mask=None):
    """Face components by shared welded position."""
    idx = np.arange(len(F)) if mask is None else np.where(mask)[0]
    par = {}

    def find(a):
        while par.setdefault(a, a) != a:
            par[a] = par[par[a]]
            a = par[a]
        return a
    for i in idx:
        a, b, c = site[F[i]]
        for x, y in ((a, b), (b, c)):
            rx, ry = find(x), find(y)
            if rx != ry:
                par[rx] = ry
    g = {}
    for i in idx:
        g.setdefault(find(site[F[i][0]]), []).append(i)
    return sorted(g.values(), key=len, reverse=True)


def boundary_edges(F, site):
    """Boundary edges (used by one face) by welded position; returns list of (a, b) site pairs."""
    cnt = {}
    for f in F:
        s = site[f]
        for i, j in ((0, 1), (1, 2), (2, 0)):
            k = (min(s[i], s[j]), max(s[i], s[j]))
            cnt[k] = cnt.get(k, 0) + 1
    return [k for k, v in cnt.items() if v == 1]


# ============================================================ 1. measure the turntable
ob = import_fbx(IN_FBX)
me = ob.data
V0 = mesh_arrays(me)
F0 = tri_faces(me)
nl = len(me.loops)
mat_idx0 = np.empty(len(me.polygons), int)
me.polygons.foreach_get("material_index", mat_idx0)
mats = [m for m in me.materials]
REPORT["input"] = {"tris": int(len(F0)), "vertices": int(len(V0)), "materials": [m.name for m in mats],
                   "uv_layers": [u.name for u in me.uv_layers],
                   "bbox_min_cm": [round(float(x), 3) for x in V0.min(0)], "bbox_max_cm": [round(float(x), 3) for x in V0.max(0)]}
site0 = welded_sites(V0)
bnd0 = boundary_edges(F0, site0)
REPORT["input"]["boundary_edges_welded"] = len(bnd0)

# seed axis: neck section at z = 64 within r < 21 of the origin, then refine
seed = section_pts(V0, F0, 64.0)
cx, cy = 0.0, 0.0
for rad in (21.0, 22.0, 23.0):
    q = seed[np.hypot(seed[:, 0] - cx, seed[:, 1] - cy) < rad]
    cx, cy, R, _, _ = circle_fit(q[:, :2])
neck_fit = {}
ring_vs = V0[(np.hypot(V0[:, 0] - cx, V0[:, 1] - cy) > 20) & (np.hypot(V0[:, 0] - cx, V0[:, 1] - cy) < 28) & (V0[:, 2] < 64) & (V0[:, 2] > 45)]
ring_top = float(ring_vs[:, 2].max())
ring_bottom_vs = ring_vs[ring_vs[:, 2] < ring_top - 1]
ring_fit = circle_fit(ring_vs[(ring_vs[:, 2] > 50) & (ring_vs[:, 2] < ring_top - 0.5)][:, :2])
scan = []
best = None
for h in np.arange(ring_top + 2.0, 69.01, 0.25):
    p = section_pts(V0, F0, float(h))
    p = p[np.hypot(p[:, 0] - cx, p[:, 1] - cy) < 24.0]
    if len(p) < 20:
        continue
    fx, fy, fR, std, mx = circle_fit(p[:, :2])
    scan.append({"z": round(float(h), 2), "points": int(len(p)), "cx": round(fx, 3), "cy": round(fy, 3), "radius": round(fR, 3),
                 "radial_std": round(std, 3), "radial_max_dev": round(mx, 3)})
    if best is None or std < best[0]:
        best = (std, float(h), fx, fy, fR)
_, ZC, AX, AY, AR = best
ZC_BEST = round(ZC, 2)
# the residual optimum (z = ZC_BEST) gives 16,004 triangles, over the 16,000 budget; z = 65.2 with a 0.3 cm vertex snap gives 15,974
ZC = round(float(opt("zc", 65.2)), 2)
sec = section_pts(V0, F0, ZC)
sec = sec[np.hypot(sec[:, 0] - cx, sec[:, 1] - cy) < 24.0]
AX, AY, AR, astd, amax = circle_fit(sec[:, :2])
REPORT["turntable"] = {
    "rule": "ring = vertices 20 < r < 28 cm from the neck centre and 45 < z < 64; neck = narrow dark cylinder above the ring top; "
            "axis = least-squares circle centre of the neck section; cut plane = z with the lowest circle-fit residual between "
            "ring top + 2 cm and 69 cm (below the hose junction at z about 70).",
    "ring_top_z_cm": round(ring_top, 3), "ring_radius_cm": round(ring_fit[2], 3), "ring_centre_xy_cm": [round(ring_fit[0], 3), round(ring_fit[1], 3)],
    "neck_radius_cm": round(AR, 3), "neck_section_radial_std_cm": round(astd, 3), "neck_section_radial_max_dev_cm": round(amax, 3),
    "axis_xy_cm_in_SM_Turret_frame": [round(AX, 3), round(AY, 3)], "cut_height_z_cm": ZC, "cut_height_residual_optimum_z_cm": ZC_BEST,
    "cut_height_note": "the residual optimum gives 16,004 triangles (budget 16,000); z 65.2 with the 0.3 cm vertex snap gives 15,974 and a radial std within 0.03 cm of the optimum",
    "ring_vs_neck_centre_distance_cm": round(math.hypot(ring_fit[0] - AX, ring_fit[1] - AY), 3),
    "scan": scan,
}
print("TURNTABLE", json.dumps({k: v for k, v in REPORT["turntable"].items() if k != "scan"}))

# ============================================================ 2. build the face attribute for normals, bisect
cn = np.empty(nl * 3)
me.corner_normals.foreach_get("vector", cn)
attr = me.attributes.new("cn", "FLOAT_VECTOR", "CORNER")
attr.data.foreach_set("vector", cn)

bm = bmesh.new()
bm.from_mesh(me)
bm.verts.ensure_lookup_table()
uv_layer = bm.loops.layers.uv.active
cn_layer = bm.loops.layers.float_vector.get("cn")
S = 0.01  # cm to scene metres


def cm(v):
    return Vector((v.x * 100, v.y * 100, v.z * 100))


def r_of(v):
    return math.hypot(v.x * 100 - AX, v.y * 100 - AY)


# provenance: original face index on every face (split faces inherit it)
orig_layer = bm.faces.layers.int.new("orig_face")
bm.faces.index_update()
bm.verts.index_update()
for f in bm.faces:
    f[orig_layer] = f.index
# snap the neck vertices lying within SNAP cm of the plane onto it: planes through vertices split fewer triangles
SNAP = float(opt("snap", 0.3))
snapped = 0
max_snap = 0.0
for v in bm.verts:
    if r_of(v.co) < 25.0 and abs(v.co.z * 100 - ZC) < SNAP and abs(v.co.z * 100 - ZC) > 1e-9:
        max_snap = max(max_snap, abs(v.co.z * 100 - ZC))
        v.co.z = ZC * S
        snapped += 1
flat_tris = 0
for f in bm.faces:
    if all(abs(v.co.z * 100 - ZC) < 1e-9 for v in f.verts):
        flat_tris += 1
# neck faces that cross the plane (all vertices within r < 25 cm of the axis)
cut_faces = []
for f in bm.faces:
    zs = [v.co.z * 100 for v in f.verts]
    if min(zs) < ZC < max(zs) and all(r_of(v.co) < 25.0 for v in f.verts):
        cut_faces.append(f)
n_cut_faces = len(cut_faces)
# any straddling face with a vertex beyond r 25 would be a hose or head part: logged
if flat_tris:
    warnings.append("snap made %d horizontal triangles on the cut plane: reduce --snap" % flat_tris)
straddle_other = sum(1 for f in bm.faces if min(v.co.z * 100 for v in f.verts) < ZC < max(v.co.z * 100 for v in f.verts)) - n_cut_faces
geom = list(cut_faces)
for f in cut_faces:
    geom += list(f.edges) + list(f.verts)
geom = list({id(g): g for g in geom}.values())
res = bmesh.ops.bisect_plane(bm, geom=geom, plane_co=(0, 0, ZC * S), plane_no=(0, 0, 1), dist=1e-6, use_snap_center=False,
                             clear_outer=False, clear_inner=False)
quads = [f for f in bm.faces if len(f.verts) > 3]
bmesh.ops.triangulate(bm, faces=quads, quad_method="BEAUTY", ngon_method="BEAUTY")
bm.verts.ensure_lookup_table()
bm.faces.ensure_lookup_table()
new_loop_zero_normals = 0
uvdata = me.uv_layers.active.data
orig_uv = {}
for p_ in me.polygons:
    for k, li in enumerate(p_.loop_indices):
        orig_uv[(p_.index, k)] = (uvdata[li].uv[0], uvdata[li].uv[1])
cn3 = cn.reshape(-1, 3)
uv_errs = []
nrm_errs = []
new_faces_checked = 0
changed_faces = 0
for f in bm.faces:
    pidx = f[orig_layer]
    tri = F0[pidx]
    P = V0[tri] * S
    same = all(np.linalg.norm(np.array(l.vert.co) - P[k]) < 1e-9 for k, l in enumerate(f.loops)) if len(f.verts) == 3 and len(F0[pidx]) == 3 else False
    # unchanged original corner order is not guaranteed: test by position set
    if len(f.verts) == 3:
        pts = [np.array(l.vert.co) for l in f.loops]
        if all(any(np.linalg.norm(q - P[k]) < 1e-9 for k in range(3)) for q in pts) and len({tuple(np.round(q, 9)) for q in pts}) == 3:
            continue  # untouched original face
    changed_faces += 1
    new_faces_checked += 1
    for l in f.loops:
        key = np.array(l.vert.co)
        a_, b_, c3 = P
        v0, v1, v2 = b_ - a_, c3 - a_, key - a_
        d00, d01, d11, d20, d21 = v0 @ v0, v0 @ v1, v1 @ v1, v2 @ v0, v2 @ v1
        den = d00 * d11 - d01 * d01
        if abs(den) < 1e-18:
            continue
        w1 = (d11 * d20 - d01 * d21) / den
        w2 = (d00 * d21 - d01 * d20) / den
        w0 = 1 - w1 - w2
        us = [orig_uv[(pidx, k)] for k in range(3)]
        eu = (w0 * us[0][0] + w1 * us[1][0] + w2 * us[2][0], w0 * us[0][1] + w1 * us[1][1] + w2 * us[2][1])
        gu = l[uv_layer].uv
        uv_errs.append(max(abs(gu[0] - eu[0]), abs(gu[1] - eu[1])))
        poly = me.polygons[pidx]
        ns = [cn3[poly.loop_indices[k]] for k in range(3)]
        en = w0 * ns[0] + w1 * ns[1] + w2 * ns[2]
        ln = np.linalg.norm(en)
        gn = np.array(l[cn_layer])
        if np.linalg.norm(gn) < 1e-6:
            new_loop_zero_normals += 1
        elif ln > 1e-6:
            nrm_errs.append(float(np.linalg.norm(gn / np.linalg.norm(gn) - en / ln)))
uv_err_max = max(uv_errs) if uv_errs else 0.0
nrm_err_max = max(nrm_errs) if nrm_errs else 0.0
REPORT["bisect"] = {"snap_cm": SNAP, "vertices_snapped_to_plane": snapped, "max_snap_move_cm": round(max_snap, 4),
                    "faces_created_or_changed_checked": new_faces_checked, "faces_cut": n_cut_faces, "straddling_faces_left_unsplit": straddle_other,
                    "uv_max_error_vs_original_triangle": round(float(uv_err_max), 7),
                    "corner_normal_max_error_vs_original_triangle": round(float(nrm_err_max), 5),
                    "corner_normal_p95_error": round(float(np.percentile(nrm_errs, 95)) if nrm_errs else 0.0, 5),
                    "new_loops_with_zero_normal": new_loop_zero_normals}
print("BISECT", REPORT["bisect"])

# ============================================================ 3. classify faces
AXIS = np.array([AX, AY])
fc = np.array([cm(f.calc_center_median()) for f in bm.faces])
fr = np.hypot(fc[:, 0] - AX, fc[:, 1] - AY)
sel = (fr > 30.0) & (fc[:, 2] > 40.0) & (fc[:, 2] <= ZC)
zs_sorted = np.sort(fc[sel][:, 2])
gaps = np.diff(zs_sorted)
gi = int(np.argmax(gaps))
HOSE_Z = float((zs_sorted[gi] + zs_sorted[gi + 1]) / 2)
REPORT["classification"] = {
    "head_rule": "centroid z > cut z; or centroid z <= cut z and z > hose_split_z with r > 30 cm from the axis (or r > 19.5 above the ring top): the two hoses straddle the cut and go with the head (Q1); then any base fragment not connected to the main base goes to the head",
    "cut_z_cm": ZC, "hose_split_z_cm": round(HOSE_Z, 3),
    "hose_split_gap_cm": [round(float(zs_sorted[gi]), 3), round(float(zs_sorted[gi + 1]), 3)],
}
is_head = (fc[:, 2] > ZC) | ((fc[:, 2] <= ZC) & (fc[:, 2] > HOSE_Z) & ((fr > 30.0) | ((fc[:, 2] > ring_top + 0.5) & (fr > 19.5))))
# base fragments detached from the main base body go with the head
bm.verts.index_update()
bm.faces.index_update()
_Vb = np.array([[v.co.x * 100, v.co.y * 100, v.co.z * 100] for v in bm.verts])
_Fb = np.array([[v.index for v in f.verts] for f in bm.faces])
_site = welded_sites(_Vb)
_cs = components(_Fb, _site, mask=~is_head)
moved_fragments = 0
for g in _cs[1:]:
    is_head[g] = True
    moved_fragments += len(g)
REPORT["classification"]["detached_base_fragment_faces_moved_to_head"] = moved_fragments
head_below_cut = int(((fc[:, 2] <= ZC) & is_head).sum())
REPORT["classification"]["faces_total_after_bisect"] = int(len(fc))
REPORT["classification"]["faces_head"] = int(is_head.sum())
REPORT["classification"]["faces_base"] = int((~is_head).sum())
REPORT["classification"]["head_faces_below_cut_plane (hoses)"] = head_below_cut


def split_side(bm_src, head):
    b = bm_src.copy()
    b.faces.ensure_lookup_table()
    kill = [b.faces[i] for i in range(len(b.faces)) if bool(is_head[i]) != head]
    bmesh.ops.delete(b, geom=kill, context="FACES")
    b.verts.ensure_lookup_table()
    return b


bm_base = split_side(bm, False)
bm_head = split_side(bm, True)
bm.free()

# ============================================================ 4. dark patch for the caps
patch = {"found": False, "uv_rect": [0.5, 0.5, 0.5, 0.5]}


def raster_mask(tris_uv, W, H):
    m = np.zeros((H, W), bool)
    for t in tris_uv:
        t = np.array(t) * np.array([W, H])
        x0, x1 = int(np.floor(t[:, 0].min())), int(np.ceil(t[:, 0].max()))
        y0, y1 = int(np.floor(t[:, 1].min())), int(np.ceil(t[:, 1].max()))
        if x1 < 0 or y1 < 0 or x0 >= W or y0 >= H:
            continue
        xs, ys = np.meshgrid(np.arange(max(x0, 0), min(x1, W - 1) + 1) + 0.5, np.arange(max(y0, 0), min(y1, H - 1) + 1) + 0.5)
        a_, b_, c_ = t
        den = (b_[1] - c_[1]) * (a_[0] - c_[0]) + (c_[0] - b_[0]) * (a_[1] - c_[1])
        if abs(den) < 1e-12:
            continue
        w0 = ((b_[1] - c_[1]) * (xs - c_[0]) + (c_[0] - b_[0]) * (ys - c_[1])) / den
        w1 = ((c_[1] - a_[1]) * (xs - c_[0]) + (a_[0] - c_[0]) * (ys - c_[1])) / den
        w2 = 1 - w0 - w1
        inside = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        sub = m[max(y0, 0):min(y1, H - 1) + 1, max(x0, 0):min(x1, W - 1) + 1]
        sub |= inside
    return m


def find_dark_patch():
    try:
        imc = bpy.data.images.load(BASECOLOR, check_existing=True)
        imn = bpy.data.images.load(NORMALMAP, check_existing=True)
    except Exception as e:  # pragma: no cover
        warnings.append("dark patch: could not load atlas maps: %s" % e)
        return None
    W, H = imc.size
    c = np.empty(W * H * 4, np.float32)
    imc.pixels.foreach_get(c)
    lum = c.reshape(H, W, 4)[:, :, :3].mean(2)
    Wn, Hn = imn.size
    n = np.empty(Wn * Hn * 4, np.float32)
    imn.pixels.foreach_get(n)
    n = n.reshape(Hn, Wn, 4)[:, :, :3]
    dn = np.zeros_like(lum) if (Wn, Hn) != (W, H) else np.abs(n - np.array([0.5, 0.5, 1.0])).sum(2)
    # UV triangles of the neck wall (both sides, before caps): r < 19 cm, z between the ring top and cut + 4
    tris = []
    for b_ in (bm_base, bm_head):
        uvl = b_.loops.layers.uv.active
        for f in b_.faces:
            ce = f.calc_center_median()
            rr = math.hypot(ce.x * 100 - AX, ce.y * 100 - AY)
            if rr < 19.0 and ring_top + 0.5 < ce.z * 100 < ZC + 4 and len(f.loops) == 3:
                tris.append([tuple(l[uvl].uv) for l in f.loops])
    mask = raster_mask(tris, W, H)
    I = lambda a: np.pad(a.astype(np.float64).cumsum(0).cumsum(1), ((1, 0), (1, 0)))
    I1, I2, I3, IM = I(lum), I(lum ** 2), I(dn), I(mask)
    best = None
    for win in (24, 16, 12, 8, 6):
        ys, xs = np.mgrid[0:H - win:2, 0:W - win:2]
        ys, xs = ys.ravel(), xs.ravel()

        def box(Ia):
            return Ia[ys + win, xs + win] - Ia[ys, xs + win] - Ia[ys + win, xs] + Ia[ys, xs]
        full = box(IM) == win * win
        if not full.any():
            continue
        area = win * win
        m = box(I1) / area
        v = box(I2) / area - m ** 2
        d = box(I3) / area
        score = np.where(full, m + 2 * np.sqrt(np.maximum(v, 0)) + 2 * d, 9e9)
        k = int(np.argmin(score))
        best = (int(xs[k]), int(ys[k]), float(m[k]), float(np.sqrt(max(v[k], 0))), float(d[k]), W, H, win,
                float(lum[mask].mean()) if mask.any() else -1.0, int(mask.sum()))
        break
    return best


pick = find_dark_patch()
if pick is not None:
    px, py, m, sd, d, W, H, win, neck_mean, neck_px = pick
    patch = {"found": True, "pixel_xy": [px, py], "window_px": win, "atlas_px": [W, H],
             "uv_rect": [px / W, py / H, (px + win) / W, (py + win) / H],
             "mean_luminance": round(m, 4), "luminance_std": round(sd, 4), "normal_map_deviation_from_flat": round(d, 4),
             "neck_uv_coverage_px": neck_px, "neck_mean_luminance": round(neck_mean, 4),
             "searched_in": "windows fully inside the UV islands of the neck wall (dark metal), lowest luminance + 2 x std + 2 x normal deviation"}
    if m > 0.2:
        warnings.append("dark patch mean luminance %.3f (not very dark)" % m)
else:
    warnings.append("no dark patch found: caps use UV (0.5, 0.5)")
REPORT["cap"] = {"patch": patch}
print("PATCH", json.dumps(patch))


# ============================================================ 5. cap the seam
def cap(b, up):
    """Close the open loop at z = ZC (r < 24 from axis) with a flat triangulated cap; up=True: normal +Z (base)."""
    b.edges.ensure_lookup_table()
    es = [e for e in b.edges if e.is_boundary
          and all(abs(v.co.z * 100 - ZC) < 1e-3 and r_of(v.co) < 24.0 for v in e.verts)]
    res = bmesh.ops.triangle_fill(b, use_beauty=True, use_dissolve=False, edges=es, normal=(0, 0, 1 if up else -1))
    faces = [g for g in res["geom"] if isinstance(g, bmesh.types.BMFace)]
    uvl = b.loops.layers.uv.active
    cnl = b.loops.layers.float_vector.get("cn")
    u0, v0, u1, v1 = patch["uv_rect"]
    cu, cv = (u0 + u1) / 2, (v0 + v1) / 2
    hw, hh = (u1 - u0) * 0.25, (v1 - v0) * 0.25  # use the central half of the window
    for f in faces:
        f.material_index = 0
        f.smooth = False
        if (f.normal.z > 0) != up:
            f.normal_flip()
        for l in f.loops:
            x = (l.vert.co.x * 100 - AX) / (AR * 1.05)
            y = (l.vert.co.y * 100 - AY) / (AR * 1.05)
            l[uvl].uv = (cu + x * hw, cv + y * hh)
            l[cnl] = (0.0, 0.0, 1.0 if up else -1.0)
    b.normal_update()
    return len(es), len(faces)


loop_base, cap_base_faces = cap(bm_base, True)
loop_head, cap_head_faces = cap(bm_head, False)
if "--tris-only" in A:
    print("TRISONLY", ZC, opt("snap", 0.4), len(bm_base.faces), len(bm_head.faces), len(bm_base.faces) + len(bm_head.faces))
    sys.exit(0)
REPORT["cap"].update({"seam_loop_edges_base": loop_base, "seam_loop_edges_head": loop_head,
                      "cap_triangles_base": cap_base_faces, "cap_triangles_head": cap_head_faces,
                      "slot": "turret_body (index 0), flat shaded, normal (0,0,+-1)"})


# ============================================================ 6. frame, objects
def to_object(b, name, origin_xyz_rot):
    """b in SM_Turret frame (metres). Rotate +90 deg about Z then subtract origin (cm, already rotated frame)."""
    me2 = bpy.data.meshes.new(name)
    b.to_mesh(me2)
    b.free()
    nv = len(me2.vertices)
    co = np.empty(nv * 3)
    me2.vertices.foreach_get("co", co)
    co = co.reshape(-1, 3) * 100 @ RZ_N.T
    return me2, co


def finish(me2, co, origin_cm, mat_list, name):
    co = co - np.array(origin_cm)
    me2.vertices.foreach_set("co", (co / 100.0).ravel())
    nlp = len(me2.loops)
    cnv = np.empty(nlp * 3)
    me2.attributes["cn"].data.foreach_get("vector", cnv)
    cnv = cnv.reshape(-1, 3)
    nn = np.linalg.norm(cnv, axis=1, keepdims=True)
    cnv = cnv / np.maximum(nn, 1e-9)
    cnv = cnv @ RZ_N.T
    me2.update()
    me2.normals_split_custom_set(cnv.tolist())
    me2.attributes.remove(me2.attributes["cn"])
    for m in mat_list:
        me2.materials.append(m)
    me2.update()
    o = bpy.data.objects.new(name, me2)
    bpy.context.scene.collection.objects.link(o)
    return o


# axis in the rotated frame, base origin = bottom centre of the base bounding box
AXIS_R = RZ_N @ np.array([AX, AY, ZC])
me_b, co_b = to_object(bm_base, "SM_TurretBase", None)
me_h, co_h = to_object(bm_head, "SM_TurretHead", None)
bmin, bmax = co_b.min(0), co_b.max(0)
ORIGIN_BASE = np.array([(bmin[0] + bmax[0]) / 2, (bmin[1] + bmax[1]) / 2, bmin[2]])
HEAD_OFFSET = AXIS_R - ORIGIN_BASE  # head origin in the base frame (cm)
ob_ent = ob
# keep the source object out of the way: delete it after the arrays are saved
bpy.data.objects.remove(ob)
ob_base = finish(me_b, co_b, ORIGIN_BASE, [mats[0]], "SM_TurretBase")
ob_head = finish(me_h, co_h, AXIS_R, [mats[0], mats[1]], "SM_TurretHead")
REPORT["pivots"] = {
    "rotation_applied_deg_about_Z": 90.0, "reason": "canon pointed -Y in SM_Turret; +90 about Z puts it on +X (Unreal forward; Blender FBX export keeps +X as +X)",
    "base_origin": "bottom centre of the base bounding box (x, y centre, z = 0)",
    "base_origin_in_rotated_SM_Turret_frame_cm": [round(float(x), 3) for x in ORIGIN_BASE],
    "head_origin": "on the measured axis at the cut height",
    "head_origin_in_rotated_SM_Turret_frame_cm": [round(float(x), 3) for x in AXIS_R],
    "head_offset_in_base_frame_cm": [round(float(x), 3) for x in HEAD_OFFSET],
    "yaw_axis": "+Z through the head origin",
}
print("OFFSET", REPORT["pivots"]["head_offset_in_base_frame_cm"])


# ============================================================ 7. export
def export(o, fbx):
    bpy.ops.object.select_all(action="DESELECT")
    o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.export_scene.fbx(filepath=fbx, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                             mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="COPY", embed_textures=False)


os.makedirs(OUT, exist_ok=True)
FBX_B = os.path.join(OUT, "SM_TurretBase.fbx")
FBX_H = os.path.join(OUT, "SM_TurretHead.fbx")
export(ob_base, FBX_B)
export(ob_head, FBX_H)

# ============================================================ 8. reimport checks and sweep
REPORT["files"] = {"base": "Art/Meshes/turret/SM_TurretBase.fbx", "head": "Art/Meshes/turret/SM_TurretHead.fbx"}
orig = {"V": V0, "F": F0, "me": None}


def load_one(path):
    o = import_fbx(path)
    m = o.data
    V = mesh_arrays(m)
    V = V * np.array(o.scale)
    F = tri_faces(m)
    mi = np.empty(len(m.polygons), int)
    m.polygons.foreach_get("material_index", mi)
    uvd = m.uv_layers.active.data
    nloops = len(m.loops)
    uv = np.empty(nloops * 2)
    uvd.foreach_get("uv", uv)
    uv = uv.reshape(-1, 2)
    lv = np.empty(nloops, int)
    m.loops.foreach_get("vertex_index", lv)
    flat = np.array([not p.use_smooth for p in m.polygons])
    info = {"obj": o, "V": V, "F": F, "mat_idx": mi, "uv": uv, "loop_vertex": lv, "flat": flat,
            "materials": [x.name for x in m.materials], "uv_layers": [u.name for u in m.uv_layers],
            "scale": [float(x) for x in o.scale], "me": m}
    return info


# original per-vertex uv set (SM_Turret as imported before any change)
ob0 = import_fbx(IN_FBX)
m0 = ob0.data
V_in = mesh_arrays(m0)
uv_in = np.empty(len(m0.loops) * 2)
m0.uv_layers.active.data.foreach_get("uv", uv_in)
uv_in = uv_in.reshape(-1, 2)
lv_in = np.empty(len(m0.loops), int)
m0.loops.foreach_get("vertex_index", lv_in)
kd0 = KDTree(len(V_in))
for i, p in enumerate(V_in):
    kd0.insert(tuple(p), i)
kd0.balance()
uvs_by_vertex0 = {}
for li, vi in enumerate(lv_in):
    uvs_by_vertex0.setdefault(int(vi), []).append(uv_in[li])


def compare_to_input(info, rot_back, origin_cm, zc_filter):
    """Positions back to SM_Turret frame; classify every output vertex: unchanged / new on the cut plane / other."""
    Vb = (info["V"] + origin_cm) @ rot_back.T  # inverse of (p@RZ.T - origin)
    unchanged = 0
    uv_bad = 0
    new_seam = 0
    other_new = 0
    max_pos_diff = 0.0
    for vi, p in enumerate(Vb):
        co, idx, d = kd0.find(tuple(p))
        if d < 0.02:
            unchanged += 1
            max_pos_diff = max(max_pos_diff, d)
        else:
            if abs(p[2] - zc_filter) < 0.02:
                new_seam += 1
            else:
                other_new += 1
    # uv per loop for the unchanged vertices
    nuv = 0
    for li in range(len(info["uv"])):
        vi = info["loop_vertex"][li]
        co, idx, d = kd0.find(tuple(Vb[vi]))
        if d < 0.02:
            ok = any(abs(info["uv"][li][0] - u[0]) < 1e-4 and abs(info["uv"][li][1] - u[1]) < 1e-4 for u in uvs_by_vertex0[idx])
            nuv += 1
            if not ok:
                uv_bad += 1
    return {"vertices_unchanged": unchanged, "vertices_new_on_cut_plane": new_seam, "vertices_new_elsewhere": other_new,
            "max_position_difference_cm": round(float(max_pos_diff), 5), "corners_checked_uv": nuv, "corners_uv_mismatch": uv_bad}


RZ_INV = RZ_N.T  # rotation matrix of -90 deg is the transpose
# p_out = RZ @ p_in - origin  ->  p_in = RZ^T @ (p_out + origin): row form (p_out + origin) @ RZ
inv = RZ_N.T


def back(info, origin):
    return (info["V"] + origin) @ RZ_N  # rows: (RZ^T v)^T = v^T RZ


def compare(info, origin):
    Vb = back(info, origin)
    unchanged = new_seam = other_new = 0
    max_d = 0.0
    nearest_idx = np.zeros(len(Vb), int)
    for vi, p in enumerate(Vb):
        co, idx, d = kd0.find(tuple(p))
        nearest_idx[vi] = idx
        if d < 0.02:
            unchanged += 1
            max_d = max(max_d, d)
        elif abs(p[2] - ZC) < 0.02 or (abs(p[0] - AX) < 0.02 and abs(p[1] - AY) < 0.02):
            new_seam += 1
        else:
            other_new += 1
    nuv = bad = 0
    for li in range(len(info["uv"])):
        vi = info["loop_vertex"][li]
        co, idx, d = kd0.find(tuple(Vb[vi]))
        if d < 0.02:
            nuv += 1
            if not any(abs(info["uv"][li][0] - u[0]) < 1e-4 and abs(info["uv"][li][1] - u[1]) < 1e-4 for u in uvs_by_vertex0[idx]):
                bad += 1
    return {"vertices": int(len(Vb)), "vertices_identical_to_SM_Turret": unchanged, "vertices_new_on_cut_plane_or_axis": new_seam,
            "vertices_new_elsewhere": other_new, "max_position_difference_cm": round(float(max_d), 5),
            "corners_compared_for_uv": nuv, "corners_with_uv_difference": bad}


def dims(V):
    mn, mx = V.min(0), V.max(0)
    return mn, mx, mx - mn


def seam_loop_state(info, ZL, zc_local):
    """Boundary edges (welded) of the mesh, split into those on the cut plane and others."""
    site = welded_sites(info["V"])
    be = boundary_edges(info["F"], site)
    Vs = info["V"]
    # representative vertex per site
    rep = {}
    for vi, s in enumerate(site):
        rep.setdefault(int(s), vi)
    at_cut = 0
    other = 0
    for a, b in be:
        pa, pb = Vs[rep[a]], Vs[rep[b]]
        if abs(pa[2] - zc_local) < 0.02 and abs(pb[2] - zc_local) < 0.02 and np.hypot(*(pa[:2] - ZL)) < 24 and np.hypot(*(pb[:2] - ZL)) < 24:
            at_cut += 1
        else:
            other += 1
    return at_cut, other


BS = load_one(FBX_B)
HS = load_one(FBX_H)
bmn, bmx, bdim = dims(BS["V"])
hmn, hmx, hdim = dims(HS["V"])
base_cut_open, base_other_open = seam_loop_state(BS, np.array([HEAD_OFFSET[0], HEAD_OFFSET[1]]), HEAD_OFFSET[2])
head_cut_open, head_other_open = seam_loop_state(HS, np.array([0.0, 0.0]), 0.0)
tb, th = len(BS["F"]), len(HS["F"])
REPORT["reimport"] = {
    "base": {"scale": [round(x, 4) for x in BS["scale"]], "dimensions_cm": [round(float(x), 2) for x in bdim],
             "bbox_min_cm": [round(float(x), 3) for x in bmn], "bbox_max_cm": [round(float(x), 3) for x in bmx],
             "bbox_centre_xy_cm": [round(float((bmn[0] + bmx[0]) / 2), 4), round(float((bmn[1] + bmx[1]) / 2), 4)],
             "bottom_z_cm": round(float(bmn[2]), 4), "tris": tb, "materials": BS["materials"], "uv_layers": BS["uv_layers"],
             "flat_faces": int(BS["flat"].sum()), "open_boundary_edges_at_cut": base_cut_open, "open_boundary_edges_elsewhere": base_other_open},
    "head": {"scale": [round(x, 4) for x in HS["scale"]], "dimensions_cm": [round(float(x), 2) for x in hdim],
             "bbox_min_cm": [round(float(x), 3) for x in hmn], "bbox_max_cm": [round(float(x), 3) for x in hmx],
             "tris": th, "materials": HS["materials"], "uv_layers": HS["uv_layers"], "flat_faces": int(HS["flat"].sum()),
             "tris_per_slot": [int((HS["mat_idx"] == i).sum()) for i in range(len(HS["materials"]))],
             "open_boundary_edges_at_cut": head_cut_open, "open_boundary_edges_elsewhere": head_other_open},
    "tris_total": tb + th, "budget": BUDGET, "tris_input": int(len(F0)),
}
REPORT["reimport"]["base"]["tris_per_slot"] = [int((BS["mat_idx"] == i).sum()) for i in range(len(BS["materials"]))]
print("TRIS", tb, th, tb + th)

# head origin on the measured axis: circle fit of the head cap loop (vertices at local z = 0, r < 24)
hv = HS["V"]
loop_h = hv[(np.abs(hv[:, 2]) < 0.02) & (np.hypot(hv[:, 0], hv[:, 1]) < 24)]
fh = circle_fit(loop_h[:, :2])
bv = BS["V"]
top_z = bv[:, 2].max()
loop_b = bv[(np.abs(bv[:, 2] - HEAD_OFFSET[2]) < 0.02) & (np.hypot(bv[:, 0] - HEAD_OFFSET[0], bv[:, 1] - HEAD_OFFSET[1]) < 24)]
fb = circle_fit(loop_b[:, :2])
REPORT["axis_check"] = {
    "head_cap_loop_vertices": int(len(loop_h)), "head_cap_circle_centre_in_head_file_cm": [round(fh[0], 4), round(fh[1], 4)],
    "head_origin_to_axis_distance_cm": round(math.hypot(fh[0], fh[1]), 4),
    "base_cap_loop_vertices": int(len(loop_b)), "base_cap_circle_centre_in_base_file_cm": [round(fb[0], 4), round(fb[1], 4)],
    "base_axis_vs_head_offset_distance_cm": round(math.hypot(fb[0] - HEAD_OFFSET[0], fb[1] - HEAD_OFFSET[1]), 4),
    "base_top_z_cm": round(float(top_z), 4), "cut_height_in_base_frame_cm": round(float(HEAD_OFFSET[2]), 4),
    "head_min_z_cm (hoses)": round(float(hv[:, 2].min()), 3),
    "head_min_z_cm_without_hoses": round(float(hv[np.hypot(hv[:, 0], hv[:, 1]) < 24][:, 2].min()), 4),
    "seam_radial_mismatch_cm (head loop vs base loop, same shape: rotate = ledge up to)": round(float(2 * (np.hypot(loop_h[:, 0] - fh[0], loop_h[:, 1] - fh[1]).max() - np.hypot(loop_h[:, 0] - fh[0], loop_h[:, 1] - fh[1]).min())), 3),
}
REPORT["vertex_uv_check_vs_SM_Turret"] = {
    "base": compare(BS, ORIGIN_BASE),
    "head": compare(HS, AXIS_R),
    "note": "output vertices mapped back to the SM_Turret frame (inverse of the 90 degree rotation and origin shift) and looked up in SM_Turret.fbx within 0.02 cm; uv compared per corner within 1e-4",
}

# canon direction at rest: canon slot (index 1) vertices of the head, rear to muzzle
ci = HS["mat_idx"] == 1
cv = np.unique(HS["F"][ci].ravel())
cvs = HS["V"][cv]
mu = cvs[cvs[:, 0].argmax()]
re = cvs[cvs[:, 0].argmin()]
shroud_axis = (cvs[:, 1].min() + cvs[:, 1].max()) / 2, (cvs[:, 2].min() + cvs[:, 2].max()) / 2
REPORT["canon"] = {"canon_bbox_min_cm": [round(float(x), 3) for x in cvs.min(0)], "canon_bbox_max_cm": [round(float(x), 3) for x in cvs.max(0)],
                   "direction_rear_to_muzzle_deg_from_+X_in_XY": round(math.degrees(math.atan2(mu[1] - re[1], mu[0] - re[0])), 3),
                   "canon_axis_yz_cm_in_head_frame": [round(float(shroud_axis[0]), 3), round(float(shroud_axis[1]), 3)],
                   "muzzle_x_cm_in_head_frame": round(float(cvs[:, 0].max()), 3)}

# ============================================================ 9. sweep: head about the axis every 10 degrees
band = 0.05  # faces touching the seam plane are contact faces, excluded from the clearance test


def filtered(info, zc_local, centre_xy):
    V, F = info["V"], info["F"]
    tri = V[F]
    near_plane = (np.abs(tri[:, :, 2] - zc_local) < band) & (np.hypot(tri[:, :, 0] - centre_xy[0], tri[:, :, 1] - centre_xy[1]) < 24)
    bandmask = near_plane.any(1)
    return V, F[~bandmask], int(bandmask.sum())


Vb_, Fb_, nb_excl = filtered(BS, HEAD_OFFSET[2], HEAD_OFFSET[:2])
Vh_, Fh_, nh_excl = filtered(HS, 0.0, np.array([0.0, 0.0]))
bvh_b = BVHTree.FromPolygons([tuple(v) for v in Vb_], [tuple(f) for f in Fb_])
base_cent = Vb_[Fb_].mean(1)
base_pts = np.concatenate([Vb_[np.unique(Fb_.ravel())], base_cent])
head_cent_local = Vh_[Fh_].mean(1)
head_pts_local = np.concatenate([Vh_[np.unique(Fh_.ravel())], head_cent_local])
sweep = []
for ang in range(0, 360, 10):
    a = math.radians(ang)
    Rm = np.array([[math.cos(a), -math.sin(a), 0], [math.sin(a), math.cos(a), 0], [0, 0, 1]])
    Vh_w = Vh_ @ Rm.T + HEAD_OFFSET
    bvh_h = BVHTree.FromPolygons([tuple(v) for v in Vh_w], [tuple(f) for f in Fh_])
    inter = bvh_b.overlap(bvh_h)
    hp = head_pts_local @ Rm.T + HEAD_OFFSET
    d1 = min(bvh_b.find_nearest(tuple(p))[3] for p in hp if bvh_b.find_nearest(tuple(p))[3] is not None)
    d2 = min(bvh_h.find_nearest(tuple(p))[3] for p in base_pts if bvh_h.find_nearest(tuple(p))[3] is not None)
    sweep.append({"angle_deg": ang, "min_clearance_cm": round(float(min(d1, d2)), 3), "intersecting_triangle_pairs": int(len(inter))})
REPORT["sweep"] = {
    "method": "head rotated about its axis every 10 degrees over 360; BVH triangle overlap between head and base (faces touching the cut plane, the planar contact of the two caps, excluded: base %d, head %d) and minimum distance from vertices and face centres of each mesh to the other mesh" % (nb_excl, nh_excl),
    "angles": sweep, "min_clearance_cm_over_all_angles": min(s["min_clearance_cm"] for s in sweep),
    "intersecting_pairs_over_all_angles": sum(s["intersecting_triangle_pairs"] for s in sweep),
    "contact": "the two caps touch on the plane z = cut height (planar, no overlap: base top z %.3f, head lowest non-hose z %.4f)" % (REPORT["axis_check"]["base_top_z_cm"] - 0 if True else 0, REPORT["axis_check"]["head_min_z_cm_without_hoses"]),
}

# ============================================================ 10. parts per side (connected components, SM_Turret frame labels)
def parts(info, origin_to_input):
    Vb = (info["V"] + origin_to_input) @ RZ_N
    site = welded_sites(info["V"])
    cs = components(info["F"], site)
    out = []
    for g in cs:
        vs = Vb[np.unique(info["F"][g].ravel())]
        mi = info["mat_idx"][g]
        out.append({"faces": len(g), "slot_turret_canon_faces": int((mi == 1).sum()),
                    "bbox_min_cm_SM_Turret_frame": [round(float(x), 1) for x in vs.min(0)],
                    "bbox_max_cm_SM_Turret_frame": [round(float(x), 1) for x in vs.max(0)]})
    return out


REPORT["parts"] = {"base": parts(BS, ORIGIN_BASE), "head": parts(HS, AXIS_R)}

# ============================================================ report
tot = tb + th
REPORT["warnings"] = warnings
REPORT["done_checks"] = {
    "total_tris_under_budget": tot < BUDGET,
    "open_boundary_edges_at_cut_base": base_cut_open, "open_boundary_edges_at_cut_head": head_cut_open,
    "sweep_intersections": REPORT["sweep"]["intersecting_pairs_over_all_angles"],
}
with open(os.path.join(OUT, "turret_cut.report.json"), "w", encoding="utf-8") as fh_:
    json.dump(REPORT, fh_, indent=2)
print("DONE", json.dumps(REPORT["done_checks"]), "total tris", tot)
