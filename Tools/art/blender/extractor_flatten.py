"""Extractor (Rustwing quadrotor): decimate the Meshy mesh to budget and flatten its flat faces (task-366-02), headless Blender 5.2.

Usage (repository root), two phases so the 2.6 M triangle mesh is loaded once:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/extractor_flatten.py -- --phase dense --work <dir>
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/extractor_flatten.py -- --phase game  --work <dir> [--nose-axis +y]
Options: --grow-dist 0.25 --grow-ang 10 --grow-min-area-pct 0.05 --dist 1.5 --face-ang 20 --max-move 0.8 --target-tris 47000

dense: imports the raw Meshy FBX, decimates it (collapse, UVs kept) to the target, writes <work>/decimated.fbx, then finds planar
       regions on the DENSE mesh (face centre within --grow-dist cm of the running least-squares plane, normal within --grow-ang
       degrees, area >= --grow-min-area-pct of the surface; task-347 thresholds) and writes <work>/dense.npz (dense mesh, labels, planes).
game:  maps each decimated face to a dense region (barycentric nearest-surface votes, vertex distance, normal), projects the vertices
       on the region planes (vertex shared by several regions goes to the intersection), flat shading on those faces, rotates /
       scales / places the mesh (Q5), builds the material from the four maps, exports Art/Meshes/extractor/SM_Extractor.fbx
       (cm, FBX_SCALE_UNITS), reimports it, and writes extractor_flat.report.json.
"""
import json
import math
import os
import shutil
import sys
import time
from array import array

import bpy
import numpy as np

sys.path.insert(0, os.path.join(os.getcwd(), "Tools", "art", "blender"))
import rr_blender as rr  # noqa: E402

ROOT = os.getcwd()
A = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []


def opt(name, default, cast=float):
    return cast(A[A.index("--" + name) + 1]) if ("--" + name) in A else default


PHASE = opt("phase", "game", str)
WORK = opt("work", os.path.join(ROOT, "Saved", "ExtractorMesh", "flat", "work"), str)
RAW = os.path.join(ROOT, "Art/Meshes/extractor/raw/Meshy_AI_Rustwing_Quadrotor_20261024_texture.fbx")
SRC_MAPS = os.path.join(ROOT, "Art/Source/extractor-meshy")
OUTDIR = os.path.join(ROOT, "Art/Meshes/extractor")
GROW_D, GROW_A, GROW_PCT = opt("grow-dist", 0.25), opt("grow-ang", 10.0), opt("grow-min-area-pct", 0.05)
DIST, FACE_ANG, MAX_MOVE = opt("dist", 1.5), opt("face-ang", 20.0), opt("max-move", 0.8)
TARGET = opt("target-tris", 47000, int)
BUDGET = 48000
FOOT = (268.0, 201.0)   # shuttle landing footprint 4 x 3 cells, cm (extraction node, Q5)
os.makedirs(WORK, exist_ok=True)
t0 = time.time()


def log(*a):
    print("[%6.1fs]" % (time.time() - t0), *a, flush=True)


def arrays(me):
    nv, nf = len(me.vertices), len(me.polygons)
    V = np.empty(nv * 3); me.vertices.foreach_get("co", V); V = V.reshape(-1, 3) * 100.0   # scene is in metres (FBX import): work in cm
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
    """(nf, 3) face neighbour across edge k (edge k = vertex k to k+1), -1 when none"""
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


# ------------------------------------------------------------------------------------------------ phase dense
def phase_dense():
    obj = rr.import_fbx(RAW)[0]
    V, F = arrays(obj.data)
    log("raw imported", len(F), "tris", len(V), "verts, bbox", (V.max(0) - V.min(0)).round(2))
    np.savez(os.path.join(WORK, "dense_mesh.npz"), V=V.astype(np.float32), F=F.astype(np.int32))
    del obj
    n0 = len(F)

    # ---- planar regions on the dense mesh
    N, ar = tri_normals(V, F)
    cen = V[F].mean(1)
    total = float(ar.sum())
    S = sites_of(V)[F]
    nbr = neighbours(S, len(F))
    log("adjacency done; surface cm2", round(total, 1), "unmatched edges", int((nbr < 0).sum()))
    nf = len(F)
    cx, cy, cz = (array("d", cen[:, i].tobytes()) for i in range(3))
    nx, ny, nz = (array("d", N[:, i].tobytes()) for i in range(3))
    vx, vy, vz = (array("d", V[:, i].tobytes()) for i in range(3))
    f0, f1, f2 = (array("q", F[:, i].tobytes()) for i in range(3))
    nb = [array("q", nbr[:, i].tobytes()) for i in range(3)]
    cosn = math.cos(math.radians(GROW_A))
    lab = -np.ones(nf, np.int32)
    labl = array("i", lab.tobytes())
    planes = []
    area_min = GROW_PCT / 100 * total
    order = np.argsort(-ar, kind="stable").tolist()
    n_regions_all = 0
    for s in order:
        if labl[s] != -1:
            continue
        rid = n_regions_all; n_regions_all += 1
        labl[s] = rid; mem = [s]; st = [s]
        ccx, ccy, ccz = cx[s], cy[s], cz[s]; qx, qy, qz = nx[s], ny[s], nz[s]; last = 1
        while st:
            f = st.pop()
            for k in range(3):
                g = nb[k][f]
                if g < 0 or labl[g] != -1:
                    continue
                if nx[g] * qx + ny[g] * qy + nz[g] * qz < cosn:
                    continue
                if abs((cx[g] - ccx) * qx + (cy[g] - ccy) * qy + (cz[g] - ccz) * qz) > GROW_D:
                    continue
                bad = False
                for vi in (f0[g], f1[g], f2[g]):
                    if abs((vx[vi] - ccx) * qx + (vy[vi] - ccy) * qy + (vz[vi] - ccz) * qz) > 2 * GROW_D:
                        bad = True; break
                if bad:
                    continue
                labl[g] = rid; mem.append(g); st.append(g)
                if len(mem) >= last * 1.3 + 8:
                    m = np.array(mem); w = ar[m]; cc = (cen[m] * w[:, None]).sum(0) / w.sum()
                    P = cen[m] - cc; ev, evec = np.linalg.eigh((P * w[:, None]).T @ P); n2 = evec[:, 0]
                    nn = np.array([qx, qy, qz]); nn = n2 if n2 @ nn >= 0 else -n2
                    ccx, ccy, ccz = cc; qx, qy, qz = nn; last = len(mem)
        m = np.array(mem); a_reg = float(ar[m].sum())
        if a_reg >= area_min:
            P = V[np.unique(F[m])]; c = P.mean(0)
            _, vec = np.linalg.eigh(np.cov((P - c).T)); n = vec[:, 0]
            if n @ (N[m].T @ ar[m]) < 0:
                n = -n
            planes.append((rid, c, n, a_reg, len(m), float(np.abs((P - c) @ n).max())))
        else:
            for g in mem:
                labl[g] = -2
    lab = np.frombuffer(labl, dtype=np.int32).copy()
    log("regions all", n_regions_all, "kept", len(planes))
    sens = {}
    # area sensitivity: reconstruct region areas of the labelled faces only for kept regions
    np.savez(os.path.join(WORK, "dense.npz"), lab=lab, plane_id=np.array([p[0] for p in planes]),
             plane_c=np.array([p[1] for p in planes]), plane_n=np.array([p[2] for p in planes]),
             plane_area=np.array([p[3] for p in planes]), plane_faces=np.array([p[4] for p in planes]),
             plane_maxdist=np.array([p[5] for p in planes]), total=total, regions_all=n_regions_all)

    # ---- pre-flatten the dense mesh: region vertices go onto their planes (a vertex shared by several regions to the intersection),
    # so the decimation below meets exactly planar panels. A move above PRE_MOVE cm is refused (vertex stays).
    PRE_MOVE = opt("pre-move", 0.8)
    in_reg = np.isin(lab, [p[0] for p in planes])
    pairs = np.unique(np.stack([F[in_reg].ravel(), np.repeat(lab[in_reg], 3)], 1), axis=0)
    pv, pr = pairs[:, 0], pairs[:, 1]
    pid_of = {p[0]: i for i, p in enumerate(planes)}
    Pc = np.array([p[1] for p in planes]); Pn = np.array([p[2] for p in planes])
    ri = np.array([pid_of[r] for r in pr])
    Vf = V.copy()
    for _ in range(12):
        for k in range(len(planes)):
            sel = pv[ri == k]
            if len(sel):
                Vf[sel] -= ((Vf[sel] - Pc[k]) @ Pn[k])[:, None] * Pn[k]
    mv = np.linalg.norm(Vf - V, axis=1)
    too_far = mv > PRE_MOVE
    Vf[too_far] = V[too_far]
    log("pre-flatten: vertices moved", int((mv > 1e-6).sum()), "refused", int(too_far.sum()), "max move cm", round(float(mv[~too_far].max()), 3))
    obj = rr.import_fbx(RAW)[0]
    obj.data.vertices.foreach_set("co", (Vf / 100.0).ravel()); obj.data.update()
    me = obj.data
    # decimation: collapse, one pass; a second small pass if the first overshoots
    for it in range(3):
        cur = rr.tri_count(obj)
        if cur <= TARGET:
            break
        md = obj.modifiers.new("dec", "DECIMATE")
        md.decimate_type = "COLLAPSE"; md.ratio = max(1e-4, TARGET / cur * (0.99 if it else 1.0)); md.use_collapse_triangulate = True
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier="dec")
        log("decimate pass", it, "->", rr.tri_count(obj), "tris")
    Vd, Fd = arrays(obj.data)

    rr.export_fbx(os.path.join(WORK, "decimated.fbx"), [obj])
    json.dump({"tris_raw": int(n0), "tris_decimated": int(len(Fd)), "pre_flatten_vertices_moved": int((mv > 1e-6).sum()), "pre_flatten_refused": int(too_far.sum())}, open(os.path.join(WORK, "dec.json"), "w"))
    log("dense phase done")


# ------------------------------------------------------------------------------------------------ phase game
def build_material(name, outdir):
    maps = {"BaseColor": "Meshy_AI_Rustwing_Quadrotor_20261024_texture.png", "Metallic": "Meshy_AI_Rustwing_Quadrotor_20261024_texture_metallic.png",
            "Normal": "Meshy_AI_Rustwing_Quadrotor_20261024_texture_normal.png", "Roughness": "Meshy_AI_Rustwing_Quadrotor_20261024_texture_roughness.png"}
    paths = {}
    for k, fn in maps.items():
        dst = os.path.join(outdir, "T_Extractor_%s.png" % k)
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


def phase_game():
    from mathutils import Vector
    from mathutils.bvhtree import BVHTree
    dm = np.load(os.path.join(WORK, "dense_mesh.npz")); VD = dm["V"].astype(np.float64); FD = dm["F"].astype(np.int64)
    dn = np.load(os.path.join(WORK, "dense.npz"))
    lab = dn["lab"]; total = float(dn["total"])
    planes = {int(i): (c, n) for i, c, n in zip(dn["plane_id"], dn["plane_c"], dn["plane_n"])}
    reg_info = {int(i): (float(a), int(nfc), float(md)) for i, a, nfc, md in zip(dn["plane_id"], dn["plane_area"], dn["plane_faces"], dn["plane_maxdist"])}
    dec = json.load(open(os.path.join(WORK, "dec.json")))
    log("dense loaded", len(FD), "regions kept", len(planes))
    bvh = BVHTree.FromPolygons([tuple(v) for v in VD.tolist()], [tuple(f) for f in FD.tolist()])
    log("bvh built")

    ob = rr.import_fbx(os.path.join(WORK, "decimated.fbx"))[0]
    me = ob.data
    V0, F0 = arrays(me)
    nv, nf = len(V0), len(F0)
    uv0 = np.empty(len(me.loops) * 2); me.uv_layers[0].uv.foreach_get("vector", uv0)
    NF, AF = tri_normals(V0, F0)
    site = sites_of(V0)
    height_raw = float(V0[:, 2].max() - V0[:, 2].min())
    log("decimated imported", nf, "tris, dims", (V0.max(0) - V0.min(0)).round(2))

    # ---- map decimated faces to dense regions
    face_reg = -np.ones(nf, int)
    BAR = [(1 / 3, 1 / 3, 1 / 3), (.7, .15, .15), (.15, .7, .15), (.15, .15, .7)]
    cosf = math.cos(math.radians(FACE_ANG))
    for f in range(nf):
        votes = {}
        for w in BAR:
            pt = (V0[F0[f]] * np.array(w)[:, None]).sum(0)
            loc, nor, idx, d = bvh.find_nearest(Vector(pt))
            if idx is not None and int(lab[idx]) in planes:
                votes[int(lab[idx])] = votes.get(int(lab[idx]), 0) + 1
        for r, nvote in sorted(votes.items(), key=lambda kv: -kv[1]):
            if nvote < 2:
                break
            c, n = planes[r]
            if np.abs((V0[F0[f]] - c) @ n).max() <= DIST and NF[f] @ n >= cosf:
                face_reg[f] = r
                break
    face_reg0 = face_reg.copy()
    log("mapped first pass", int((face_reg0 >= 0).sum()), "of", nf)

    # ---- cause of the bulge, measured on the mapped faces before any change
    cn = np.empty(len(me.loops) * 3); me.corner_normals.foreach_get("vector", cn); cn = cn.reshape(-1, 3)
    loop_start = np.empty(nf, int); me.polygons.foreach_get("loop_start", loop_start)
    mapped = np.nonzero(face_reg >= 0)[0]

    def stats(arr):
        arr = np.array(arr)
        return {"mean": round(float(arr.mean()), 3), "p95": round(float(np.percentile(arr, 95)), 3), "max": round(float(arr.max()), 3)} if len(arr) else None
    vd, fa_, nd = [], [], []
    for f in mapped:
        c, n = planes[face_reg[f]]
        vd += list(np.abs((V0[F0[f]] - c) @ n))
        fa_.append(math.degrees(math.acos(max(-1, min(1, float(NF[f] @ n))))))
        for k in range(3):
            nd.append(math.degrees(math.acos(max(-1, min(1, float(cn[loop_start[f] + k] @ NF[f]))))))
    nmap = bpy.data.images.load(os.path.join(SRC_MAPS, "Meshy_AI_Rustwing_Quadrotor_20261024_texture_normal.png"))
    W, H = nmap.size
    px = np.empty(W * H * 4, np.float32); nmap.pixels.foreach_get(px); px = px.reshape(H, W, 4)
    uvl = uv0.reshape(-1, 2)

    def tilt(f, w):
        ls = loop_start[f]; uv = (uvl[ls:ls + 3] * np.array(w)[:, None]).sum(0)
        v = px[min(H - 1, max(0, int(uv[1] * H))), min(W - 1, max(0, int(uv[0] * W))), :3] * 2 - 1
        return math.degrees(math.acos(max(-1, min(1, v[2] / max(np.linalg.norm(v), 1e-9)))))
    rng = np.random.default_rng(1)
    samp_all = rng.choice(nf, min(3000, nf), replace=False)
    samp_map = rng.choice(mapped, min(3000, len(mapped)), replace=False) if len(mapped) else []
    cause = {"mapped_faces": int(len(mapped)), "vertex_distance_to_plane_cm": stats(vd), "face_normal_vs_plane_normal_deg": stats(fa_),
             "split_normal_vs_face_normal_deg": stats(nd),
             "normal_map_tilt_deg_on_mapped_faces_sample": stats([tilt(f, (1 / 3, 1 / 3, 1 / 3)) for f in samp_map]),
             "normal_map_tilt_deg_all_faces_sample": stats([tilt(f, (1 / 3, 1 / 3, 1 / 3)) for f in samp_all]),
             "smooth_faces_before": int(sum(1 for p in me.polygons if p.use_smooth))}
    log("CAUSE", json.dumps(cause))

    # ---- vertex targets (task-347 loop)
    RES_TOL = 0.005
    refused = []
    unm = {"move_over_limit": 0, "conflicting_planes": 0}
    site_target = {}
    for it in range(40):
        mapped = np.nonzero(face_reg >= 0)[0]
        reg_of_site = {}
        for f in mapped:
            for v in F0[f]:
                reg_of_site.setdefault(int(site[v]), set()).add(int(face_reg[f]))
        first = {}
        for i in range(nv):
            first.setdefault(int(site[i]), i)
        site_target = {}; bad = {}
        for s_, rs in reg_of_site.items():
            p = V0[first[s_]]
            rs = sorted(rs)
            Nm = np.array([planes[r][1] for r in rs]); d = np.array([planes[r][1] @ planes[r][0] for r in rs])
            x = p + np.linalg.pinv(Nm, rcond=1e-2) @ (d - Nm @ p)
            if np.abs(Nm @ x - d).max() > RES_TOL:
                bad[s_] = "conflicting_planes"; continue
            if np.linalg.norm(x - p) > MAX_MOVE:
                bad[s_] = "move_over_limit"; refused.append(round(float(np.linalg.norm(x - p)), 3)); continue
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
        c, n = planes[r]; vs = np.unique(F0[fs_])
        res_before[r] = float(np.abs((V0[vs] - c) @ n).max()); res_after[r] = float(np.abs((V1[vs] - c) @ n).max())
    log("flatten solved: mapped", len(mapped), "moved", int((moved > 1e-6).sum()), "regions used", len(res_after))

    # ---- orientation, scale, pivot (Q5). Nose rule from --nose-axis (found on the mesh, logged in the report)
    nose = opt("nose-axis", "+x", str)
    sgn = 1.0 if nose[0] == "+" else -1.0
    ax = "xyz".index(nose[1])
    # rotation about Z (multiples of 90 degrees) bringing the nose direction onto +X
    if ax == 0:
        ang = 0.0 if sgn > 0 else 180.0
    else:
        ang = -90.0 if sgn > 0 else 90.0
    ca, sa = math.cos(math.radians(ang)), math.sin(math.radians(ang))
    R = np.array([[ca, -sa, 0], [sa, ca, 0], [0, 0, 1.0]])
    Vr = V1 @ R.T
    ext = Vr.max(0) - Vr.min(0)
    lo = Vr.min(0)
    swap = ext[1] > ext[0] + 1e-6   # long side must be on X: report if the mesh is longer on Y
    scale = min(FOOT[0] / ext[0], FOOT[1] / ext[1])
    ctr = np.array([(Vr[:, 0].max() + Vr[:, 0].min()) / 2, (Vr[:, 1].max() + Vr[:, 1].min()) / 2, Vr[:, 2].min()])
    T = lambda P: ((P @ R.T) - ctr) * scale   # noqa: E731  raw frame -> SM_Extractor frame (cm)
    V2 = T(V1)
    log("placed: rot", ang, "scale", round(scale, 4), "dims", (V2.max(0) - V2.min(0)).round(2))

    # ---- write geometry, flat shading on mapped faces, material
    me.vertices.foreach_set("co", (V2 / 100.0).ravel()); me.update()
    for p in me.polygons:
        p.use_smooth = True
    for f in mapped:
        me.polygons[int(f)].use_smooth = False
    me.update()
    os.makedirs(OUTDIR, exist_ok=True)
    mat, paths = build_material("M_Extractor", OUTDIR)
    me.materials.clear(); me.materials.append(mat)
    ob.name = "SM_Extractor"; me.name = "SM_Extractor"
    fbx = os.path.join(OUTDIR, "SM_Extractor.fbx")
    rr.export_fbx(fbx, [ob])
    log("exported", fbx)

    # ---- reimport check in a clean scene
    ob2 = rr.import_fbx(fbx)[0]
    m2 = ob2.data
    Vc, Fc = arrays(m2)
    uv2 = np.empty(len(m2.loops) * 2); m2.uv_layers[0].uv.foreach_get("vector", uv2)
    mn2, mx2 = Vc.min(0), Vc.max(0)
    tris = rr.tri_count(m2)
    sm2 = np.array([p.use_smooth for p in m2.polygons])
    same_topo = bool(np.array_equal(Fc, F0))
    res2 = {}
    if same_topo:
        for r in res_after:
            c, n = planes[r]; c2 = T(c[None])[0]; n2 = (n @ R.T)
            vs = np.unique(F0[mapped[face_reg[mapped] == r]])
            res2[r] = float(np.abs((Vc[vs] - c2) @ n2).max())
    # silhouette: output vertices to dense source (scaled), dense vertices (sampled) to output surface
    bvh_out = BVHTree.FromPolygons([tuple(v) for v in Vc.tolist()], [tuple(f) for f in Fc.tolist()])
    Vraw = (Vc / scale + ctr) @ R   # output vertices back in the raw frame: distances to the dense source, times scale
    d_out = [bvh.find_nearest(Vector(v))[3] * scale for v in Vraw.tolist()]
    sel = np.random.default_rng(2).choice(len(VD), 60000, replace=False)
    d_src = [bvh_out.find_nearest(Vector(v))[3] for v in T(VD[sel]).tolist()]
    VDs = T(VD)
    dim_src = VDs.max(0) - VDs.min(0)
    h_out = float(mx2[2] - mn2[2]); tol = 0.01 * h_out
    bbox_diff = float(max(np.abs(mn2 - VDs.min(0)).max(), np.abs(mx2 - VDs.max(0)).max()))
    # rotor blade regions: dense planar regions that are the largest near the corners, reported as centres (SM frame)
    regions_report = []
    for r, (a, nfc, md) in sorted(reg_info.items(), key=lambda kv: -kv[1][0]):
        c, n = planes[r]
        regions_report.append({"id": r, "area_cm2_raw": round(a, 1), "area_pct": round(a / total * 100, 3), "dense_faces": nfc,
                               "centre_cm_sm_frame": [round(float(t), 2) for t in T(c[None])[0]], "normal_sm_frame": [round(float(t), 4) for t in (n @ R.T)],
                               "dense_vertex_max_dist_to_plane_raw_cm": round(md, 4),
                               "max_residual_before_raw_cm": round(res_before.get(r, float("nan")), 4) if r in res_before else None,
                               "max_residual_after_cm": round(res2.get(r, float("nan")), 6) if r in res2 else None,
                               "decimated_faces_flat": int((face_reg[mapped] == r).sum())})
    report = {
        "asset": "SM_Extractor", "task": "task-366-02-extractor-flatten", "source": os.path.relpath(RAW, ROOT).replace("\\", "/"),
        "cause_of_bulge": cause,
        "thresholds": {"grow_plane_distance_cm": GROW_D, "grow_normal_deg": GROW_A, "min_area_pct_of_surface": GROW_PCT, "map_max_vertex_distance_cm": DIST,
                       "map_max_face_normal_deg": FACE_ANG, "max_vertex_move_cm": MAX_MOVE, "note": "task-347 thresholds, provisional owner Q4 2026-10-29"},
        "dense": {"tris": int(dec["tris_raw"]), "surface_cm2": round(total, 1), "regions_all": int(dn["regions_all"]), "regions_kept": len(planes),
                  "kept_area_share_pct": round(float(sum(v[0] for v in reg_info.values()) / total * 100, ), 2)},
        "triangles_before": int(dec["tris_raw"]), "triangles_after": int(tris), "budget": BUDGET,
        "faces_mapped_first_pass": int((face_reg0 >= 0).sum()), "faces_unmapped_in_loop_by_reason": unm, "refused_move_cm": refused[:20],
        "vertices_moved": int((moved > 1e-6).sum()), "max_move_raw_cm": round(float(moved.max()), 3),
        "regions_used": len(res_after), "faces_flat_shaded": int(len(mapped)),
        "max_residual_region_cm_raw_frame_after": round(max(res_after.values()), 6) if res_after else None,
        "max_residual_region_cm_after_reimport": round(max(res2.values()), 6) if res2 else None,
        "regions": regions_report,
        "placement": {"nose_axis_raw": nose, "rotation_about_z_deg": ang, "long_side_on_y_before_fit": bool(swap), "uniform_scale": round(float(scale), 5),
                      "footprint_cm": list(FOOT), "bounds_cm": [round(float(t), 2) for t in (mx2 - mn2)], "min_cm": [round(float(t), 3) for t in mn2], "max_cm": [round(float(t), 3) for t in mx2],
                      "pivot": "bottom centre (x, y bbox centre, min z = 0)", "source_dims_raw_cm": [round(float(t), 2) for t in (VD.max(0) - VD.min(0))],
                      "source_dims_scaled_cm": [round(float(t), 2) for t in dim_src]},
        "silhouette": {"height_cm": round(h_out, 2), "tolerance_1pct_cm": round(tol, 3), "bbox_max_change_vs_scaled_source_cm": round(bbox_diff, 3),
                       "output_vertex_to_source_surface_cm": stats(d_out), "source_vertex_sample_to_output_surface_cm": stats(d_src)},
        "uv": {"layers": [u.name for u in m2.uv_layers], "uv0_max_abs_diff_vs_decimated": round(float(np.abs(uv2 - uv0).max()), 8) if len(uv2) == len(uv0) else "loop count differs",
               "uv0_loops_outside_0_1": int(((uv2 < -1e-4) | (uv2 > 1 + 1e-4)).sum())},
        "materials": [m.name for m in m2.materials], "texture_files": {k: os.path.relpath(v, ROOT).replace("\\", "/") for k, v in paths.items()},
        "faces_smooth_after": int(sm2.sum()), "faces_flat_after": int((~sm2).sum()),
        "reimport_same_topology": same_topo, "warnings": [],
    }
    w = report["warnings"]
    if tris > BUDGET: w.append("tris %d over budget %d" % (tris, BUDGET))
    if res2 and max(res2.values()) >= 0.01: w.append("residual >= 0.01 cm")
    if abs(mn2[2]) > 0.1: w.append("min z not 0")
    if mx2[0] - mn2[0] > FOOT[0] + 0.1 or mx2[1] - mn2[1] > FOOT[1] + 0.1: w.append("bounds over footprint")
    if mx2[0] - mn2[0] < mx2[1] - mn2[1]: w.append("long side not on X")
    if max(d_out) > tol or max(d_src) > tol: w.append("silhouette error above 1 pct of height (see silhouette block)")
    with open(os.path.join(OUTDIR, "extractor_flat.report.json"), "w", encoding="utf-8") as fh:
        json.dump(report, fh, indent=2)
    log("DONE", json.dumps({k: report[k] for k in ("triangles_after", "regions_used", "faces_flat_shaded", "max_residual_region_cm_after_reimport", "warnings")}), report["placement"]["bounds_cm"])


if __name__ == "__main__":
    try:
        {"dense": phase_dense, "game": phase_game}[PHASE]()
    except Exception:
        import traceback
        traceback.print_exc()
        sys.exit(1)
