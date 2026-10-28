"""Turret: flatten the bulged flat surfaces of the optimised mesh (task-347-01), headless Blender 5.2.

Usage (repository root):
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/turret_flatten.py -- [--in <fbx>] [--out-dir Art/Meshes/turret]
        [--ang 3] [--min-area-pct 0.5] [--grow-dist 0.25] [--grow-ang 10] [--grow-min-area-pct 0.05]
        [--dist 1.5] [--face-ang 20] [--max-move 1.5] [--measure-only]

1. Measures what makes the faces look bulged (vertex distance to the fitted plane, split normal deviation, normal map tilt).
2. Planar regions on the unoptimised Meshy mesh. Q3 default (connected faces within --ang degrees of the running mean normal,
   area >= --min-area-pct of the surface) is run and logged as a baseline; it finds only 6 underside regions because the Meshy
   surface is noisy at triangle scale. The regions actually used grow by plane distance (face centre within --grow-dist cm of
   the running least-squares plane, normal within --grow-ang degrees), area >= --grow-min-area-pct of the surface.
3. Optimised body faces (canon material excluded) mapped to a region: nearest unoptimised face (centre) is in the region,
   every vertex within --dist cm of the plane, face normal within --face-ang degrees of the plane normal.
4. One plane per region (least squares on the unoptimised region vertices); mapped vertices are projected onto it, a vertex
   shared by several regions goes to the intersection of their planes (minimum move). A move above --max-move cm is refused
   (1 % of the turret height = 1.56 cm), the vertex keeps its position and is reported.
5. Mapped faces get flat shading (split normals = face normal). UVs, topology, canon vertices untouched.
6. Export over SM_Turret.fbx (cm, pivot bottom centre) and write turret_flat.report.json.
"""
import bpy
import json
import math
import os
import sys
import time
import numpy as np
from mathutils import Vector
from mathutils.bvhtree import BVHTree

ROOT = os.getcwd()
A = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []


def opt(name, default, cast=float):
    return cast(A[A.index("--" + name) + 1]) if ("--" + name) in A else default


OUT = os.path.join(ROOT, opt("out-dir", "Art/Meshes/turret", str))
IN_FBX = os.path.join(ROOT, opt("in", "Art/Meshes/turret/SM_Turret.fbx", str))
UN_FBX = os.path.join(ROOT, "Art/Source/turret-meshy/meshy-unoptimised/Meshy_AI_Teal_Bastion_Gunner_20261028_texture_remesh_tri.fbx")
ANG, MIN_PCT, DIST, FACE_ANG, MAX_MOVE = opt("ang", 3.0), opt("min-area-pct", 0.5), opt("dist", 1.5), opt("face-ang", 20.0), opt("max-move", 1.5)
GROW_D, GROW_A, GROW_PCT = opt("grow-dist", 0.25), opt("grow-ang", 10.0), opt("grow-min-area-pct", 0.05)
MEASURE_ONLY = "--measure-only" in A
PIVOT_OFFSET_CM = np.array([0.641, 0.355, 78.116])   # SM_Turret = Meshy source frame + offset (turret_canon.report.json)
BUDGET = 16000
NORMAL_MAP = os.path.join(ROOT, "Art/Meshes/turret/T_Turret_Body_Normal.png")
warnings = []


def read_mesh(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.context.scene.objects if o.type == "MESH"][0]


def arrays(me):
    nv, nf = len(me.vertices), len(me.polygons)
    V = np.empty(nv * 3); me.vertices.foreach_get("co", V); V = V.reshape(-1, 3) * 100.0
    F = np.empty(nf * 3, int); me.polygons.foreach_get("vertices", F); F = F.reshape(-1, 3)
    return V, F


def tri_normals(V, F):
    a, b, c = V[F[:, 0]], V[F[:, 1]], V[F[:, 2]]
    cr = np.cross(b - a, c - a)
    ar = np.linalg.norm(cr, axis=1) / 2
    return cr / np.maximum(2 * ar[:, None], 1e-12), ar


def sites_of(V):
    _, s = np.unique(np.round(V, 3), axis=0, return_inverse=True)
    return s.ravel()


# ------------------------------------------------------------------ unoptimised regions
def adjacency(V, F):
    S = sites_of(V)[F]
    nf = len(F)
    e = np.concatenate([S[:, [0, 1]], S[:, [1, 2]], S[:, [2, 0]]]); fi = np.tile(np.arange(nf), 3)
    k = np.sort(e, axis=1); key = k[:, 0].astype(np.int64) * (S.max() + 1) + k[:, 1]
    o = np.argsort(key, kind="stable"); ks = key[o]; fs = fi[o]
    adj = [[] for _ in range(nf)]
    for i in np.nonzero(ks[1:] == ks[:-1])[0]:
        adj[fs[i]].append(fs[i + 1]); adj[fs[i + 1]].append(fs[i])
    return adj


def detect_regions(V, F, adj, N, area):
    """Q3 default: normals within ANG degrees of the running mean normal."""
    nf = len(F)
    cosT = math.cos(math.radians(ANG))
    lab = -np.ones(nf, int); regs = []
    for s in np.argsort(-area):
        if lab[s] >= 0:
            continue
        rid = len(regs); lab[s] = rid; mem = [s]; msum = N[s] * area[s]; stack = [s]
        while stack:
            f = stack.pop(); m = msum / np.linalg.norm(msum)
            for g in adj[f]:
                if lab[g] < 0 and N[g] @ m >= cosT:
                    lab[g] = rid; mem.append(g); msum = msum + N[g] * area[g]; stack.append(g)
        regs.append(mem)
    return lab, regs


def grow_planes(V, F, adj, N, area):
    """Regions grown by plane distance: noise at triangle scale does not split a flat panel."""
    nf = len(F)
    cen = V[F].mean(1)
    cosn = math.cos(math.radians(GROW_A))
    lab = -np.ones(nf, int); regs = []
    for s in np.argsort(-area):
        if lab[s] >= 0:
            continue
        rid = len(regs); lab[s] = rid; mem = [s]; st = [s]
        cc = cen[s]; nn = N[s]; last = 1
        while st:
            f = st.pop()
            for g in adj[f]:
                if lab[g] >= 0 or N[g] @ nn < cosn or abs((cen[g] - cc) @ nn) > GROW_D or np.abs((V[F[g]] - cc) @ nn).max() > 2 * GROW_D:
                    continue
                lab[g] = rid; mem.append(g); st.append(g)
                if len(mem) >= last * 1.3 + 8:
                    m = np.array(mem); w = area[m]; cc = (cen[m] * w[:, None]).sum(0) / w.sum()
                    P = cen[m] - cc; ev, evec = np.linalg.eigh((P * w[:, None]).T @ P); n2 = evec[:, 0]
                    nn = n2 if n2 @ nn >= 0 else -n2; last = len(mem)
        regs.append(mem)
    return lab, regs


def fit_plane(V, F, mem, N, area):
    m = np.array(mem)
    P = V[np.unique(F[m])]
    c = P.mean(0)
    w, vec = np.linalg.eigh(np.cov((P - c).T))
    n = vec[:, 0]
    if n @ (N[m].T @ area[m]) < 0:
        n = -n
    return c, n, P


def main():
    t0 = time.time()
    # ---- unoptimised reference, moved to the SM_Turret frame
    uo = read_mesh(UN_FBX)
    VU, FU = arrays(uo.data)
    VU = VU + PIVOT_OFFSET_CM
    NU, AU = tri_normals(VU, FU)
    adjU = adjacency(VU, FU)
    total = AU.sum()
    lab0, regs0 = detect_regions(VU, FU, adjU, NU, AU)
    q3 = {"regions_all": len(regs0), "regions_kept": int(sum(1 for m in regs0 if AU[m].sum() >= MIN_PCT / 100 * total)),
          "kept_area_share_pct": round(float(sum(AU[m].sum() for m in regs0 if AU[m].sum() >= MIN_PCT / 100 * total) / total * 100), 2),
          "kept_normals_z": [round(float(NU[m].T @ AU[m] / AU[m].sum() @ np.array([0, 0, 1])), 3) for m in regs0 if AU[m].sum() >= MIN_PCT / 100 * total]}
    lab, regs = grow_planes(VU, FU, adjU, NU, AU)
    big = sorted([(AU[m].sum(), i) for i, m in enumerate(regs) if AU[m].sum() >= GROW_PCT / 100 * total], reverse=True)
    sens = {}
    for pct in (0.5, 0.3, 0.2, 0.1, 0.05):
        sens[str(pct)] = {"regions": int(sum(1 for m in regs if AU[m].sum() >= pct / 100 * total)),
                          "area_share_pct": round(float(sum(AU[m].sum() for m in regs if AU[m].sum() >= pct / 100 * total) / total * 100), 2)}
    print("unopt regions", len(regs), "kept", len(big), "t", round(time.time() - t0, 1))
    bvh = BVHTree.FromPolygons([tuple(v) for v in (VU / 100).tolist()], [tuple(f) for f in FU.tolist()])

    # ---- optimised game mesh (SM_Turret.fbx, task-346)
    ob = read_mesh(IN_FBX)
    me = ob.data
    V0, F0 = arrays(me)
    nv, nf = len(V0), len(F0)
    mi = np.empty(nf, int); me.polygons.foreach_get("material_index", mi)
    canon_f = mi == 1
    canon_v = np.zeros(nv, bool); canon_v[np.unique(F0[canon_f])] = True
    body_v_used = np.zeros(nv, bool); body_v_used[np.unique(F0[~canon_f])] = True
    shared_cb = int((canon_v & body_v_used).sum())
    NF, AF = tri_normals(V0, F0)
    tris_before = nf
    bbox0 = (V0.min(0), V0.max(0))
    height = float(bbox0[1][2] - bbox0[0][2])
    uv0 = np.empty(len(me.loops) * 2); me.uv_layers[0].uv.foreach_get("vector", uv0)
    V_orig = V0.copy()
    site = sites_of(V0)
    # corner normals (split normals as imported)
    cn = np.empty(len(me.loops) * 3); me.corner_normals.foreach_get("vector", cn); cn = cn.reshape(-1, 3)
    loop_start = np.empty(nf, int); me.polygons.foreach_get("loop_start", loop_start)

    # ---- map optimised faces to regions
    planes = {}
    for A_, i in big:
        c, n, P = fit_plane(VU, FU, regs[i], NU, AU)
        planes[i] = (c, n, P)
    face_reg = -np.ones(nf, int)
    BAR = [(1 / 3, 1 / 3, 1 / 3), (.7, .15, .15), (.15, .7, .15), (.15, .15, .7)]
    cosf = math.cos(math.radians(FACE_ANG))
    for f in np.nonzero(~canon_f)[0]:
        votes = {}
        for w in BAR:
            pt = (V0[F0[f]] * np.array(w)[:, None]).sum(0)
            loc, nor, idx, d = bvh.find_nearest(Vector(pt / 100))
            if idx is not None and lab[idx] in planes:
                votes[lab[idx]] = votes.get(lab[idx], 0) + 1
        for r, nvote in sorted(votes.items(), key=lambda kv: -kv[1]):
            if nvote < 2:
                break
            c, n, _ = planes[r]
            if np.abs((V0[F0[f]] - c) @ n).max() <= DIST and NF[f] @ n >= cosf:
                face_reg[f] = r
                break
    face_reg0 = face_reg.copy()

    # ---- cause measurement on the mapped faces (before any change)
    img = bpy.data.images.load(NORMAL_MAP)
    W, H = img.size
    px = np.empty(W * H * 4, np.float32); img.pixels.foreach_get(px); px = px.reshape(H, W, 4)
    me_uv = uv0.reshape(-1, 2)

    def tilt_at(f, w):
        ls = loop_start[f]
        uv = (me_uv[ls:ls + 3] * np.array(w)[:, None]).sum(0)
        x = min(W - 1, max(0, int(uv[0] * W))); y = min(H - 1, max(0, int(uv[1] * H)))
        v = px[y, x, :3] * 2 - 1
        return math.degrees(math.acos(max(-1, min(1, v[2] / max(np.linalg.norm(v), 1e-9)))))
    W4 = [(1 / 3, 1 / 3, 1 / 3), (.6, .2, .2), (.2, .6, .2), (.2, .2, .6)]
    cause = {}
    mapped = np.nonzero(face_reg >= 0)[0]
    body_f = np.nonzero(~canon_f)[0]
    rng = np.random.default_rng(1)
    sample_all = rng.choice(body_f, min(3000, len(body_f)), replace=False)

    def stats(arr):
        arr = np.array(arr)
        return {"mean": round(float(arr.mean()), 3), "p95": round(float(np.percentile(arr, 95)), 3), "max": round(float(arr.max()), 3)} if len(arr) else None
    # (a) vertex distance to fitted plane
    vd = []
    for f in mapped:
        c, n, _ = planes[face_reg[f]]
        vd += list(np.abs((V0[F0[f]] - c) @ n))
    # (b) split-normal deviation from the face normal
    nd = []
    for f in mapped:
        ls = loop_start[f]
        for k in range(3):
            cv = cn[ls + k]
            nd.append(math.degrees(math.acos(max(-1, min(1, float(cv @ NF[f]))))))
    # (c) normal-map tilt
    tm = [tilt_at(f, w) for f in mapped for w in W4]
    ta = [tilt_at(f, w) for f in sample_all for w in W4[:1]]
    # (d) face normal vs plane normal (geometric tilt of the facets)
    fa = [math.degrees(math.acos(max(-1, min(1, float(NF[f] @ planes[face_reg[f]][1]))))) for f in mapped]
    # slope of the whole mapped area for reference: unoptimised region residual
    cause = {
        "mapped_faces": int(len(mapped)),
        "vertex_distance_to_plane_cm": stats(vd),
        "face_normal_vs_plane_normal_deg": stats(fa),
        "split_normal_vs_face_normal_deg": stats(nd),
        "normal_map_tilt_deg_on_mapped_faces": stats(tm),
        "normal_map_tilt_deg_all_body_sample": stats(ta),
        "smooth_faces_before": int(sum(1 for p in me.polygons if p.use_smooth)),
    }
    print("CAUSE", json.dumps(cause))

    # ---- vertex targets. A vertex goes to the intersection of the planes of the mapped faces around it (minimum move).
    # A face whose vertices cannot all sit on its plane (conflicting planes, move above the silhouette limit, canon vertex)
    # is unmapped and stays as it is; repeat until every mapped face is exactly planar.
    RES_TOL = 0.005
    refused = []
    unmapped_by_reason = {"move_over_limit": 0, "conflicting_planes": 0, "canon_vertex": 0}
    for it in range(30):
        mapped = np.nonzero(face_reg >= 0)[0]
        reg_of_site = {}
        for f in mapped:
            for v in F0[f]:
                reg_of_site.setdefault(site[v], set()).add(int(face_reg[f]))
        site_target = {}
        bad_sites = {}
        for s_, rs in reg_of_site.items():
            idx = np.nonzero(site == s_)[0]
            p = V0[idx[0]]
            if canon_v[idx].any():
                bad_sites[s_] = "canon_vertex"
                continue
            rs = sorted(rs)
            Nm = np.array([planes[r][1] for r in rs]); d = np.array([planes[r][1] @ planes[r][0] for r in rs])
            x = p + np.linalg.pinv(Nm, rcond=1e-2) @ (d - Nm @ p)
            if np.abs(Nm @ x - d).max() > RES_TOL:
                bad_sites[s_] = "conflicting_planes"
                continue
            if np.linalg.norm(x - p) > MAX_MOVE:
                bad_sites[s_] = "move_over_limit"
                refused.append({"regions": rs, "move_cm": round(float(np.linalg.norm(x - p)), 3), "pos_cm": [round(float(t), 2) for t in p]})
                continue
            site_target[s_] = x
        if not bad_sites:
            break
        for f in mapped:
            for v in F0[f]:
                if site[v] in bad_sites:
                    unmapped_by_reason[bad_sites[site[v]]] += 1
                    face_reg[f] = -1
                    break
    mapped = np.nonzero(face_reg >= 0)[0]
    V1 = V0.copy()
    for s_, x in site_target.items():
        V1[site == s_] = x
    moved = np.linalg.norm(V1 - V0, axis=1)
    res = {}
    for r in planes:
        fs_ = mapped[face_reg[mapped] == r]
        if len(fs_) == 0:
            continue
        c, n, _ = planes[r]
        vs = np.unique(F0[fs_])
        d0 = np.abs((V0[vs] - c) @ n); d1 = np.abs((V1[vs] - c) @ n)
        res[r] = {"faces": int(len(fs_)), "vertices": int(len(vs)), "max_residual_before_cm": round(float(d0.max()), 4),
                  "max_residual_after_cm": round(float(d1.max()), 6), "vertices_moved": int((moved[vs] > 1e-6).sum()),
                  "max_move_cm": round(float(moved[vs].max()), 3)}
    flat_faces = mapped.copy()

    regions_report = []
    for A_, i in big:
        c, n, P = planes[i]
        du = np.abs((P - c) @ n)
        regions_report.append({"id": int(i), "area_cm2": round(float(A_), 1), "area_pct": round(float(A_ / total * 100), 3), "unopt_faces": len(regs[i]),
                               "centre_cm_sm_frame": [round(float(t), 2) for t in c], "normal": [round(float(t), 4) for t in n],
                               "unopt_vertex_max_dist_to_plane_cm": round(float(du.max()), 4), **res.get(i, {"faces": 0})})
    report = {
        "asset": "SM_Turret", "task": "task-347-01-turret-flat-surfaces",
        "cause_of_bulge": cause,
        "thresholds": {"q3_default_region_normal_tolerance_deg": ANG, "q3_default_min_area_pct_of_surface": MIN_PCT,
                       "used_grow_plane_distance_cm": GROW_D, "used_grow_normal_deg": GROW_A, "used_min_area_pct_of_surface": GROW_PCT, "map_max_vertex_distance_cm": DIST,
                       "map_max_face_normal_deg": FACE_ANG, "max_vertex_move_cm": MAX_MOVE, "silhouette_limit_1pct_height_cm": round(height * 0.01, 3),
                       "note": "provisional, owner Q3 2026-10-28"},
        "unoptimised": {"surface_cm2": round(float(total), 1), "faces": len(FU), "q3_default_detection": q3, "regions_all": len(regs), "regions_kept": len(big),
                        "kept_area_share_pct": round(float(sum(a for a, _ in big) / total * 100), 2), "sensitivity_min_area": sens},
        "regions": regions_report,
        "faces_mapped_first_pass": int((face_reg0 >= 0).sum()), "faces_unmapped_in_loop_by_reason": unmapped_by_reason, "refused_vertices": refused,
        "vertices_moved": int((moved > 1e-6).sum()), "sites_moved": len(site_target),
        "max_move_cm": round(float(moved.max()), 3),
        "canon_vertices_shared_with_body": shared_cb,
        "faces_flat_shaded": int(len(flat_faces)),
        "warnings": warnings,
    }
    if MEASURE_ONLY:
        print(json.dumps(report, indent=1))
        return

    # ---- apply: positions, flat split normals on mapped faces
    co = (V1 / 100.0).astype(np.float64)
    me.vertices.foreach_set("co", co.ravel())
    me.update()
    cn2 = cn.copy()
    newN, _ = tri_normals(V1, F0)
    for f in flat_faces:
        cn2[loop_start[f]:loop_start[f] + 3] = newN[f]
    me.normals_split_custom_set(cn2.tolist())
    for f in flat_faces:
        me.polygons[int(f)].use_smooth = False
    me.update()

    bpy.ops.object.select_all(action="DESELECT")
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    os.makedirs(OUT, exist_ok=True)
    fbx = os.path.join(OUT, "SM_Turret.fbx")
    bpy.ops.export_scene.fbx(filepath=fbx, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                             mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="COPY", embed_textures=False)

    # ---- reimport check
    ob2 = read_mesh(fbx)
    m2 = ob2.data
    V2, F2 = arrays(m2)
    mi2 = np.empty(len(F2), int); m2.polygons.foreach_get("material_index", mi2)
    uv2 = np.empty(len(m2.loops) * 2); m2.uv_layers[0].uv.foreach_get("vector", uv2)
    m2.calc_loop_triangles()
    tris = len(m2.loop_triangles)
    cv0 = np.unique(F0[canon_f]); cv2 = np.unique(F2[mi2 == 1])
    same_topo = bool(np.array_equal(F0, F2) and np.array_equal(mi, mi2))
    canon_diff = float(np.abs(V2[cv2] - V_orig[cv0]).max()) if same_topo else None
    free = ~np.isin(np.arange(nv), np.nonzero(moved > 1e-6)[0])
    still_diff = float(np.abs(V2[free] - V_orig[free]).max()) if same_topo else None
    # residuals after reimport
    res2 = []
    for r in planes:
        fs_ = mapped[face_reg[mapped] == r]
        if len(fs_) == 0:
            continue
        c, n, _ = planes[r]
        vs = np.unique(F0[fs_])
        res2.append(float(np.abs((V2[vs] - c) @ n).max()))
    mn2, mx2 = V2.min(0), V2.max(0)
    sm = np.array([p.use_smooth for p in m2.polygons])
    report["output"] = {
        "file": "Art/Meshes/turret/SM_Turret.fbx", "tris": tris, "tris_before": tris_before, "budget": BUDGET,
        "dimensions_cm": [round(float(t), 2) for t in (mx2 - mn2)], "dimensions_before_cm": [round(float(t), 2) for t in (bbox0[1] - bbox0[0])],
        "bbox_min_cm": [round(float(t), 3) for t in mn2], "bbox_max_cm": [round(float(t), 3) for t in mx2],
        "bbox_max_change_cm": round(float(max(np.abs(mn2 - bbox0[0]).max(), np.abs(mx2 - bbox0[1]).max())), 4), "bbox_tolerance_1pct_height_cm": round(height * 0.01, 3),
        "pivot_bottom_centre": bool(abs((mn2[0] + mx2[0]) / 2) < 0.05 and abs((mn2[1] + mx2[1]) / 2) < 0.05 and abs(mn2[2]) < 0.05),
        "materials": [m.name for m in m2.materials], "uv_layers": [u.name for u in m2.uv_layers],
        "uv_max_abs_diff": round(float(np.abs(uv2 - uv0).max()), 8) if len(uv2) == len(uv0) else "loop count differs",
        "max_residual_to_plane_after_reimport_cm": round(max(res2), 6) if res2 else None,
        "canon_identity": {"same_topology_and_material_index": same_topo, "canon_vertices": int(len(cv0)), "max_position_diff_cm": canon_diff,
                           "unmoved_vertices_max_diff_cm": still_diff},
        "faces_smooth_after": int(sm.sum()), "faces_flat_after": int((~sm).sum()),
    }
    if tris >= BUDGET:
        warnings.append("tris %d not under budget %d" % (tris, BUDGET))
    with open(os.path.join(OUT, "turret_flat.report.json"), "w", encoding="utf-8") as fh:
        json.dump(report, fh, indent=2)
    print("DONE tris", tris, "moved", report["vertices_moved"], "canon diff", canon_diff, "max res", report["output"]["max_residual_to_plane_after_reimport_cm"])


if __name__ == "__main__":
    try:
        main()
    except Exception:
        import traceback
        traceback.print_exc()
        sys.exit(1)
