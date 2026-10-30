"""Burst rifle (Meshy Aqua Wasteland Carbine): scale, orient, pivot and flatten the planar regions (task-394-02), headless Blender 5.2.

Usage (repository root):
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/burst_rifle_flatten.py -- [--raw C:/rrb/raw.fbx] [--length 60] [--grow-dist-cm 0.075]
Options: --grow-ang 10 --grow-min-area-pct 0.05 --max-move-cm 0.25 --muzzle -x --grip-cut-z -16 (raw cm)

The raw FBX (staged on a short path, byte copy of Art/Source/burst-rifle/meshy/burst_rifle_meshy_raw.fbx) is not decimated (Q3: 9,892 tris
kept). Planar regions are found on the mesh itself (face centre within the plane distance of the running least-squares plane, normal within
--grow-ang, area >= --grow-min-area-pct of the surface; the extractor recipe, task-366), every region vertex is projected on its plane (a
vertex shared by several regions goes to their intersection; a move above the limit or conflicting planes unmap the faces), flat shading
on those faces. The mesh is rotated about Z so the muzzle points to +X, scaled to --length cm stock to muzzle, and its origin set at the
pistol grip hand hold. Exports Art/Meshes/burst-rifle/SM_BurstRifle.fbx (cm, FBX_SCALE_UNITS), reimports it in a clean scene and writes
burst_rifle_flat.report.json. Texture files are byte-copied, never edited.
"""
import json
import math
import os
import shutil
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


RAW = opt("raw", "C:/rrb/raw.fbx", str)
SRC_MAPS = os.path.join(ROOT, "Art/Source/burst-rifle/meshy")
OUTDIR = os.path.join(ROOT, "Art/Meshes/burst-rifle")
LENGTH = opt("length", 60.0)
GROW_OUT = opt("grow-dist-cm", 0.075)          # plane distance in output cm (Q5)
GROW_A = opt("grow-ang", 10.0)
GROW_PCT = opt("grow-min-area-pct", 0.05)
MAX_MOVE_OUT = opt("max-move-cm", 0.25)        # in output cm
MUZZLE = opt("muzzle", "-x", str)              # raw axis the muzzle points to (found on the mesh: the thin barrel end is at -X)
GRIP_CUT_Z = opt("grip-cut-z", -16.0)          # raw cm: below the trigger guard bottom (-14.5), the hand closes under it
BUDGET = 9892
t0 = time.time()


def log(*a):
    print("[%6.1fs]" % (time.time() - t0), *a, flush=True)


def arrays(me):
    nv, nf = len(me.vertices), len(me.polygons)
    V = np.empty(nv * 3); me.vertices.foreach_get("co", V); V = V.reshape(-1, 3) * 100.0   # FBX import: metres, work in cm
    F = np.empty(nf * 3, np.int64); me.polygons.foreach_get("vertices", F); F = F.reshape(-1, 3)
    return V, F


def tri_normals(V, F):
    a, b, c = V[F[:, 0]], V[F[:, 1]], V[F[:, 2]]
    cr = np.cross(b - a, c - a)
    ln = np.linalg.norm(cr, axis=1)
    return cr / np.maximum(ln[:, None], 1e-12), ln / 2


def sites_of(V):
    _, s = np.unique(np.round(V, 3), axis=0, return_inverse=True)
    return s.ravel()


def neighbours(S, nf):
    e = np.concatenate([S[:, [0, 1]], S[:, [1, 2]], S[:, [2, 0]]])
    k = np.sort(e, axis=1)
    key = k[:, 0].astype(np.int64) * (int(S.max()) + 1) + k[:, 1]
    o = np.argsort(key, kind="stable"); ks = key[o]
    nbr = -np.ones((nf, 3), np.int64)
    pair = np.nonzero(ks[1:] == ks[:-1])[0]
    a, b = o[pair], o[pair + 1]
    nbr[a % nf, a // nf] = b % nf
    nbr[b % nf, b // nf] = a % nf
    return nbr


def stats(arr):
    arr = np.array(arr)
    return {"mean": round(float(arr.mean()), 4), "p95": round(float(np.percentile(arr, 95)), 4), "max": round(float(arr.max()), 4)} if len(arr) else None


def build_material(name, outdir):
    maps = {"BaseColor": "burst_rifle_meshy_basecolor.png", "Metallic": "burst_rifle_meshy_metallic.png",
            "Normal": "burst_rifle_meshy_normal.png", "Roughness": "burst_rifle_meshy_roughness.png"}
    paths = {}
    for k, fn in maps.items():
        dst = os.path.join(outdir, "T_BurstRifle_%s.png" % k)
        shutil.copyfile(os.path.join(SRC_MAPS, fn), dst)   # byte copy: textures are not edited
        paths[k] = dst
    mat = bpy.data.materials.new(name); mat.use_nodes = True
    nt = mat.node_tree; bsdf = nt.nodes["Principled BSDF"]

    def img(k, cs):
        n = nt.nodes.new("ShaderNodeTexImage"); n.image = bpy.data.images.load(paths[k]); n.image.colorspace_settings.name = cs; return n
    nt.links.new(img("BaseColor", "sRGB").outputs["Color"], bsdf.inputs["Base Color"])
    nt.links.new(img("Metallic", "Non-Color").outputs["Color"], bsdf.inputs["Metallic"])
    nt.links.new(img("Roughness", "Non-Color").outputs["Color"], bsdf.inputs["Roughness"])
    nm = nt.nodes.new("ShaderNodeNormalMap"); nt.links.new(img("Normal", "Non-Color").outputs["Color"], nm.inputs["Color"])
    nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    return mat, paths


def main():
    from mathutils import Vector
    from mathutils.bvhtree import BVHTree
    ob = rr.import_fbx(RAW)[0]
    me = ob.data
    V0, F0 = arrays(me)
    nv, nf = len(V0), len(F0)
    uv0 = np.empty(len(me.loops) * 2); me.uv_layers[0].uv.foreach_get("vector", uv0)
    n0 = rr.tri_count(me)
    NF, AF = tri_normals(V0, F0)
    total = float(AF.sum())
    site = sites_of(V0)
    log("imported", n0, "tris", nv, "verts, dims", (V0.max(0) - V0.min(0)).round(2))

    # ---- orientation, scale (computed first: thresholds are given in output cm)
    sgn = 1.0 if MUZZLE[0] == "+" else -1.0
    ax = "xyz".index(MUZZLE[1])
    ang = (0.0 if sgn > 0 else 180.0) if ax == 0 else (-90.0 if sgn > 0 else 90.0)
    ca, sa = math.cos(math.radians(ang)), math.sin(math.radians(ang))
    R = np.array([[ca, -sa, 0], [sa, ca, 0], [0, 0, 1.0]])
    ext0 = (V0 @ R.T).max(0) - (V0 @ R.T).min(0)
    scale = LENGTH / ext0[0]
    GROW_D = GROW_OUT / scale
    MAX_MOVE = MAX_MOVE_OUT / scale
    log("scale", round(scale, 5), "raw grow dist cm", round(GROW_D, 4), "raw max move", round(MAX_MOVE, 4))

    # ---- planar regions (growing, extractor recipe)
    cen = V0[F0].mean(1)
    nbr = neighbours(site[F0], nf)
    cosn = math.cos(math.radians(GROW_A))
    lab = -np.ones(nf, np.int64)
    planes = {}
    area_min = GROW_PCT / 100 * total
    n_all = 0
    for s in np.argsort(-AF, kind="stable").tolist():
        if lab[s] != -1:
            continue
        rid = n_all; n_all += 1
        lab[s] = rid; mem = [s]; st = [s]
        cc = cen[s].copy(); q = NF[s].copy(); last = 1
        while st:
            f = st.pop()
            for k in range(3):
                g = nbr[f, k]
                if g < 0 or lab[g] != -1:
                    continue
                if NF[g] @ q < cosn:
                    continue
                if abs((cen[g] - cc) @ q) > GROW_D:
                    continue
                if np.abs((V0[F0[g]] - cc) @ q).max() > 2 * GROW_D:
                    continue
                lab[g] = rid; mem.append(g); st.append(g)
                if len(mem) >= last * 1.3 + 8:
                    m = np.array(mem); w = AF[m]; c2 = (cen[m] * w[:, None]).sum(0) / w.sum()
                    P = cen[m] - c2; ev, evec = np.linalg.eigh((P * w[:, None]).T @ P); n2 = evec[:, 0]
                    q = n2 if n2 @ q >= 0 else -n2
                    cc = c2; last = len(mem)
        m = np.array(mem); a_reg = float(AF[m].sum())
        if a_reg >= area_min:
            P = V0[np.unique(F0[m])]; c = P.mean(0)
            _, vec = np.linalg.eigh(np.cov((P - c).T)); n = vec[:, 0]
            if n @ (NF[m].T @ AF[m]) < 0:
                n = -n
            planes[rid] = (c, n, a_reg, len(m), float(np.abs((P - c) @ n).max()))
        else:
            lab[m] = -2
    face_reg = np.where(np.isin(lab, list(planes.keys())), lab, -1)
    face_reg0 = face_reg.copy()
    log("regions all", n_all, "kept", len(planes), "faces in kept regions", int((face_reg >= 0).sum()), "of", nf)

    # ---- cause of the bulge, measured on the region faces before any change
    cn = np.empty(len(me.loops) * 3); me.corner_normals.foreach_get("vector", cn); cn = cn.reshape(-1, 3)
    loop_start = np.empty(nf, int); me.polygons.foreach_get("loop_start", loop_start)
    mapped = np.nonzero(face_reg >= 0)[0]
    vd, fa_, nd = [], [], []
    for f in mapped:
        c, n = planes[face_reg[f]][:2]
        vd += list(np.abs((V0[F0[f]] - c) @ n))
        fa_.append(math.degrees(math.acos(max(-1, min(1, float(NF[f] @ n))))))
        for k in range(3):
            nd.append(math.degrees(math.acos(max(-1, min(1, float(cn[loop_start[f] + k] @ NF[f]))))))
    nmap = bpy.data.images.load(os.path.join(SRC_MAPS, "burst_rifle_meshy_normal.png"))
    W, H = nmap.size
    px = np.empty(W * H * 4, np.float32); nmap.pixels.foreach_get(px); px = px.reshape(H, W, 4)
    uvl = uv0.reshape(-1, 2)

    def tilt(f):
        ls = loop_start[f]; uv = uvl[ls:ls + 3].mean(0)
        v = px[min(H - 1, max(0, int(uv[1] * H))), min(W - 1, max(0, int(uv[0] * W))), :3] * 2 - 1
        return math.degrees(math.acos(max(-1, min(1, v[2] / max(np.linalg.norm(v), 1e-9)))))
    rng = np.random.default_rng(1)
    samp_all = rng.choice(nf, min(3000, nf), replace=False)
    samp_map = rng.choice(mapped, min(3000, len(mapped)), replace=False) if len(mapped) else []
    cause = {"mapped_faces": int(len(mapped)), "units": "raw cm for distances (output = raw x scale)",
             "vertex_distance_to_plane_cm": stats(vd), "face_normal_vs_plane_normal_deg": stats(fa_),
             "split_normal_vs_face_normal_deg": stats(nd),
             "normal_map_tilt_deg_on_region_faces_sample": stats([tilt(f) for f in samp_map]),
             "normal_map_tilt_deg_all_faces_sample": stats([tilt(f) for f in samp_all]),
             "smooth_faces_before": int(sum(1 for p in me.polygons if p.use_smooth)), "faces_total": int(nf)}
    log("CAUSE", json.dumps(cause))

    # ---- vertex targets
    RES_TOL = 0.005
    refused = []
    unm = {"move_over_limit": 0, "conflicting_planes": 0}
    first = {}
    for i in range(nv):
        first.setdefault(int(site[i]), i)
    site_target = {}
    for it in range(40):
        mapped = np.nonzero(face_reg >= 0)[0]
        reg_of_site = {}
        for f in mapped:
            for v in F0[f]:
                reg_of_site.setdefault(int(site[v]), set()).add(int(face_reg[f]))
        site_target = {}; bad = {}
        for s_, rs in reg_of_site.items():
            p = V0[first[s_]]
            rs = sorted(rs)
            Nm = np.array([planes[r][1] for r in rs]); d = np.array([planes[r][1] @ planes[r][0] for r in rs])
            x = p + np.linalg.pinv(Nm, rcond=1e-2) @ (d - Nm @ p)
            if np.abs(Nm @ x - d).max() > RES_TOL:
                bad[s_] = "conflicting_planes"; continue
            if np.linalg.norm(x - p) > MAX_MOVE:
                bad[s_] = "move_over_limit"; refused.append(round(float(np.linalg.norm(x - p)) * scale, 3)); continue
            site_target[s_] = x
        if not bad:
            break
        for f in mapped:
            for v in F0[f]:
                if int(site[v]) in bad:
                    unm[bad[int(site[v])]] += 1; face_reg[f] = -1; break
    mapped = np.nonzero(face_reg >= 0)[0]
    V1 = V0.copy()
    for s_, x in site_target.items():
        V1[site == s_] = x
    moved = np.linalg.norm(V1 - V0, axis=1)
    res_before, res_after = {}, {}
    for r in planes:
        fs_ = mapped[face_reg[mapped] == r]
        if len(fs_) == 0:
            continue
        c, n = planes[r][:2]; vs = np.unique(F0[fs_])
        res_before[r] = float(np.abs((V0[vs] - c) @ n).max()); res_after[r] = float(np.abs((V1[vs] - c) @ n).max())
    log("flatten solved: mapped", len(mapped), "moved", int((moved > 1e-6).sum()), "regions used", len(res_after), "unmapped", unm)

    # ---- grip hand hold (raw frame): lowest vertex gives the grip x; hand hold = bbox centre of the vertices under the trigger guard around it
    Vr0 = V1 @ R.T
    low = Vr0[np.argmin(Vr0[:, 2])]
    cutz = GRIP_CUT_Z
    gsel = (Vr0[:, 2] <= cutz) & (np.abs(Vr0[:, 0] - low[0]) < 14.0)
    G = Vr0[gsel]
    pivot = np.array([(G[:, 0].min() + G[:, 0].max()) / 2, (G[:, 1].min() + G[:, 1].max()) / 2, (G[:, 2].min() + G[:, 2].max()) / 2])
    log("grip: lowest vertex", low.round(2), "grip vertices", int(gsel.sum()), "bbox", G.min(0).round(2), G.max(0).round(2), "pivot (raw rot frame)", pivot.round(2))

    def T(P):
        return ((P @ R.T) - pivot) * scale   # raw frame -> SM_BurstRifle frame (cm)
    V2 = T(V1)
    log("placed: rot", ang, "scale", round(scale, 5), "dims", (V2.max(0) - V2.min(0)).round(2))

    # ---- write geometry, flat shading on mapped faces, material
    me.vertices.foreach_set("co", (V2 / 100.0).ravel()); me.update()
    for p in me.polygons:
        p.use_smooth = True
    for f in mapped:
        me.polygons[int(f)].use_smooth = False
    me.update()
    os.makedirs(OUTDIR, exist_ok=True)
    mat, paths = build_material("M_BurstRifle", OUTDIR)
    me.materials.clear(); me.materials.append(mat)
    ob.name = "SM_BurstRifle"; me.name = "SM_BurstRifle"
    fbx = os.path.join(OUTDIR, "SM_BurstRifle.fbx")
    rr.export_fbx(fbx, [ob])
    log("exported", fbx)
    shutil.rmtree(os.path.join(OUTDIR, "SM_BurstRifle.fbm"), ignore_errors=True)   # path_mode COPY duplicates the maps there; the FBX keeps the maps next to it

    # ---- reimport check in a clean scene
    ob2 = rr.import_fbx(fbx)[0]
    m2 = ob2.data
    shutil.rmtree(os.path.join(OUTDIR, "SM_BurstRifle.fbm"), ignore_errors=True)
    Vc, Fc = arrays(m2)
    uv2 = np.empty(len(m2.loops) * 2); m2.uv_layers[0].uv.foreach_get("vector", uv2)
    mn2, mx2 = Vc.min(0), Vc.max(0)
    tris = rr.tri_count(m2)
    sm2 = np.array([p.use_smooth for p in m2.polygons])
    same_topo = bool(np.array_equal(Fc, F0))
    res2 = {}
    if same_topo:
        for r in res_after:
            c, n = planes[r][:2]; c2 = T(c[None])[0]; n2 = (n @ R.T)
            vs = np.unique(F0[mapped[face_reg[mapped] == r]])
            res2[r] = float(np.abs((Vc[vs] - c2) @ n2).max())
    # silhouette against the scaled source
    bvh_src = BVHTree.FromPolygons([tuple(v) for v in V0.tolist()], [tuple(f) for f in F0.tolist()])
    bvh_out = BVHTree.FromPolygons([tuple(v) for v in Vc.tolist()], [tuple(f) for f in Fc.tolist()])
    Vraw = (Vc / scale + pivot) @ R   # output vertices back to the raw frame
    d_out = [bvh_src.find_nearest(Vector(v))[3] * scale for v in Vraw.tolist()]
    VDs = T(V0)
    d_src = [bvh_out.find_nearest(Vector(v))[3] for v in VDs.tolist()]
    length_out = float(mx2[0] - mn2[0]); tol = 0.01 * length_out
    bbox_diff = float(max(np.abs(mn2 - VDs.min(0)).max(), np.abs(mx2 - VDs.max(0)).max()))
    dim_src = VDs.max(0) - VDs.min(0)
    regions_report = []
    for r, (c, n, a, nfc, md) in sorted(planes.items(), key=lambda kv: -kv[1][2]):
        regions_report.append({"id": r, "area_cm2_raw": round(a, 1), "area_pct": round(a / total * 100, 3), "faces": nfc,
                               "centre_cm_sm_frame": [round(float(t), 2) for t in T(c[None])[0]], "normal_sm_frame": [round(float(t), 4) for t in (n @ R.T)],
                               "vertex_max_dist_to_plane_raw_cm": round(md, 4),
                               "max_residual_before_out_cm": round(res_before[r] * scale, 4) if r in res_before else None,
                               "max_residual_after_cm": round(res2[r], 6) if r in res2 else None,
                               "faces_flat": int((face_reg[mapped] == r).sum())})
    report = {
        "asset": "SM_BurstRifle", "task": "task-394-02-burst-rifle-flatten", "source": "Art/Source/burst-rifle/meshy/burst_rifle_meshy_raw.fbx",
        "cause_of_bulge": cause,
        "thresholds": {"plane_distance_cm_output": GROW_OUT, "plane_distance_cm_raw": round(GROW_D, 4), "normal_deg": GROW_A, "min_area_pct_of_surface": GROW_PCT,
                       "max_vertex_move_cm_output": MAX_MOVE_OUT, "note": "Q5 provisional, owner 2026-10-30"},
        "regions_all": n_all, "regions_kept": len(planes), "kept_area_share_pct": round(float(sum(v[2] for v in planes.values()) / total * 100), 2),
        "surface_cm2_raw": round(total, 1),
        "triangles_before": int(n0), "triangles_after": int(tris), "budget": BUDGET,
        "faces_in_kept_regions": int((face_reg0 >= 0).sum()), "faces_unmapped_in_loop_by_reason": unm, "refused_move_cm_output": refused[:20],
        "vertices_moved": int((moved > 1e-6).sum()), "max_move_cm_output": round(float(moved.max() * scale), 4),
        "regions_used": len(res_after), "faces_flat_shaded": int(len(mapped)),
        "max_residual_region_cm_after_reimport": round(max(res2.values()), 6) if res2 else None,
        "max_residual_region_cm_before_output": round(max(res_before.values()) * scale, 4) if res_before else None,
        "regions": regions_report,
        "placement": {"muzzle_axis_raw": MUZZLE, "muzzle_rule": "thin barrel end with muzzle brake and front sight at raw -X, stock (tall butt plate) at raw +X; top already +Z (sight on top)",
                      "rotation_about_z_deg": ang, "uniform_scale": round(float(scale), 6), "length_cm_target": LENGTH,
                      "bounds_cm": [round(float(t), 3) for t in (mx2 - mn2)], "min_cm": [round(float(t), 3) for t in mn2], "max_cm": [round(float(t), 3) for t in mx2],
                      "source_dims_raw_cm": [round(float(t), 2) for t in (V0.max(0) - V0.min(0))], "source_dims_scaled_cm": [round(float(t), 3) for t in dim_src],
                      "grip_point": {"rule": "lowest vertex gives the grip; vertices with z <= %.1f raw cm (under the trigger guard) within 14 raw cm of its x; hand hold = bbox centre of that set" % GRIP_CUT_Z,
                                     "lowest_vertex_raw_rot_frame": [round(float(t), 2) for t in low], "grip_vertices": int(gsel.sum()),
                                     "grip_bbox_raw_rot_frame": [[round(float(t), 2) for t in G.min(0)], [round(float(t), 2) for t in G.max(0)]],
                                     "pivot_raw_rot_frame": [round(float(t), 2) for t in pivot],
                                     "grip_bbox_cm_sm_frame": [[round(float(t), 2) for t in T(Vr0[gsel].min(0)[None] @ R)[0]], [round(float(t), 2) for t in T(Vr0[gsel].max(0)[None] @ R)[0]]]},
                      "origin_check_in_output_frame": "origin = grip hand hold; muzzle +X, top +Z"},
        "silhouette": {"length_cm": round(length_out, 3), "tolerance_1pct_cm": round(tol, 3), "bbox_max_change_vs_scaled_source_cm": round(bbox_diff, 4),
                       "output_vertex_to_source_surface_cm": stats(d_out), "source_vertex_to_output_surface_cm": stats(d_src),
                       "max_vertex_displacement_cm": round(float(moved.max() * scale), 4)},
        "uv": {"layers": [u.name for u in m2.uv_layers], "uv0_max_abs_diff_vs_source": round(float(np.abs(uv2 - uv0).max()), 8) if len(uv2) == len(uv0) else "loop count differs",
               "uv0_loops_outside_0_1": int(((uv2 < -1e-4) | (uv2 > 1 + 1e-4)).sum()), "faces_with_uv": int(len(m2.polygons))},
        "materials": [m.name for m in m2.materials], "material_slots": len(m2.materials),
        "texture_files": {k: os.path.relpath(v, ROOT).replace("\\", "/") for k, v in paths.items()},
        "faces_smooth_after": int(sm2.sum()), "faces_flat_after": int((~sm2).sum()),
        "reimport_same_topology": same_topo, "warnings": [],
    }
    w = report["warnings"]
    if tris > BUDGET: w.append("tris %d over budget %d" % (tris, BUDGET))
    if res2 and max(res2.values()) >= 0.01: w.append("residual >= 0.01 cm")
    if abs(length_out - LENGTH) > 0.1: w.append("length off")
    if max(d_out) > tol or max(d_src) > tol: w.append("silhouette error above 1 pct of length")
    if report["faces_flat_after"] != len(mapped): w.append("flat face count differs after reimport")
    with open(os.path.join(OUTDIR, "burst_rifle_flat.report.json"), "w", encoding="utf-8") as fh:
        json.dump(report, fh, indent=2)
    log("DONE", json.dumps({k: report[k] for k in ("triangles_after", "regions_used", "faces_flat_shaded", "max_residual_region_cm_after_reimport", "warnings")}), report["placement"]["bounds_cm"])


if __name__ == "__main__":
    try:
        main()
    except Exception:
        import traceback
        traceback.print_exc()
        sys.exit(1)
