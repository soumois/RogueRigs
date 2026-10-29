"""Extractor (Rustwing quadrotor): new UV0 and four re-baked maps on the task-366 mesh (task-368-01), headless Blender 5.2.

Usage (repository root; the work dir holds the dense-phase output of extractor_flatten.py: dense_mesh.npz, dense.npz, decimated.fbx):
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/extractor_flatten.py -- --phase dense --work Saved/ExtractorMesh/rebake/work
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/extractor_rebake.py -- --phase uv   --work Saved/ExtractorMesh/rebake/work
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/extractor_rebake.py -- --phase bake --work Saved/ExtractorMesh/rebake/work
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/extractor_rebake.py -- --phase final --work Saved/ExtractorMesh/rebake/work
Options: --old-dir <dir with the task-366 SM_Extractor.fbx and maps>  --cone 60  --sharp 60  --margin-px 10  --extrusion-cm 1.0  --ray-cm 3.0

uv:    reads the task-366 SM_Extractor.fbx (geometry kept), seams = edges sharper than --sharp degrees + borders of the flat-shaded
       regions (components of at least --min-flat faces) + extra cuts (cone charts of --cone degrees, one boundary loop per chart),
       angle-based unwrap, uniform texel density, packed with --margin-px at 2048; writes work/uv.npz and the stretch numbers.
bake:  builds the dense mesh in the SM_Extractor frame (raw Meshy mesh, planar-region vertices projected flat as in task-366, rotation,
       scale and offset fitted on the unmoved vertices), bakes colour, metallic, roughness (EMIT of the Meshy maps through the source
       UVs), the tangent-space OpenGL normal (dense shape plus Meshy normal map) and a hit mask (missed rays), Cycles selected to active.
final: assembles the material, exports Art/Meshes/extractor/SM_Extractor.fbx, reimports it and writes extractor_rebake.report.json.
"""
import json
import math
import os
import sys
import time

import bpy
import numpy as np

sys.path.insert(0, os.path.join(os.getcwd(), "Tools", "art", "blender"))
import rr_blender as rr  # noqa: E402

ROOT = os.getcwd()
A = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []


def opt(name, default, cast=float):
    return cast(A[A.index("--" + name) + 1]) if ("--" + name) in A else default


PHASE = opt("phase", "uv", str)
WORK = os.path.abspath(opt("work", os.path.join(ROOT, "Saved", "ExtractorMesh", "rebake", "work"), str))
OLD = os.path.abspath(opt("old-dir", os.path.join(ROOT, "Saved", "ExtractorMesh", "rebake", "old"), str))
OUTDIR = os.path.join(ROOT, "Art", "Meshes", "extractor")
SRC_MAPS = os.path.join(ROOT, "Art", "Source", "extractor-meshy")
RAW = os.path.join(ROOT, "Art", "Meshes", "extractor", "raw", "Meshy_AI_Rustwing_Quadrotor_20261024_texture.fbx")
RES = 2048
CONE, SHARP, MIN_FLAT = opt("cone", 60.0), opt("sharp", 60.0), opt("min-flat", 8, int)
MERGE_MAX = opt("merge-max", 80.0)
MIN_CHART = opt("min-chart", 80, int)
MARGIN_PX = opt("margin-px", 8.5)
EXTR, RAYD, DILATE = opt("extrusion-cm", 0.5), opt("ray-cm", 3.0), 16
SAMPLES = opt("samples", 8, int)
MAPS = {"BaseColor": "Meshy_AI_Rustwing_Quadrotor_20261024_texture.png", "Metallic": "Meshy_AI_Rustwing_Quadrotor_20261024_texture_metallic.png",
        "Normal": "Meshy_AI_Rustwing_Quadrotor_20261024_texture_normal.png", "Roughness": "Meshy_AI_Rustwing_Quadrotor_20261024_texture_roughness.png"}
os.makedirs(WORK, exist_ok=True)
t0 = time.time()


def log(*a):
    print("[%6.1fs]" % (time.time() - t0), *a, flush=True)


def cm_verts(me):
    n = len(me.vertices)
    V = np.empty(n * 3); me.vertices.foreach_get("co", V)
    return V.reshape(-1, 3) * 100.0   # scene is in metres after the FBX import


def faces_of(me):
    F = np.empty(len(me.polygons) * 3, np.int64); me.polygons.foreach_get("vertices", F)
    return F.reshape(-1, 3)


def get_uv(me):
    uv = np.empty(len(me.loops) * 2); me.uv_layers[0].uv.foreach_get("vector", uv)
    return uv.reshape(-1, 2)


def tri_normals(V, F):
    a, b, c = V[F[:, 0]], V[F[:, 1]], V[F[:, 2]]
    cr = np.cross(b - a, c - a)
    ln = np.linalg.norm(cr, axis=1)
    return cr / np.maximum(ln[:, None], 1e-12), ln / 2


# ------------------------------------------------------------------------------------------------ metrics
def stretch_metrics(V, F, UV, per_face=False):
    """UV stretch per triangle. UV (3*nf, 2) in 0-1; J maps the 3D triangle plane (cm) to texels at RES.
    sigma1 >= sigma2 singular values. anisotropy = s1/s2; l2 = Sander L2 stretch sqrt((s1^2+s2^2)/2) normalised by the global
    texel density (area-weighted rms of s); density = geometric mean sqrt(s1*s2) over the global density. Triangle area weights."""
    nf = len(F)
    P = V[F]
    e1, e2 = P[:, 1] - P[:, 0], P[:, 2] - P[:, 0]
    L1 = np.linalg.norm(e1, axis=1)
    x = e1 / np.maximum(L1[:, None], 1e-12)
    nrm = np.cross(e1, e2); area = np.linalg.norm(nrm, axis=1) / 2
    nrm /= np.maximum(2 * area[:, None], 1e-12)
    y = np.cross(nrm, x)
    p1 = np.stack([L1, np.zeros(nf)], 1)
    p2 = np.stack([(e2 * x).sum(1), (e2 * y).sum(1)], 1)
    u = UV.reshape(nf, 3, 2) * RES
    u1, u2 = u[:, 1] - u[:, 0], u[:, 2] - u[:, 0]
    # J = [u1 u2] @ inv([p1 p2])
    det_p = p1[:, 0] * p2[:, 1] - p1[:, 1] * p2[:, 0]
    inv = np.zeros((nf, 2, 2))
    inv[:, 0, 0] = p2[:, 1]; inv[:, 0, 1] = -p2[:, 0]; inv[:, 1, 0] = -p1[:, 1]; inv[:, 1, 1] = p1[:, 0]
    inv /= np.where(np.abs(det_p) < 1e-12, 1e-12, det_p)[:, None, None]
    U = np.stack([u1, u2], axis=2)     # columns u1, u2
    Jm = U @ inv
    sv = np.linalg.svd(Jm, compute_uv=False)
    s1, s2 = sv[:, 0], sv[:, 1]
    detJ = np.linalg.det(Jm)
    uv_area = np.abs(u1[:, 0] * u2[:, 1] - u1[:, 1] * u2[:, 0]) / 2
    sg = math.sqrt(uv_area.sum() / area.sum())   # global texels per cm
    aniso = s1 / np.maximum(s2, 1e-9)
    aniso = np.minimum(aniso, 1000.0)
    l2 = np.sqrt((s1 ** 2 + s2 ** 2) / 2) / sg
    dens = np.sqrt(s1 * s2) / sg
    w = area / area.sum()

    def wp(x, q):
        o = np.argsort(x); c = np.cumsum(w[o]); return float(x[o][min(len(o) - 1, np.searchsorted(c, q / 100))])
    if per_face:
        return detJ < 0
    return {"global_texels_per_cm": round(sg, 4), "flipped_triangles": int((detJ < 0).sum()), "degenerate_uv_triangles": int((uv_area < 1e-9).sum()),
            "anisotropy_area_weighted_mean": round(float((aniso * w).sum()), 4), "anisotropy_p95": round(wp(aniso, 95), 3), "anisotropy_p99": round(wp(aniso, 99), 3),
            "anisotropy_max": round(float(aniso.max()), 2),
            "l2_stretch_area_weighted_mean": round(float((l2 * w).sum()), 4), "l2_stretch_p95": round(wp(l2, 95), 3),
            "texel_density_ratio_p5": round(wp(dens, 5), 4), "texel_density_ratio_p50": round(wp(dens, 50), 4), "texel_density_ratio_p95": round(wp(dens, 95), 4),
            "uv_area_share_of_unit_square": round(float(uv_area.sum() / RES ** 2), 4)}


def raster_ids(UV, F_ids, res, shrink=0.0):
    """pixel-centre rasterisation: returns (res,res) int32 map of triangle index (-1 empty) and a count map (triangles covering the pixel centre)."""
    ids = -np.ones((res, res), np.int32); first = -np.ones((res, res), np.int32); cnt = np.zeros((res, res), np.int16)
    T = UV.reshape(-1, 3, 2) * res
    for t in F_ids:
        p = T[t]
        x0, x1 = int(math.floor(p[:, 0].min())), int(math.ceil(p[:, 0].max()))
        y0, y1 = int(math.floor(p[:, 1].min())), int(math.ceil(p[:, 1].max()))
        x0, y0 = max(x0, 0), max(y0, 0); x1, y1 = min(x1, res - 1), min(y1, res - 1)
        if x1 < x0 or y1 < y0:
            continue
        xs = np.arange(x0, x1 + 1) + 0.5; ys = np.arange(y0, y1 + 1) + 0.5
        gx, gy = np.meshgrid(xs, ys)
        d = (p[1, 0] - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (p[1, 1] - p[0, 1])
        if abs(d) < 1e-12:
            continue
        l1 = ((gx - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (gy - p[0, 1])) / d
        l2 = ((p[1, 0] - p[0, 0]) * (gy - p[0, 1]) - (gx - p[0, 0]) * (p[1, 1] - p[0, 1])) / d
        inside = (l1 > 1e-9) & (l2 > 1e-9) & (1 - l1 - l2 > 1e-9)
        if inside.any():
            sl = (slice(y0, y1 + 1), slice(x0, x1 + 1))
            sub = first[sl]; sub[inside & (sub < 0)] = t
            ids[sl][inside] = t
            cnt[sl][inside] += 1
    return ids, first, cnt


def margin_check(chart_of_face, UV, res=4096, kmax=40):
    """minimum gap in pixels at 2048 between two different islands, by label dilation on a 4096 raster (approximate, +-1 px at 4096)."""
    nf = len(chart_of_face)
    ids, first, cnt = raster_ids(UV, range(nf), res)
    lab = np.where(ids >= 0, chart_of_face[np.maximum(ids, 0)] + 1, 0).astype(np.int32)
    BIG = 1 << 30

    def shifted_minmax(r):
        lo = np.where(lab > 0, lab, BIG); hi = lab.copy()
        mn = lo.copy(); mx = hi.copy()
        # separable chebyshev window min / max
        for axis in (0, 1):
            a_mn = mn.copy(); a_mx = mx.copy()
            for s in range(1, r + 1):
                for sg in (1, -1):
                    sh_mn = np.roll(mn, sg * s, axis=axis); sh_mx = np.roll(mx, sg * s, axis=axis)
                    a_mn = np.minimum(a_mn, sh_mn); a_mx = np.maximum(a_mx, sh_mx)
            mn, mx = a_mn, a_mx
        return mn, mx
    gap = None
    lo_r, hi_r = 0, kmax
    # smallest r such that some pixel sees two labels within r (gap <= 2r)
    while lo_r < hi_r:
        mid = (lo_r + hi_r) // 2
        mn, mx = shifted_minmax(mid)
        hit = ((mn < BIG) & (mx > 0) & (mn != mx)).any()
        if hit:
            hi_r = mid
        else:
            lo_r = mid + 1
    gap4096 = max(0, 2 * lo_r - 1)   # two islands within 2r px (Chebyshev) of each other: gap in [2r-1, 2r]
    return gap4096 / (res / RES), int(((cnt > 1)).sum()), ids, cnt


# ------------------------------------------------------------------------------------------------ phase uv
def mesh_topology(V, F, smooth):
    nf = len(F)
    ek = np.sort(np.concatenate([F[:, [0, 1]], F[:, [1, 2]], F[:, [2, 0]]]), axis=1)
    key = ek[:, 0] * (len(V) + 1) + ek[:, 1]
    uk, inv = np.unique(key, return_inverse=True)
    EV = np.stack([uk // (len(V) + 1), uk % (len(V) + 1)], 1)
    ef = -np.ones((len(uk), 2), np.int64)
    fidx = np.tile(np.arange(nf), 3)
    cnt = np.zeros(len(uk), int)
    for i in range(3 * nf):
        e = inv[i]
        ef[e, cnt[e]] = fidx[i]; cnt[e] += 1
    assert (cnt == 2).all(), "mesh not closed manifold"
    # per face: its 3 edge ids (edge k = vertex k to k+1)
    fe = inv.reshape(3, nf).T
    return EV, ef, fe


def flood(nf, ef, seam):
    """face components across non-seam edges"""
    adj = [[] for _ in range(nf)]
    for e in np.nonzero(~seam)[0]:
        a, b = ef[e]; adj[a].append(b); adj[b].append(a)
    comp = -np.ones(nf, np.int64); n = 0
    for s in range(nf):
        if comp[s] >= 0:
            continue
        comp[s] = n; st = [s]
        while st:
            f = st.pop()
            for g in adj[f]:
                if comp[g] < 0:
                    comp[g] = n; st.append(g)
        n += 1
    return comp, n


def cone_split(comp, ncomp, FN, AR, ef, lim_face):
    """split charts so that every face is within lim_deg of its chart's running mean normal; returns the new seam edges"""
    nf = len(comp)
    adj = [[] for _ in range(nf)]
    for e in range(len(ef)):
        a, b = ef[e]; adj[a].append((b, e)); adj[b].append((a, e))
    new = -np.ones(nf, np.int64); n = 0
    members = [[] for _ in range(ncomp)]
    for f in range(nf):
        members[comp[f]].append(f)
    for c in range(ncomp):
        for seed in sorted(members[c], key=lambda f: -AR[f]):
            if new[seed] >= 0:
                continue
            cosl = math.cos(math.radians(lim_face[seed]))
            new[seed] = n; acc = FN[seed] * AR[seed]; nm = FN[seed].copy(); st = [seed]
            while st:
                f = st.pop(0)
                for g, e in adj[f]:
                    if new[g] >= 0 or comp[g] != c:
                        continue
                    if FN[g] @ nm >= cosl:
                        new[g] = n; acc = acc + FN[g] * AR[g]; nm = acc / np.linalg.norm(acc); st.append(g)
            n += 1
    seam = new[ef[:, 0]] != new[ef[:, 1]]
    return seam


def fix_topology(V, F, ef, EV, seam, max_iter=60):
    """cut every chart that is not a disk (boundary loops > 1): shortest vertex path from a smaller boundary loop to another one"""
    nf = len(F)
    log_cuts = 0
    for it in range(max_iter):
        comp, n = flood(nf, ef, seam)
        # per chart: verts, edges, boundary edges
        ecomp0 = comp[ef[:, 0]]
        interior = ~seam   # interior edge: both faces in the same chart
        changed = False
        bnd_edges = np.nonzero(seam)[0]
        # boundary graph per chart: edges of seam owned by each of the two sides
        loops_of = {}
        for e in bnd_edges:
            for side in (0, 1):
                c = comp[ef[e, side]]
                loops_of.setdefault(c, []).append(e)
        chart_edges_int = {}
        for e in np.nonzero(interior)[0]:
            chart_edges_int.setdefault(comp[ef[e, 0]], []).append(e)
        for c, be in loops_of.items():
            # union-find on boundary vertices
            par = {}

            def find(x):
                while par.setdefault(x, x) != x:
                    par[x] = par[par[x]]; x = par[x]
                return x
            for e in be:
                a, b = EV[e]; par[find(a)] = find(b)
            groups = {}
            for v in list(par.keys()):
                groups.setdefault(find(v), set()).add(v)
            if len(groups) <= 1:
                continue
            # chart vertex graph through interior edges
            nb = {}
            for e in chart_edges_int.get(c, []):
                a, b = EV[e]; nb.setdefault(a, []).append((b, e)); nb.setdefault(b, []).append((a, e))
            gl = sorted(groups.values(), key=len)
            src = gl[0]; tgt = set().union(*gl[1:])
            prev = {v: None for v in src}; q = list(src); found = None
            while q and found is None:
                nq = []
                for v in q:
                    for w, e in nb.get(v, []):
                        if w in prev:
                            continue
                        prev[w] = (v, e)
                        if w in tgt:
                            found = w; break
                        if w not in src:
                            nq.append(w)
                    if found is not None:
                        break
                q = nq
            if found is None:
                continue
            w = found
            while prev[w] is not None:
                v, e = prev[w]; seam[e] = True; w = v; log_cuts += 1
            changed = True
        if not changed:
            break
    comp, n = flood(nf, ef, seam)
    return seam, comp, n, log_cuts


def merge_small(comp, n, ef, EV, V, FN, AR, protected_face, min_faces, level_face):
    """merge every chart under min_faces faces into the neighbour sharing the longest border (never into or out of a protected flat chart)"""
    comp = comp.copy()
    el = np.linalg.norm(V[EV[:, 0]] - V[EV[:, 1]], axis=1)
    for _ in range(200):
        size = np.bincount(comp, minlength=comp.max() + 1)
        prot = np.zeros(len(size), bool); prot[np.unique(comp[protected_face])] = True
        a, b = comp[ef[:, 0]], comp[ef[:, 1]]
        d = a != b
        pa, pb, pl = a[d], b[d], el[d]
        pair = {}
        for x, y, l in zip(pa.tolist(), pb.tolist(), pl.tolist()):
            pair[(x, y)] = pair.get((x, y), 0.0) + l
        cmean = np.zeros((len(size), 3)); np.add.at(cmean, comp, FN * AR[:, None]); cmean /= np.maximum(np.linalg.norm(cmean, axis=1), 1e-12)[:, None]
        nbrs = {}
        for (x, y), l in pair.items():
            nbrs.setdefault(x, []).append((y, l))
        level_of = np.zeros(len(size)); level_of[comp] = level_face
        small = [c for c in np.argsort(size) if 0 < size[c] < min_faces and not prot[c]]
        if not small:
            break
        merged_to = {}; touched = set()
        for c in small:
            if c in touched or c not in nbrs:
                continue
            best, bs = None, -1.0
            fc = np.nonzero(comp == c)[0]
            for y, l in nbrs[c]:
                if prot[y] or y in touched or level_of[y] != level_of[c]:
                    continue
                fy = np.nonzero(comp == y)[0]; fa = np.concatenate([fc, fy])
                mn_ = (FN[fa] * AR[fa][:, None]).sum(0); mn_ /= max(np.linalg.norm(mn_), 1e-12)
                if (FN[fa] @ mn_).min() < math.cos(math.radians(MERGE_MAX)):
                    continue
                sc = l * max(0.1, float(cmean[c] @ cmean[y]))
                if sc > bs:
                    best, bs = y, sc
            if best is None:
                continue
            merged_to[c] = best; touched.add(c); touched.add(best)
        if not merged_to:
            break
        for c, y in merged_to.items():
            comp[comp == c] = y
        _, comp = np.unique(comp, return_inverse=True)
    return comp, comp.max() + 1


def unwrap_pack(ob, me, seam, EV, V, nverts):
    be = np.empty(len(me.edges) * 2, np.int64); me.edges.foreach_get("vertices", be); be = np.sort(be.reshape(-1, 2), axis=1)
    keyb = be[:, 0] * (nverts + 1) + be[:, 1]
    keye = EV[:, 0] * (nverts + 1) + EV[:, 1]
    pos = {int(k): i for i, k in enumerate(keyb)}
    for i in range(len(me.edges)):
        me.edges[i].use_seam = False
    for e in np.nonzero(seam)[0]:
        me.edges[pos[int(keye[e])]].use_seam = True
    me.update()
    bpy.context.scene.tool_settings.use_uv_select_sync = True
    bpy.context.view_layer.objects.active = ob; ob.select_set(True)
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.unwrap(method="ANGLE_BASED", fill_holes=True, margin=0.0)
    bpy.ops.uv.select_all(action="SELECT")
    bpy.ops.uv.average_islands_scale()
    bpy.ops.uv.pack_islands(udim_source="CLOSEST_UDIM", rotate=True, margin_method="ADD", margin=MARGIN_PX / RES)
    bpy.ops.object.mode_set(mode="OBJECT")
    return get_uv(me)


def phase_uv():
    ob = rr.import_fbx(os.path.join(OLD, "SM_Extractor.fbx"))[0]
    me = ob.data
    V = cm_verts(me); F = faces_of(me); nf = len(F)
    smooth = np.array([p.use_smooth for p in me.polygons])
    uv_old = get_uv(me)
    np.savez(os.path.join(WORK, "old_uv.npz"), uv=uv_old, V=V, F=F, smooth=smooth)
    FN, AR = tri_normals(V, F)
    EV, ef, fe = mesh_topology(V, F, smooth)
    log("topology", len(V), "verts", len(EV), "edges", nf, "faces")
    dih = np.degrees(np.arccos(np.clip((FN[ef[:, 0]] * FN[ef[:, 1]]).sum(1), -1, 1)))
    flat = ~smooth
    both = flat[ef[:, 0]] & flat[ef[:, 1]]
    comp_flat, nflat = flood(nf, ef, ~(both & (dih < 0.5)))    # flat-shaded faces linked through coplanar edges
    sizes = np.bincount(comp_flat[flat], minlength=nflat)
    big_flat = flat & (sizes[comp_flat] >= MIN_FLAT)
    seam_sharp = dih > SHARP
    border = big_flat[ef[:, 0]] != big_flat[ef[:, 1]]
    border |= big_flat[ef[:, 0]] & big_flat[ef[:, 1]] & (comp_flat[ef[:, 0]] != comp_flat[ef[:, 1]])
    seam0 = seam_sharp | border
    log("seams: sharp", int(seam_sharp.sum()), "flat borders", int(border.sum()), "flat regions >= %d faces" % MIN_FLAT, int(len(np.unique(comp_flat[big_flat]))))
    comp0, n0 = flood(nf, ef, seam0)
    log("charts after sharp+flat seams", n0)
    lim_face = np.full(nf, CONE)
    rounds = []
    cen_f = V[F].mean(1)
    for rnd in range(45):
        if rnd == 0:
            comp, n = flood(nf, ef, seam0)
            seam = seam0 | cone_split(comp, n, FN, AR, ef, lim_face)
            comp, n = flood(nf, ef, seam)
            n_cone = n
            comp, n = merge_small(comp, n, ef, EV, V, FN, AR, big_flat, MIN_CHART, lim_face)
            seam = comp[ef[:, 0]] != comp[ef[:, 1]]
        else:   # bisect every failing chart along its principal axis (median of the face centres)
            comp = comp.copy(); nxt = comp.max() + 1
            for c in bad_c:
                fs = np.nonzero(comp == c)[0]
                if len(fs) < 2:
                    continue
                X = cen_f[fs] - cen_f[fs].mean(0)
                ax = np.linalg.eigh(X.T @ X)[1][:, -1]
                pr = X @ ax
                comp[fs[pr > np.median(pr)]] = nxt; nxt += 1
            comp, n = flood(nf, ef, comp[ef[:, 0]] != comp[ef[:, 1]])
            seam = comp[ef[:, 0]] != comp[ef[:, 1]]
        seam, comp, n, ncuts = fix_topology(V, F, ef, EV, seam)
        uv_new = unwrap_pack(ob, me, seam, EV, V, len(V))
        flips = stretch_metrics(V, F, uv_new, per_face=True)
        ids, first, cnt = raster_ids(uv_new, range(nf), 2 * RES)   # 4096 raster: sub-pixel overlaps count
        ovp = cnt > 1
        bad_f = set(np.nonzero(flips)[0].tolist())
        bad_f |= set(ids[ovp].tolist()); bad_f |= set(first[ovp].tolist())
        bad_c = np.unique(comp[list(bad_f)]) if bad_f else np.array([], int)
        log("round", rnd, "charts", n, "(cone %d)" % n_cone, "cut edges", ncuts, "flipped", int(flips.sum()), "overlap px", int(ovp.sum()), "failing charts", len(bad_c))
        if len(bad_c) and len(bad_c) < 12:
            log("   failing chart sizes", np.bincount(comp)[bad_c].tolist(), "min face area cm2", [round(float(AR[comp == c].min()), 4) for c in bad_c])
        rounds.append({"round": rnd, "charts": int(n), "flipped": int(flips.sum()), "overlap_pixels": int(ovp.sum()), "failing_charts": int(len(bad_c))})
        if len(bad_c) == 0:
            break
    print("UV bounds", uv_new.min(0), uv_new.max(0))
    csz = np.bincount(comp, minlength=n)
    np.savez(os.path.join(WORK, "uv.npz"), uv=uv_new, comp=comp, seam=seam, EV=EV, n_sharp=int(seam_sharp.sum()), n_flat_border=int(border.sum()), cut_edges=ncuts)
    old_m = stretch_metrics(V, F, uv_old)
    new_m = stretch_metrics(V, F, uv_new)
    log("STRETCH old", json.dumps(old_m))
    log("STRETCH new", json.dumps(new_m))
    gap, ov_px, ids, cnt = margin_check(comp, uv_new)
    log("margin px (approx, 2048)", gap, "pixel centres covered twice (4096 raster)", ov_px)
    json.dump({"old": old_m, "new": new_m, "margin_px_2048": gap, "overlap_pixels_4096": ov_px, "charts": int(n), "chart_faces_min": int(csz.min()),
               "chart_faces_median": float(np.median(csz)), "chart_faces_max": int(csz.max()), "rounds": rounds, "seams_sharp_edges": int(seam_sharp.sum()),
               "seams_flat_border_edges": int(border.sum()), "topology_cut_edges": int(ncuts), "seam_edges_total": int(seam.sum()),
               "cone_deg": CONE, "sharp_deg": SHARP, "min_chart_faces": MIN_CHART, "min_flat_faces": MIN_FLAT, "pack_margin_px": MARGIN_PX}, open(os.path.join(WORK, "uv_stats.json"), "w"), indent=1)


# ------------------------------------------------------------------------------------------------ phase bake
def fit_frame(V0, Vsm):
    """SM = s * (R V0) + offset, R = 90 deg about Z (task-366 placement); fitted on the vertices the flatten did not move"""
    R = np.array([[0, -1, 0], [1, 0, 0], [0, 0, 1.0]])
    P = V0 @ R.T
    inl = np.ones(len(P), bool)
    for _ in range(6):
        A_ = np.zeros((inl.sum() * 3, 4)); b_ = np.zeros(inl.sum() * 3)
        for k in range(3):
            A_[k::3, 0] = P[inl, k]; A_[k::3, 1 + k] = 1.0; b_[k::3] = Vsm[inl, k]
        sol = np.linalg.lstsq(A_, b_, rcond=None)[0]
        s_, off = sol[0], sol[1:]
        res = np.linalg.norm(P * s_ + off - Vsm, axis=1)
        inl = res < max(1e-3, np.percentile(res, 50) * 3)
    return R, float(s_), off, res, int(inl.sum())


def pre_flatten(VD, FD, dn):
    lab = dn["lab"]
    ids = [int(i) for i in dn["plane_id"]]
    Pc = np.array(dn["plane_c"]); Pn = np.array(dn["plane_n"])
    in_reg = np.isin(lab, ids)
    pairs = np.unique(np.stack([FD[in_reg].ravel(), np.repeat(lab[in_reg], 3)], 1), axis=0)
    pv, pr = pairs[:, 0], pairs[:, 1]
    pid_of = {p: i for i, p in enumerate(ids)}
    ri = np.array([pid_of[r] for r in pr])
    Vf = VD.copy()
    for _ in range(12):
        for k in range(len(ids)):
            sel = pv[ri == k]
            if len(sel):
                Vf[sel] -= ((Vf[sel] - Pc[k]) @ Pn[k])[:, None] * Pn[k]
    mv = np.linalg.norm(Vf - VD, axis=1)
    too_far = mv > 0.8
    Vf[too_far] = VD[too_far]
    return Vf, in_reg


def make_img(name, cs, fill=(0, 0, 0, 1)):
    im = bpy.data.images.new(name, RES, RES, alpha=False, float_buffer=True)
    im.colorspace_settings.name = cs
    im.pixels.foreach_set(np.tile(np.array(fill, np.float32), RES * RES)); return im


def build_dense_and_low():
    uvz = np.load(os.path.join(WORK, "uv.npz"))
    dm = np.load(os.path.join(WORK, "dense_mesh.npz")); VD = dm["V"].astype(np.float64); FD = dm["F"].astype(np.int64)
    dn = np.load(os.path.join(WORK, "dense.npz"))
    Vf, in_reg = pre_flatten(VD, FD, dn)
    log("pre-flatten", int((np.linalg.norm(Vf - VD, axis=1) > 1e-6).sum()), "vertices moved")
    dec = rr.import_fbx(os.path.join(WORK, "decimated.fbx"))[0]
    V0 = cm_verts(dec.data)
    old = rr.import_fbx(os.path.join(OLD, "SM_Extractor.fbx"))[0]
    Vsm = cm_verts(old.data)
    assert len(V0) == len(Vsm), "decimated vertex count differs from SM_Extractor: %d %d" % (len(V0), len(Vsm))
    R, scale, off, res, ninl = fit_frame(V0, Vsm)
    log("frame fit: scale", scale, "offset", off.round(4), "inliers", ninl, "of", len(V0), "median residual cm", float(np.median(res)), "p90", float(np.percentile(res, 90)))
    Vdense = (Vf @ R.T) * scale + off
    frame = {"scale": scale, "offset_cm": off.tolist(), "inliers": ninl, "median_residual_cm": float(np.median(res)), "p90_residual_cm": float(np.percentile(res, 90)),
             "note": "SM = scale * (Rz90 V_raw) + offset"}
    hi = rr.import_fbx(RAW)[0]
    me_h = hi.data
    log("dense imported", len(me_h.polygons), "faces, custom normals", me_h.has_custom_normals, "matrix identity", bool(np.allclose(np.array(hi.matrix_world), np.eye(4))))
    assert len(me_h.vertices) == len(VD)
    me_h.vertices.foreach_set("co", (Vdense / 100.0).ravel())
    if me_h.has_custom_normals:
        bpy.context.view_layer.objects.active = hi
        bpy.ops.mesh.customdata_custom_splitnormals_clear()
    sm_flags = np.ones(len(FD), bool); sm_flags[in_reg] = False
    me_h.polygons.foreach_set("use_smooth", sm_flags); me_h.update()
    lo = rr.import_fbx(os.path.join(OLD, "SM_Extractor.fbx"), clear=False)
    lo = [o for o in lo if o is not hi][0]
    lo.name = "low"
    lo.data.uv_layers[0].uv.foreach_set("vector", uvz["uv"].ravel()); lo.data.update()
    return hi, lo, uvz, frame


def phase_bake():
    hi, lo, uvz, frame = build_dense_and_low()
    json.dump(frame, open(os.path.join(WORK, "frame.json"), "w"))
    me_h, me_l = hi.data, lo.data
    UV = uvz["uv"]
    nfl = len(me_l.polygons)
    ids, first, cnt = raster_ids(UV, range(nfl), RES)
    inside = ids >= 0
    log("island pixels (pixel centres)", int(inside.sum()))
    sc = bpy.context.scene
    sc.render.engine = "CYCLES"; sc.cycles.device = "CPU"; sc.cycles.use_denoising = False
    bk = sc.render.bake
    bk.use_selected_to_active = True; bk.cage_extrusion = EXTR / 100.0; bk.max_ray_distance = RAYD / 100.0
    bk.margin_type = "EXTEND"; bk.use_clear = False
    bk.normal_space = "TANGENT"; bk.normal_r = "POS_X"; bk.normal_g = "POS_Y"; bk.normal_b = "POS_Z"

    def dense_mat(name, kind):
        m = bpy.data.materials.new(name); m.use_nodes = True; nt = m.node_tree
        for n in list(nt.nodes):
            nt.nodes.remove(n)
        out = nt.nodes.new("ShaderNodeOutputMaterial")
        if kind in ("mask0", "reach"):
            em = nt.nodes.new("ShaderNodeEmission"); em.inputs["Color"].default_value = (1, 1, 1, 1)
            nt.links.new(em.outputs[0], out.inputs["Surface"])
            return m
        tx = nt.nodes.new("ShaderNodeTexImage"); tx.interpolation = "Linear"
        tx.image = bpy.data.images.load(os.path.join(SRC_MAPS, MAPS[kind])); tx.image.colorspace_settings.name = "sRGB" if kind == "BaseColor" else "Non-Color"
        if kind == "Normal":
            nm = nt.nodes.new("ShaderNodeNormalMap"); nt.links.new(tx.outputs["Color"], nm.inputs["Color"])
            bs = nt.nodes.new("ShaderNodeBsdfPrincipled"); nt.links.new(nm.outputs["Normal"], bs.inputs["Normal"])
            nt.links.new(bs.outputs[0], out.inputs["Surface"])
        else:
            em = nt.nodes.new("ShaderNodeEmission"); nt.links.new(tx.outputs["Color"], em.inputs["Color"])
            nt.links.new(em.outputs[0], out.inputs["Surface"])
        return m

    def bake_pass(kind, btype, margin, samples, fill):
        im = make_img("bake_" + kind, "Non-Color", fill)
        lm = bpy.data.materials.new("lowbake"); lm.use_nodes = True
        nt = lm.node_tree; tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = im; nt.nodes.active = tx
        me_l.materials.clear(); me_l.materials.append(lm)
        me_h.materials.clear(); me_h.materials.append(dense_mat("dense_" + kind, kind))
        sc.cycles.samples = samples
        bk.margin = margin
        bpy.ops.object.select_all(action="DESELECT"); hi.select_set(True); lo.select_set(True)
        bpy.context.view_layer.objects.active = lo
        t1 = time.time()
        bpy.ops.object.bake(type=btype)
        log("baked", kind, "in %.0fs" % (time.time() - t1))
        px = np.empty(RES * RES * 4, np.float32); im.pixels.foreach_get(px)
        return px.reshape(RES, RES, 4)[:, :, :3].copy()
    mask0 = bake_pass("mask0", "EMIT", 0, 1, (0, 0, 0, 1))[:, :, 0]
    hit = mask0 > 0.5
    missed = inside & ~hit
    log("hit mask: island pixels", int(inside.sum()), "hit", int((hit & inside).sum()), "MISSED", int(missed.sum()))
    reach = bake_pass("reach", "EMIT", DILATE, 1, (0, 0, 0, 1))[:, :, 0] > 0.5
    col = bake_pass("BaseColor", "EMIT", DILATE, SAMPLES, (0, 0, 0, 1))
    met = bake_pass("Metallic", "EMIT", DILATE, SAMPLES, (0, 0, 0, 1))
    rou = bake_pass("Roughness", "EMIT", DILATE, SAMPLES, (0, 0, 0, 1))
    nor = bake_pass("Normal", "NORMAL", DILATE, SAMPLES, (0.5, 0.5, 1, 1))
    np.savez_compressed(os.path.join(WORK, "baked.npz"), col=col, met=met, rou=rou, nor=nor, hit=hit, inside=inside, reach=reach, missed=missed, extr=EXTR, ray=RAYD)
    log("saved baked.npz; bake colour mean (linear)", col[inside].mean(0), "ray", RAYD, "extrusion", EXTR)


# ------------------------------------------------------------------------------------------------ phase final
def srgb_encode(x):
    x = np.clip(x, 0, 1)
    return np.where(x <= 0.0031308, 12.92 * x, 1.055 * np.power(x, 1 / 2.4) - 0.055)


def save_png(path, rgb, bw=False):
    """8-bit PNG written raw (no colour transform): grayscale for bw (like the Meshy metallic and roughness maps), else RGB. Row 0 of rgb is the bottom of the image."""
    import struct
    import zlib
    a = np.rint(np.clip(rgb, 0, 1) * 255).astype(np.uint8)[::-1]
    if bw:
        a = np.rint(a.astype(np.float32).mean(2)).astype(np.uint8); ctype = 0
    else:
        ctype = 2
    h, w = a.shape[:2]
    raw = np.concatenate([np.zeros((h, 1), np.uint8), a.reshape(h, -1)], axis=1).tobytes()

    def chunk(tag, data):
        c = struct.pack(">I", len(data)) + tag + data
        return c + struct.pack(">I", zlib.crc32(tag + data) & 0xffffffff)
    with open(path, "wb") as fh:
        fh.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, ctype, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


def phase_final():
    bz = np.load(os.path.join(WORK, "baked.npz")); uvz = np.load(os.path.join(WORK, "uv.npz")); oz = np.load(os.path.join(WORK, "old_uv.npz"))
    uvs = json.load(open(os.path.join(WORK, "uv_stats.json"))); frame = json.load(open(os.path.join(WORK, "frame.json")))
    inside, reach, hit = bz["inside"], bz["reach"], bz["hit"]
    # ---- maps: base colour linear -> sRGB, pixels the dilation did not reach get the mean of the island pixels
    col_s = srgb_encode(bz["col"]); met, rou, nor = bz["met"], bz["rou"], bz["nor"]
    valid = hit | ~inside
    still = inside & ~hit
    n_missed0 = int(still.sum())
    rounds_fill = 0
    for rounds_fill in range(1, 41):
        if not still.any():
            break
        vf = valid.astype(np.float32)
        def nsum(a):
            acc = np.zeros_like(a)
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    if dy or dx:
                        acc += np.roll(np.roll(a, dy, 0), dx, 1)
            return acc
        cnt_n = nsum(vf)
        can = still & (cnt_n > 0)
        for arr in (col_s, met, rou, nor):
            num = nsum(arr * vf[:, :, None])
            arr[can] = num[can] / cnt_n[can][:, None]
        valid = valid | can
        still = still & ~can
    log("missed island pixels inpainted from baked neighbours:", n_missed0, "in", rounds_fill, "rounds, left", int(still.sum()))
    fills = {}
    for name, arr in (("BaseColor", col_s), ("Metallic", met), ("Roughness", rou)):
        mean = arr[inside].mean(0); fills[name] = mean
        arr[~reach] = mean
    nor = nor.copy(); nor[~reach] = (0.5, 0.5, 1.0)
    outs = {"BaseColor": col_s, "Metallic": met, "Roughness": rou, "Normal": nor}
    paths = {}
    for k, arr in outs.items():
        paths[k] = os.path.join(OUTDIR, "T_Extractor_%s.png" % k)
        save_png(paths[k], arr, bw=k in ("Metallic", "Roughness"))
    log("maps written")
    # ---- pixels inside the islands that are still unbaked (exact clear value) after the dilation
    zero_col = int(still.sum())
    n_dec = nor * 2 - 1
    zero_nor = int((inside & (np.linalg.norm(n_dec, axis=2) < 0.05)).sum())
    # ---- normal tilt on the flat-shaded faces (island pixels of flat faces) and on all faces
    ob = rr.import_fbx(os.path.join(OLD, "SM_Extractor.fbx"))[0]
    me = ob.data
    V = cm_verts(me); F = faces_of(me); nf = len(F)
    flat_face = np.array([not p.use_smooth for p in me.polygons])
    UV = uvz["uv"]
    ids, first, cnt = raster_ids(UV, range(nf), RES)
    vec = n_dec
    tilt = np.degrees(np.arccos(np.clip(vec[:, :, 2] / np.maximum(np.linalg.norm(vec, axis=2), 1e-9), -1, 1)))
    fp = inside & flat_face[np.maximum(ids, 0)]
    allp = inside
    tl = {"flat_faces": int(flat_face.sum()), "flat_face_pixels": int(fp.sum()),
          "flat_mean_deg": round(float(tilt[fp].mean()), 3), "flat_p95_deg": round(float(np.percentile(tilt[fp], 95)), 3), "flat_max_deg": round(float(tilt[fp].max()), 3),
          "all_faces_mean_deg": round(float(tilt[allp].mean()), 3), "all_faces_p95_deg": round(float(np.percentile(tilt[allp], 95)), 3),
          "flat_median_deg": round(float(np.median(tilt[fp])), 3), "flat_share_over_30deg_pct": round(float((tilt[fp] > 30).mean() * 100), 3),
          "flat_mean_deg_excluding_pixels_over_30deg": round(float(tilt[fp][tilt[fp] <= 30].mean()), 3),
          "flat_face_centre_samples_mean_deg": round(float(tilt[np.clip((UV.reshape(-1, 3, 2).mean(1)[np.nonzero(flat_face)[0]] * RES).astype(int)[:, 1], 0, RES - 1),
                                                           np.clip((UV.reshape(-1, 3, 2).mean(1)[np.nonzero(flat_face)[0]] * RES).astype(int)[:, 0], 0, RES - 1)].mean()), 3),
          "meshy_map_reference_mean_deg_on_mapped_faces": 3.699, "limit_mean_deg": 3.7,
          "note": "pixels over 30 deg are relief of the dense mesh inside the flat faces' footprint (bolts, panel borders), baked on purpose (Q2: dense shape plus Meshy detail)"}
    # old map tilt on the same flat faces through the old UVs (same method), for the comparison
    log("tilt", json.dumps(tl))
    # ---- missed rays located
    missed = bz["missed"]
    mf = ids[missed]
    mtri = np.unique(mf)
    cen = V[F].mean(1)
    tot = np.bincount(ids[ids >= 0], minlength=nf)
    mc = np.bincount(mf, minlength=nf)
    frac = mc[mtri] / np.maximum(tot[mtri], 1)
    missed_rep = {"island_pixels": int(inside.sum()), "missed_pixels": int(missed.sum()), "missed_share_pct": round(float(missed.sum() / inside.sum() * 100), 4),
                  "faces_touched": int(len(mtri)), "faces_fully_missed": int((frac >= 0.999).sum()), "median_share_of_face_pixels_missed": round(float(np.median(frac)), 3) if len(mtri) else 0,
                  "located": "edge pixels of %d faces, scattered over the whole mesh (z %.1f to %.1f cm); filled from the baked 8-neighbours (iterated, %d round(s)); island pixels left unbaked: %d; normal vectors shorter than 0.05: %d"
                             % (len(mtri), float(cen[mtri][:, 2].min()) if len(mtri) else 0, float(cen[mtri][:, 2].max()) if len(mtri) else 0, rounds_fill, zero_col, zero_nor),
                  "flat_shaded_faces_among_missed": int(flat_face[mtri].sum()) if len(mtri) else 0}
    # ---- the game mesh: same geometry, new UV0, one material
    ob = rr.import_fbx(os.path.join(OLD, "SM_Extractor.fbx"))[0]
    me = ob.data
    me.uv_layers[0].uv.foreach_set("vector", UV.ravel()); me.update()
    mat = bpy.data.materials.new("M_Extractor"); mat.use_nodes = True
    nt = mat.node_tree; bsdf = nt.nodes["Principled BSDF"]

    def img(k, cs):
        n = nt.nodes.new("ShaderNodeTexImage"); n.image = bpy.data.images.load(paths[k]); n.image.colorspace_settings.name = cs; return n
    nt.links.new(img("BaseColor", "sRGB").outputs["Color"], bsdf.inputs["Base Color"])
    nt.links.new(img("Metallic", "Non-Color").outputs["Color"], bsdf.inputs["Metallic"])
    nt.links.new(img("Roughness", "Non-Color").outputs["Color"], bsdf.inputs["Roughness"])
    nm = nt.nodes.new("ShaderNodeNormalMap"); nt.links.new(img("Normal", "Non-Color").outputs["Color"], nm.inputs["Color"])
    nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    me.materials.clear(); me.materials.append(mat)
    ob.name = "SM_Extractor"; me.name = "SM_Extractor"
    fbx = os.path.join(OUTDIR, "SM_Extractor.fbx")
    rr.export_fbx(fbx, [ob])
    log("exported", fbx)
    # ---- reimport in a clean scene and compare with the task-366 mesh
    n2 = rr.import_fbx(fbx)[0]
    m2 = n2.data
    old = rr.import_fbx(os.path.join(OLD, "SM_Extractor.fbx"), clear=False)
    old = [o for o in old if o is not n2][0]
    mo = old.data
    Vn, Fn = cm_verts(m2), faces_of(m2)
    Vo, Fo = cm_verts(mo), faces_of(mo)

    def corner_pos(Vx, Fx):
        return Vx[Fx]   # (nf, 3 corners, 3)
    Pn_, Po_ = corner_pos(Vn, Fn), corner_pos(Vo, Fo)
    same_face_order = Pn_.shape == Po_.shape
    if same_face_order:
        d_direct = np.abs(Pn_ - Po_).max()
    else:
        d_direct = float("nan")
    # order-independent: faces matched by centroid (rounded) then corner positions sorted
    def canon(P):
        c = P.mean(1)
        o = np.lexsort((c[:, 2], c[:, 1], c[:, 0]))
        return P[o], c[o]
    Pn_c, cn_ = canon(Pn_); Po_c, co_ = canon(Po_)
    d_centroid = float(np.abs(cn_ - co_).max())
    smn = np.array([p.use_smooth for p in m2.polygons]); smo = np.array([p.use_smooth for p in mo.polygons])
    flat_same = bool(same_face_order and (smn == smo).all())
    cnn = np.empty(len(m2.loops) * 3); m2.corner_normals.foreach_get("vector", cnn)
    cno = np.empty(len(mo.loops) * 3); mo.corner_normals.foreach_get("vector", cno)
    cn_ang = None
    if len(cnn) == len(cno) and same_face_order:
        a = cnn.reshape(-1, 3); b = cno.reshape(-1, 3)
        cn_ang = np.degrees(np.arccos(np.clip((a * b).sum(1), -1, 1)))
    uv2 = get_uv(m2)
    mn2, mx2 = Vn.min(0), Vn.max(0)
    geom = {"triangles": int(rr.tri_count(m2)), "triangles_task366": int(rr.tri_count(mo)), "same_face_count": bool(same_face_order),
            "max_corner_position_difference_cm_in_face_order": round(float(d_direct), 6), "max_face_centroid_difference_cm_sorted": round(d_centroid, 6),
            "vertices_new_after_uv_split": int(len(Vn)), "vertices_task366": int(len(Vo)),
            "flat_shaded_faces_new": int((~smn).sum()), "flat_shaded_faces_task366": int((~smo).sum()), "flat_flags_identical": flat_same,
            "corner_normal_max_angle_deg": round(float(cn_ang.max()), 4) if cn_ang is not None else None,
            "corner_normal_mean_angle_deg": round(float(cn_ang.mean()), 5) if cn_ang is not None else None,
            "bounds_cm": [round(float(t), 3) for t in (mx2 - mn2)], "min_cm": [round(float(t), 3) for t in mn2], "max_cm": [round(float(t), 3) for t in mx2]}
    # UV checks on the reimported mesh (per-corner UV in the FBX): range, coverage, overlap, stretch
    nfn = len(Fn)
    ids2, first2, cnt2 = raster_ids(uv2, range(nfn), 2 * RES)
    # island id from UV connectivity of the reimported mesh = by vertex (the FBX splits vertices on UV seams)
    comp2 = uv_islands_from_uv(Fn, uv2, nfn)
    gap, ov_px, _, _ = margin_check(comp2, uv2)
    st_new = stretch_metrics(Vn, Fn, uv2)
    uvchk = {"uv_layers": [u.name for u in m2.uv_layers], "loops": int(len(uv2)), "faces_with_nonzero_uv_area": int(
        (np.abs(np.cross(uv2.reshape(nfn, 3, 2)[:, 1] - uv2.reshape(nfn, 3, 2)[:, 0], uv2.reshape(nfn, 3, 2)[:, 2] - uv2.reshape(nfn, 3, 2)[:, 0])) > 1e-12).sum()),
             "min_uv": [round(float(t), 5) for t in uv2.min(0)], "max_uv": [round(float(t), 5) for t in uv2.max(0)],
             "loops_outside_0_1": int(((uv2 < 0) | (uv2 > 1)).sum()), "islands": int(comp2.max() + 1),
             "overlap_pixel_centres_4096": int((cnt2 > 1).sum()), "margin_px_2048": gap, "stretch": st_new}
    w = []
    if geom["triangles"] != 47000: w.append("triangles %d, expected 47000" % geom["triangles"])
    if max(d_direct if d_direct == d_direct else 0, d_centroid) > 0.001: w.append("vertex position difference above 0.001 cm")
    if not flat_same: w.append("flat-shaded flags differ from the task-366 mesh")
    if uvchk["loops_outside_0_1"]: w.append("UV outside 0-1")
    if uvchk["overlap_pixel_centres_4096"]: w.append("overlapping UV pixels")
    if gap < 8: w.append("margin below 8 px: %.1f" % gap)
    if tl["flat_mean_deg"] > 3.7: w.append("normal tilt on flat faces above 3.7 deg")
    if len(m2.materials) != 1: w.append("material slots != 1")
    if missed_rep["missed_pixels"]: w.append("%d island pixels had no ray hit (located, filled from baked neighbours)" % missed_rep["missed_pixels"])
    report = {
        "asset": "SM_Extractor", "task": "task-368-01-extractor-rebake-maps", "script": "Tools/art/blender/extractor_rebake.py",
        "uv_method": {"description": "seams = edges sharper than %g deg + borders of flat-shaded regions of at least %d faces; extra cuts: cone charts (%g deg from the chart mean normal), "
                                     "small charts (< %d faces) merged into the neighbour with the longest border while the merged chart stays within %g deg, "
                                     "one boundary loop per chart (path cuts), failing charts (flipped or self-overlapping) bisected until none fails; Blender angle-based unwrap "
                                     "(fill holes), average island scale (uniform texel density), pack with rotation, %g px margin" % (SHARP, MIN_FLAT, CONE, MIN_CHART, MERGE_MAX, MARGIN_PX),
                      "seam_edges_sharp_60deg": uvs["seams_sharp_edges"], "seam_edges_flat_border": uvs["seams_flat_border_edges"], "topology_cut_edges": uvs["topology_cut_edges"],
                      "seam_edges_total": uvs["seam_edges_total"], "rounds": uvs["rounds"]},
        "uv_layout": uvchk,
        "stretch_old_uv0": uvs["old"], "stretch_new_uv0": uvs["new"],
        "stretch_note": "J maps each 3D triangle (cm) to texels at 2048; anisotropy = sigma1/sigma2; l2 = Sander L2 stretch over the global density (1.0 = no stretch); density ratio = sqrt(s1 s2) over the global density; area-weighted. The old UV0 flips %d triangles (mirrored/overlapping) and varies the texel density from %.2f to %.2f of the mean (p5 to p95)." % (
            uvs["old"]["flipped_triangles"], uvs["old"]["texel_density_ratio_p5"], uvs["old"]["texel_density_ratio_p95"]),
        "bake": {"engine": "Cycles CPU, selected to active (dense Meshy mesh to SM_Extractor)", "samples_colour_normal": SAMPLES, "cage_extrusion_cm": EXTR, "max_ray_distance_cm": RAYD,
                 "dilation_px": DILATE, "margin_type": "EXTEND", "maps": {k: os.path.relpath(v, ROOT).replace("\\", "/") for k, v in paths.items()},
                 "resolution": [RES, RES], "colour_spaces": {"BaseColor": "sRGB (encoded here from the linear bake)", "Metallic": "Non-Color", "Roughness": "Non-Color", "Normal": "Non-Color"},
                 "dense_source": "raw Meshy mesh in the SM_Extractor frame: Rz90, scale %.6f, offset %s cm (fitted on %d unmoved vertices, median residual %.2e cm), planar-region vertices projected flat as in task-366, "
                                 "region faces flat-shaded, others smooth; base colour, metallic and roughness through the Meshy UVs, normal = dense shape + Meshy normal map" % (
                                     frame["scale"], [round(t, 4) for t in frame["offset_cm"]], frame["inliers"], frame["median_residual_cm"]),
                 "fill_outside_islands": {"BaseColor": [round(float(t), 4) for t in fills["BaseColor"]], "Metallic": [round(float(t), 4) for t in fills["Metallic"]],
                                          "Roughness": [round(float(t), 4) for t in fills["Roughness"]], "Normal": [0.5, 0.5, 1.0]},
                 "missed_rays": missed_rep},
        "normal_convention": "tangent space, OpenGL (+Y up) like the Meshy normal map; for Unreal flip the green channel at import (or enable Flip Green Channel). Tangent basis: Blender MikkTSpace on the new UV0.",
        "normal_tilt_deg": tl,
        "geometry_check": geom,
        "colour_difference": "see render comparison (section added by the compare phase)",
        "warnings": w,
    }
    json.dump(report, open(os.path.join(OUTDIR, "extractor_rebake.report.json"), "w"), indent=2)
    log("REPORT", json.dumps({"geom": geom, "warnings": w, "tilt": tl}, default=str)[:1500])


def uv_islands_from_uv(F, UVc, nf):
    """island id per face: two faces share an island when they share an edge and both corners of the edge carry the same UV (1e-6)"""
    UVf = UVc.reshape(nf, 3, 2)
    par = np.arange(nf)

    def find(x):
        while par[x] != x:
            par[x] = par[par[x]]; x = par[x]
        return x
    edge = {}
    for f in range(nf):
        for k in range(3):
            a, b = int(F[f, k]), int(F[f, (k + 1) % 3])
            ua, ub = UVf[f, k], UVf[f, (k + 1) % 3]
            key = (min(a, b), max(a, b))
            edge.setdefault(key, []).append((f, a, ua, ub))
    for key, lst in edge.items():
        if len(lst) == 2:
            (f1, a1, ua1, ub1), (f2, a2, ua2, ub2) = lst
            if a1 == a2:
                same = np.abs(ua1 - ua2).max() < 1e-6 and np.abs(ub1 - ub2).max() < 1e-6
            else:
                same = np.abs(ua1 - ub2).max() < 1e-6 and np.abs(ub1 - ua2).max() < 1e-6
            if same:
                par[find(f1)] = find(f2)
    roots = np.array([find(f) for f in range(nf)])
    _, comp = np.unique(roots, return_inverse=True)
    return comp


def flood_by_vertex(F, nf):
    """island id per face of the reimported mesh: faces sharing a vertex (the FBX import splits vertices on UV seams, so this is the UV island)"""
    par = np.arange(F.max() + 1)

    def find(x):
        while par[x] != x:
            par[x] = par[par[x]]; x = par[x]
        return x
    for f in range(nf):
        a, b, c = (find(int(F[f, 0])), find(int(F[f, 1])), find(int(F[f, 2])))
        par[b] = a; par[find(c)] = find(a)
    roots = np.array([find(int(F[f, 0])) for f in range(nf)])
    _, comp = np.unique(roots, return_inverse=True)
    return comp


# ------------------------------------------------------------------------------------------------ phase compare
def read_png(path):
    im = bpy.data.images.load(path); im.colorspace_settings.name = "Non-Color"
    w, h = im.size
    px = np.empty(w * h * 4, np.float32); im.pixels.foreach_get(px)
    bpy.data.images.remove(im)
    return px.reshape(h, w, 4)[:, :, :3] * 255.0


def phase_compare():
    R = os.path.join(ROOT, "Saved", "ExtractorMesh", "rebake")
    views = {"preview": ["front", "side", "three_quarter", "rear_quarter", "top"], "close": ["panelA", "panelB"]}
    out = {"unit": "8-bit sRGB values (0-255), mean absolute difference per channel, over the pixels where the object is drawn in any of the three renders",
           "method": "same cameras and light for source, old SM_Extractor and new SM_Extractor; block = 8 x 8 pixel block average first (removes silhouette and sub-pixel alignment noise)"}
    tot = {"old": [], "new": [], "old_block": [], "new_block": []}
    per = {}
    for grp, names in views.items():
        for n in names:
            if grp == "preview":
                s, o, w_ = (read_png(os.path.join(R, "%s_%s.png" % (p, n))) for p in ("source", "old_sm", "new_sm"))
            else:
                s, o, w_ = (read_png(os.path.join(R, "%s_close_%s.png" % (p, n))) for p in ("source", "old_sm", "new_sm"))
            bg = s[0, 0]
            fg = ((np.abs(s - bg).max(2) > 2) | (np.abs(o - bg).max(2) > 2) | (np.abs(w_ - bg).max(2) > 2))

            def blk(a):
                h, w = a.shape[0] // 8 * 8, a.shape[1] // 8 * 8
                return a[:h, :w].reshape(h // 8, 8, w // 8, 8, -1).mean((1, 3))
            fb = blk(fg.astype(np.float32)[:, :, None])[:, :, 0] > 0.5
            e = {}
            for k, a in (("old", o), ("new", w_)):
                d = np.abs(a - s).mean(2)
                db = np.abs(blk(a) - blk(s)).mean(2)
                e[k] = round(float(d[fg].mean()), 3); e[k + "_block"] = round(float(db[fb].mean()), 3)
                tot[k].append(float(d[fg].mean())); tot[k + "_block"].append(float(db[fb].mean()))
            e["object_pixels"] = int(fg.sum())
            per["%s_%s" % (grp, n)] = e
    out["per_view"] = per
    out["mean_over_5_preview_views"] = {k: round(float(np.mean(v[:5])), 3) for k, v in tot.items()}
    out["mean_over_2_closeups"] = {k: round(float(np.mean(v[5:])), 3) for k, v in tot.items()}
    out["new_lower_than_old_in_every_view"] = bool(all(per[k]["new"] < per[k]["old"] and per[k]["new_block"] < per[k]["old_block"] for k in per))
    rp = os.path.join(OUTDIR, "extractor_rebake.report.json")
    rep = json.load(open(rp))
    rep["colour_difference"] = out
    rep["renders_dir"] = "Saved/ExtractorMesh/rebake (not committed)"
    if not out["new_lower_than_old_in_every_view"]:
        rep["warnings"].append("colour difference to the source not lower than the old mesh in every view")
    json.dump(rep, open(rp, "w"), indent=2)
    log("COLOUR", json.dumps(out)[:2500])


if __name__ == "__main__":
    try:
        {"uv": phase_uv, "bake": phase_bake, "final": phase_final, "compare": phase_compare}[PHASE]()
    except Exception:
        import traceback
        traceback.print_exc()
        sys.exit(1)
