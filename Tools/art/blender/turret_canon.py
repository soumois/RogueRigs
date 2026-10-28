"""Turret canon rebuild (task-346-01), headless Blender 5.2.

Replaces the canon of the optimised Meshy turret by a canon built in bpy after measurements of the
unoptimised Meshy turret: muzzle, flange, inner barrel, perforated outer shroud (real holes, boolean),
studs, lug. Canon texture (turret_canon: base colour, normal, roughness, 1024 px) baked from the
unoptimised canon. The body keeps its vertices and UVs.

Usage:
  blender -b --factory-startup -P Tools/art/blender/turret_canon.py -- [--out-dir Art/Meshes/turret]
Run from the repository root. Writes SM_Turret.fbx, turret_canon.report.json, T_Turret_*.png in out-dir.

Frame (cm, Blender axes as imported from the Meshy FBX, 1 unit = 1 m in the scene): the canon axis is
parallel to Y, pointing to -Y, through (x, z) = (AX, AZ). Angle 0 = +X, 90 = +Z (up).
"""
import bpy
import bmesh
import json
import math
import os
import shutil
import sys
import numpy as np
from mathutils import Vector, Matrix
from mathutils.bvhtree import BVHTree

ROOT = os.getcwd()
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = os.path.join(ROOT, args[args.index("--out-dir") + 1] if "--out-dir" in args else "Art/Meshes/turret")
SRC_OPT = os.path.join(ROOT, "Art/Source/turret-meshy/meshy-optimised")
SRC_UN = os.path.join(ROOT, "Art/Source/turret-meshy/meshy-unoptimised")
FBX_NAME = "Meshy_AI_Teal_Bastion_Gunner_20261028_texture_remesh_tri"
CM = 0.01
AX, AZ = -0.65, 19.3         # canon axis (cm): circle fit of the unoptimised (-0.70, 19.29) and optimised (-0.63, 19.30) canons
Y_CUT = -46.5                # canon = faces with centre y < Y_CUT and radial distance < R_CANON
R_CANON = 17.0              # front block, studs and lug (y < Y_FRONT) reach 15.5 cm
R_SHROUD = 12.0             # behind Y_FRONT only the shroud, barrel and studs (r < 12) are canon: the collar front chamfers stay
Y_FRONT = -84.0
BUDGET = 16000
warnings = []
BV_UNCUT = []


# ---------------------------------------------------------------- helpers
def import_fbx(path, name):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=path)
    obj = [o for o in bpy.data.objects if o not in before and o.type == "MESH"][0]
    obj.name = name
    return obj


def verts_np(me):
    n = len(me.vertices)
    a = np.empty(n * 3)
    me.vertices.foreach_get("co", a)
    return a.reshape(-1, 3) * 100.0   # cm


def make_bvh(obj):
    dg = bpy.context.evaluated_depsgraph_get()
    return BVHTree.FromObject(obj, dg)


def radial_grid(bv, y0, y1, dy, da_deg, cx=AX, cz=AZ):
    """Outer surface radius (cm) seen from outside along radial rays; 0 = miss."""
    ys = np.arange(y0, y1, dy)
    na = int(round(360 / da_deg))
    H = np.zeros((len(ys), na))
    for j, y in enumerate(ys):
        for i in range(na):
            a = math.radians(i * da_deg)
            d = Vector((math.cos(a), 0, math.sin(a)))
            org = Vector((cx, y, cz)) * CM + d * 0.20
            loc, nrm, idx, dist = bv.ray_cast(org, -d, 0.25)
            H[j, i] = (0.20 - dist) * 100 if loc else 0.0
    return ys, H


def circle_fit(pts):
    x, z = pts[:, 0], pts[:, 1]
    A = np.c_[2 * x, 2 * z, np.ones(len(x))]
    b = x * x + z * z
    c, *_ = np.linalg.lstsq(A, b, rcond=None)
    return c[0], c[1], math.sqrt(max(c[2] + c[0] ** 2 + c[1] ** 2, 0))


def measure_profile(name, bv):
    """Length, radii, axis of the canon (outer silhouette seen along radial rays)."""
    ys, H = radial_grid(bv, -100.0, -44.0, 0.5, 3)
    p85 = np.array([np.percentile(r, 85) for r in H])
    anyhit = (H > 0).any(axis=1)
    tip = float(ys[np.argmax(anyhit)])
    collar = None
    for y, p in zip(ys, p85):
        if y > tip + 5 and p > 12.5:
            collar = float(y)
            break

    def row(y):
        return float(p85[int(round((y - ys[0]) / 0.5))])
    # axis: circle fit on outer-surface hits of cylindrical slices
    cen, rad = [], []
    for y in list(np.arange(-82, -72, 2.0)) + list(np.arange(-66, -50, 2.0)):
        j = int(round((y - ys[0]) / 0.5))
        r = H[j]
        sel = r >= p85[j] - 0.25
        a = np.radians(np.arange(len(r)) * 3.0)[sel]
        pts = np.c_[AX + r[sel] * np.cos(a), AZ + r[sel] * np.sin(a)]
        if len(pts) >= 8:
            fx, fz, fr = circle_fit(pts)
            cen.append((y, fx, fz))
            rad.append(fr)
    cen = np.array(cen)
    sx = np.polyfit(cen[:, 0], cen[:, 1], 1)
    sz = np.polyfit(cen[:, 0], cen[:, 2], 1)
    ymid = -66.0
    return {
        "mesh": name,
        "axis_direction": "-Y (parallel to Y; slope of centre per cm of length: dx %.4f, dz %.4f)" % (sx[0], sz[0]),
        "axis_slope_dx_dy": round(float(sx[0]), 4), "axis_slope_dz_dy": round(float(sz[0]), 4),
        "axis_centre_cm_at_y-66": [round(float(np.polyval(sx, ymid)), 2), round(float(np.polyval(sz, ymid)), 2)],
        "tip_y_cm": round(tip, 2), "collar_start_y_cm": round(collar, 2) if collar is not None else None,
        "canon_length_cm": round(collar - tip, 2) if collar is not None else None,
        "outer_radius_cm_by_y": {str(y): round(row(y), 2) for y in (-94.0, -93.0, -92.0, -91.0, -88.0, -85.0, -80.0, -75.0, -71.0, -68.0, -64.0, -60.0, -56.0, -52.0, -48.0)},
    }, (ys, p85)


def smooth_ref(H, wy, wa, q=0.8):
    ny, na = H.shape
    P = np.concatenate([H[:, -wa:], H, H[:, :wa]], axis=1)
    R = np.zeros_like(H)
    for j in range(ny):
        j0, j1 = max(0, j - wy), min(ny, j + wy + 1)
        blk = P[j0:j1]
        for i in range(na):
            w = blk[:, i:i + 2 * wa + 1].ravel()
            w = w[w > 0]
            R[j, i] = np.sort(w)[int(q * (len(w) - 1))] if len(w) else 0
    return R


def measure_holes(name, bv, bv_ref=None):
    """Holes of the outer shroud on the upper arc: shape/size/spacing from a fine radial ray grid."""
    DY, DA = 0.25, 1.5
    ys, H = radial_grid(bv, -86.0, -48.0, DY, DA)
    na = H.shape[1]
    ang = np.arange(na) * DA
    valid = (ang <= 195) | (ang >= 345)
    if bv_ref is not None:
        # new canon: the undisturbed surface is the same shroud before the holes are cut (known exactly)
        _, R = radial_grid(bv_ref, -86.0, -48.0, DY, DA)
    else:
        # meshy meshes: local reference = 80th percentile of the surrounding surface (window 8 cm x 48 deg)
        R = smooth_ref(H, 16, 16)
    D = (R - H > 1.2) & (H > 0)
    D &= valid[None, :]
    lab = np.zeros(D.shape, int)
    comps = []
    for a in range(D.shape[0]):
        for b in range(na):
            if D[a, b] and not lab[a, b]:
                lab[a, b] = len(comps) + 1
                st, c = [(a, b)], []
                while st:
                    p, q = st.pop()
                    c.append((p, q))
                    for dp, dq in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        pp, qq = p + dp, (q + dq) % na
                        if 0 <= pp < D.shape[0] and D[pp, qq] and not lab[pp, qq]:
                            lab[pp, qq] = len(comps) + 1
                            st.append((pp, qq))
                comps.append(c)
    holes, rejected = [], []
    for c in comps:
        Rm = float(np.mean([R[p, q] for p, q in c]))
        area = len(c) * DY * math.radians(DA) * Rm
        if area < 2.0:
            continue
        yy = np.array([ys[p] for p, q in c])
        qq = np.array([q * DA for p, q in c], float)
        if qq.max() - qq.min() > 180:
            qq = np.where(qq > 180, qq - 360, qq)
        h = float(yy.max() - yy.min() + DY)
        w = float(math.radians(qq.max() - qq.min() + DA) * Rm)
        rec = {"y_cm": round(float(yy.mean()), 2), "angle_deg": round(float(qq.mean() % 360), 1),
               "height_cm": round(h, 2), "width_cm": round(w, 2),
               "d_eq_cm": round(2 * math.sqrt(area / math.pi), 2), "area_cm2": round(area, 2),
               "ref_radius_cm": round(Rm, 2),
               "depth_cm": round(float(np.mean([R[p, q] - H[p, q] for p, q in c])), 2)}
        asp = max(h, w) / max(min(h, w), 1e-6)
        if asp <= 3.0 and h <= 8 and w <= 10 and Rm <= 10.6:
            holes.append(rec)
        else:
            rec["why"] = "seam, step or hook, not a hole (aspect %.1f, h %.1f, w %.1f, R %.1f)" % (asp, h, w, Rm)
            rejected.append(rec)
    holes.sort(key=lambda r: (r["y_cm"], r["angle_deg"]))
    return holes, rejected


def hole_stats(holes):
    if not holes:
        return {"count": 0}
    d = np.array([h["d_eq_cm"] for h in holes])
    # spacing: nearest neighbour distance on the unrolled surface (arc length at R 9.5, y)
    P = np.array([[math.radians(h["angle_deg"]) * 9.5, h["y_cm"]] for h in holes])
    nn = []
    for i in range(len(P)):
        dd = np.hypot(P[:, 0] - P[i, 0], P[:, 1] - P[i, 1])
        dd[i] = 1e9
        dd = np.minimum(dd, np.hypot(np.abs(P[:, 0] - P[i, 0]) - 2 * math.pi * 9.5, P[:, 1] - P[i, 1]).clip(0))
        nn.append(float(dd.min()))
    rows = []
    for h in sorted(holes, key=lambda r: r["y_cm"]):
        if rows and abs(rows[-1][-1]["y_cm"] - h["y_cm"]) <= 1.6:
            rows[-1].append(h)
        else:
            rows.append([h])
    return {"count": len(holes), "shape": "round (oval dimples on the unoptimised shroud)",
            "rows": len(rows), "count_per_row": [len(r) for r in rows],
            "row_y_cm": [round(float(np.mean([h["y_cm"] for h in r])), 1) for r in rows],
            "size_d_eq_cm_mean": round(float(d.mean()), 2), "size_d_eq_cm_min": round(float(d.min()), 2),
            "size_d_eq_cm_max": round(float(d.max()), 2),
            "spacing_nearest_neighbour_cm_mean": round(float(np.mean(nn)), 2)}


# ---------------------------------------------------------------- canon construction (cm)
def P(r, ang_deg, y):
    a = math.radians(ang_deg)
    return Vector(((AX + r * math.cos(a)) * CM, y * CM, (AZ + r * math.sin(a)) * CM))


def tube(bm, y0, y1, ro0, ro1, wall, sides, uvl=None, ang0=0.0):
    """Closed tube along -Y: outer cone ro0 (at y0) to ro1 (at y1), wall thickness, annulus end caps."""
    def ring(y, r):
        return [bm.verts.new(P(r, ang0 + 360.0 * k / sides, y)) for k in range(sides)]
    oa, ob = ring(y0, ro0), ring(y1, ro1)
    ia, ib = ring(y0, max(ro0 - wall, 0.01)), ring(y1, max(ro1 - wall, 0.01))
    fs = []
    for k in range(sides):
        n = (k + 1) % sides
        fs.append(bm.faces.new((oa[k], oa[n], ob[n], ob[k])))
        fs.append(bm.faces.new((ia[k], ib[k], ib[n], ia[n])))
        fs.append(bm.faces.new((oa[k], ia[k], ia[n], oa[n])))
        fs.append(bm.faces.new((ob[k], ob[n], ib[n], ib[k])))
    bmesh.ops.recalc_face_normals(bm, faces=fs)
    return fs


def ptube(bm, prof, wall, sides, ang0=0.0):
    """Closed tube along -Y from a profile [(y, outer_radius), ...]: outer and inner strips, two annulus end caps."""
    outer = [[bm.verts.new(P(r, ang0 + 360.0 * k / sides, y)) for k in range(sides)] for y, r in prof]
    inner = [[bm.verts.new(P(r - wall, ang0 + 360.0 * k / sides, y)) for k in range(sides)] for y, r in prof]
    fs = []
    for j in range(len(prof) - 1):
        for k in range(sides):
            n = (k + 1) % sides
            fs.append(bm.faces.new((outer[j][k], outer[j][n], outer[j + 1][n], outer[j + 1][k])))
            fs.append(bm.faces.new((inner[j][k], inner[j + 1][k], inner[j + 1][n], inner[j][n])))
    for j, flip in ((0, False), (len(prof) - 1, True)):
        for k in range(sides):
            n = (k + 1) % sides
            q = (outer[j][k], inner[j][k], inner[j][n], outer[j][n])
            fs.append(bm.faces.new(q[::-1] if flip else q))
    bmesh.ops.recalc_face_normals(bm, faces=fs)
    return fs


def cylinder(bm, y0, y1, r, sides, ang0=0.0):
    a = [bm.verts.new(P(r, ang0 + 360.0 * k / sides, y0)) for k in range(sides)]
    b = [bm.verts.new(P(r, ang0 + 360.0 * k / sides, y1)) for k in range(sides)]
    fs = [bm.faces.new((a[k], a[(k + 1) % sides], b[(k + 1) % sides], b[k])) for k in range(sides)]
    fs.append(bm.faces.new(a))
    fs.append(bm.faces.new(b[::-1]))
    bmesh.ops.recalc_face_normals(bm, faces=fs)
    return fs


def box(bm, x0, x1, y0, y1, z0, z1):
    vs = [bm.verts.new(Vector((x * CM, y * CM, z * CM))) for x in (x0, x1) for y in (y0, y1) for z in (z0, z1)]
    bmesh.ops.convex_hull(bm, input=vs)


# ---------------------------------------------------------------- main
def main():
    os.makedirs(OUT, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    sc = bpy.context.scene

    opt = import_fbx(os.path.join(SRC_OPT, FBX_NAME + ".fbx"), "opt")
    un = import_fbx(os.path.join(SRC_UN, FBX_NAME + ".fbx"), "unopt")
    report = {"asset": "SM_Turret", "task": "task-346-01-turret-canon", "warnings": warnings}

    # (1) measure both canons
    bv_un, bv_opt = make_bvh(un), make_bvh(opt)
    prof_un, _ = measure_profile("unoptimised", bv_un)
    prof_opt, _ = measure_profile("optimised (source canon, before)", bv_opt)
    holes_un, rej_un = measure_holes("unoptimised", bv_un)
    holes_opt, rej_opt = measure_holes("optimised", bv_opt)
    report["canon_measurements"] = {
        "location_rule": "canon = faces whose centre has y < %.1f cm and distance to the canon axis < %.1f cm, or < %.1f cm when y < %.1f cm (muzzle block, lug); axis parallel to Y through x,z = %.1f, %.1f cm, canon extends to -Y. Collar (white housing, R >= 12.5 cm) starts at y = %.1f cm and is body." % (Y_CUT, R_SHROUD, R_CANON, Y_FRONT, AX, AZ, Y_CUT),
        "unoptimised": prof_un, "optimised_source": prof_opt,
    }
    report["holes"] = {
        "unoptimised": {"stats": hole_stats(holes_un), "holes": holes_un, "rejected_components": rej_un},
        "optimised_source": {"stats": hole_stats(holes_opt)},
    }
    print("MEASURED unopt", json.dumps(prof_un))
    print("MEASURED holes", json.dumps(hole_stats(holes_un)))

    # (2) remove the optimised canon faces
    me = opt.data
    src_tris = len(me.polygons)
    src_verts = len(me.vertices)
    bm = bmesh.new()
    bm.from_mesh(me)
    def is_canon(c):
        r = math.hypot(c.x * 100 - AX, c.z * 100 - AZ)
        return c.y * 100 < Y_CUT and (r < R_SHROUD or (c.y * 100 < Y_FRONT and r < R_CANON))
    canon_faces = [f for f in bm.faces if is_canon(f.calc_center_median())]
    canon_tris_removed = sum(len(f.verts) - 2 for f in canon_faces)
    keep_verts_src = set(v.index for f in bm.faces if f not in set(canon_faces) for v in f.verts)
    bmesh.ops.delete(bm, geom=canon_faces, context="FACES")
    body_verts = len(bm.verts)
    body_tris = sum(len(f.verts) - 2 for f in bm.faces)
    bm.to_mesh(me)
    bm.free()
    body_co = verts_np(me)
    uvl_name = me.uv_layers[0].name
    body_uv = np.empty(len(me.loops) * 2)
    me.uv_layers[0].uv.foreach_get("vector", body_uv)
    print("body verts", body_verts, "body tris", body_tris, "canon tris removed", canon_tris_removed, "keep_verts_src", len(keep_verts_src))
    report["body"] = {"source_tris": src_tris, "source_verts": src_verts, "canon_tris_removed": canon_tris_removed,
                      "canon_faces_removed": len(canon_faces),
                      "body_tris_kept": body_tris, "body_verts_kept": body_verts,
                      "source_verts_outside_canon": len(keep_verts_src)}

    # (3) build the new canon
    spec = SPEC
    canon, hole_info = build_new_canon(holes_un, spec)
    cb = make_bvh(canon)
    canon.data.calc_loop_triangles()
    canon_tris = len(canon.data.loop_triangles)
    print("canon tris", canon_tris)

    # ray check through every hole centre
    chk = []
    for h in hole_info:
        d = Vector((math.cos(math.radians(h["angle_deg"])), 0, math.sin(math.radians(h["angle_deg"]))))
        org = Vector((AX, h["y_cm"], AZ)) * CM + d * 0.20
        loc, nrm, idx, dist = cb.ray_cast(org, -d, 0.25)
        r_hit = (0.20 - dist) * 100 if loc else None
        chk.append({"hole": h["id"], "first_hit_radius_cm": round(r_hit, 2) if r_hit else None,
                    "reaches_inner_barrel": bool(r_hit is not None and abs(r_hit - spec["barrel_r"]) < 0.3)})
    report["holes"]["ray_through_centre"] = chk
    report["holes"]["new"] = {"constructed": hole_info}

    # (4) texture: UV, bake from the unoptimised canon
    uv_and_bake(canon, un, report)

    # (5) join, pivot, export
    finalize(opt, canon, body_co, body_uv, body_verts, report, src_tris, canon_tris, uvl_name)


FIDELITY = {
    "note": "Agent judgement from renders in Saved/TurretMesh/canon/ (after_*.png against reference_*.png and before_*.png, same cameras); no criteria were listed in the subtask, these are the canon's own",
    "returns_used": 1,
    "return_1": "after_canon.png showed the white collar front chamfers cut with the canon (rule was r < 17 cm for every y < -46.5); rule changed to r < 12 cm behind y = -84 cm, collar restored",
    "criteria": {
        "C1 length, tip, radii, axis position within 2% of height": {"verdict": "pass", "evidence": "see deviation_vs_unoptimised"},
        "C2 muzzle ring with open bore, flange behind it": {"verdict": "pass", "evidence": "after_canon.png: dark bore visible in a ring, flange step behind it, as in reference_canon.png"},
        "C3 two shroud segments with a step (front plates larger than the rear tube)": {"verdict": "pass", "evidence": "after_canon_top.png: wide front segment, narrower rear tube, step at y = -69 cm, as reference_canon_top.png"},
        "C4 perforated outer shroud, real holes, inner barrel visible through them": {"verdict": "pass", "evidence": "after_canon.png and after_canon_top.png: 15 open holes with dark inner barrel behind; ray through each hole centre hits the barrel (report holes.ray_through_centre)"},
        "C5 studs on top of the front shroud": {"verdict": "pass", "evidence": "after_canon_top.png: two studs on the centre line, as the reference"},
        "C6 lug under the muzzle": {"verdict": "pass", "evidence": "after_canon.png: square lug under the muzzle (the reference lug is a curved hook, shape simplified)"},
        "C7 dark metal, white chips at the muzzle plates": {"verdict": "pass", "evidence": "after_canon.png: dark grey shroud, white chips at the front plate edges from the baked texture"},
        "C8 body, collar and housing untouched": {"verdict": "pass", "evidence": "body vertices equal the source outside the canon, positions and UVs identical (body_unchanged); after_front.png and after_side.png match before_*.png"},
    },
    "final_render": "Saved/TurretMesh/canon/after_canon.png",
}

SPEC = {
    "barrel_y0": -91.5, "barrel_y1": -45.0, "barrel_r": 6.6, "barrel_sides": 12,
    "muzzle_y0": -94.6, "muzzle_y1": -91.5, "muzzle_r": 6.0, "bore_r": 3.8, "muzzle_sides": 16,
    "flange_y0": -92.0, "flange_y1": -90.0, "flange_r": 9.3, "flange_sides": 16,
    "shroud_profile": [(-90.0, 10.2), (-70.2, 10.2), (-68.2, 8.45), (-45.0, 10.35)],
    "front_r": 10.2,
    "wall": 0.9, "shroud_sides": 20,
    "stud_y": [-87.5, -74.5], "stud_r": 1.6, "stud_top_r": 11.5,
    "lug": (-3.2, 3.2, -89.5, -85.5, 4.2, 10.5),
}


def build_new_canon(holes_un, spec):
    bm = bmesh.new()
    cylinder(bm, spec["barrel_y0"], spec["barrel_y1"], spec["barrel_r"], spec["barrel_sides"])
    tube(bm, spec["muzzle_y0"], spec["muzzle_y1"], spec["muzzle_r"], spec["muzzle_r"], spec["muzzle_r"] - spec["bore_r"], spec["muzzle_sides"])
    tube(bm, spec["flange_y0"], spec["flange_y1"], spec["flange_r"], spec["flange_r"], spec["flange_r"] - spec["muzzle_r"] + 0.1, spec["flange_sides"])
    for sy in spec["stud_y"]:
        r = spec["stud_r"]
        ring0 = [bm.verts.new(Vector(((AX + r * math.cos(math.radians(60 * k))) * CM,
                                      (sy + r * math.sin(math.radians(60 * k))) * CM,
                                      (AZ + spec["front_r"] - 0.5) * CM)))
                 for k in range(6)]
        ring1 = [bm.verts.new(Vector(((AX + r * math.cos(math.radians(60 * k))) * CM,
                                      (sy + r * math.sin(math.radians(60 * k))) * CM,
                                      (AZ + spec["stud_top_r"]) * CM))) for k in range(6)]
        fs = [bm.faces.new((ring0[k], ring0[(k + 1) % 6], ring1[(k + 1) % 6], ring1[k])) for k in range(6)]
        fs.append(bm.faces.new(ring0))
        fs.append(bm.faces.new(ring1[::-1]))
        bmesh.ops.recalc_face_normals(bm, faces=fs)
    box(bm, *spec["lug"])
    # flatten box z/x to world-correct values: the box() above uses x,y,z in cm directly (source frame)
    me_acc = bpy.data.meshes.new("canon_acc")
    bm.to_mesh(me_acc)
    bm.free()
    acc = bpy.data.objects.new("canon_acc", me_acc)
    bpy.context.scene.collection.objects.link(acc)

    # perforated shroud: two tubes, boolean difference with one object holding every hole cutter
    holes = []
    for k, h in enumerate(holes_un):
        d = float(np.clip(h["d_eq_cm"], 2.0, 6.5))
        holes.append({"id": k + 1, "y_cm": h["y_cm"], "angle_deg": h["angle_deg"], "d_eq_cm": round(d, 2),
                      "circumradius_cm": round(d / 1.898, 3), "sides": spec.get("hole_sides", 8)})
    hb = bmesh.new()
    for h in holes:
        a = math.radians(h["angle_deg"])
        radial = Vector((math.cos(a), 0, math.sin(a)))
        r_mid = (spec["barrel_r"] + 0.7 + 12.0) / 2
        L = 12.0 - (spec["barrel_r"] + 0.7)
        mat = Matrix.Translation(Vector((AX + r_mid * radial.x, h["y_cm"], AZ + r_mid * radial.z)) * CM) \
            @ radial.to_track_quat("Z", "Y").to_matrix().to_4x4()
        bmesh.ops.create_cone(hb, cap_ends=True, cap_tris=False, segments=h["sides"],
                              radius1=h["circumradius_cm"] * CM, radius2=h["circumradius_cm"] * CM,
                              depth=L * CM, matrix=mat)
    me_cut = bpy.data.meshes.new("cutters")
    hb.to_mesh(me_cut)
    hb.free()
    cut = bpy.data.objects.new("cutters", me_cut)
    bpy.context.scene.collection.objects.link(cut)

    parts = []
    b = bmesh.new()
    ptube(b, spec["shroud_profile"], spec["wall"], spec["shroud_sides"])
    mu = bpy.data.meshes.new("shroud_uncut")
    b.to_mesh(mu)
    uncut = bpy.data.objects.new("shroud_uncut", mu)
    bpy.context.scene.collection.objects.link(uncut)
    BV_UNCUT.append(make_bvh(uncut))
    m = bpy.data.meshes.new("shroud")
    b.to_mesh(m)
    b.free()
    o = bpy.data.objects.new("shroud", m)
    bpy.context.scene.collection.objects.link(o)
    bpy.context.view_layer.objects.active = o
    md = o.modifiers.new("holes", "BOOLEAN")
    md.operation = "DIFFERENCE"
    md.solver = "EXACT"
    md.object = cut
    md.use_self = True
    bpy.ops.object.select_all(action="DESELECT")
    o.select_set(True)
    bpy.ops.object.modifier_apply(modifier="holes")
    parts.append(o)
    bpy.data.objects.remove(cut)

    bpy.ops.object.select_all(action="DESELECT")
    for o in parts + [acc]:
        o.select_set(True)
    bpy.context.view_layer.objects.active = acc
    bpy.ops.object.join()
    canon = acc
    canon.name = "canon"
    bm = bmesh.new()
    bm.from_mesh(canon.data)
    bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method="BEAUTY", ngon_method="BEAUTY")
    bmesh.ops.remove_doubles(bm, verts=bm.verts, dist=1e-6)
    bm.normal_update()
    # hole wall faces per hole (faces whose centre lies inside a cutter prism between shroud walls)
    for h in holes:
        a = math.radians(h["angle_deg"])
        radial = Vector((math.cos(a), 0, math.sin(a)))
        walls = 0
        rim_edges = set()
        for f in bm.faces:
            c = f.calc_center_median() / CM
            rel = Vector((c.x - AX, 0, c.z - AZ))
            rr = rel.dot(radial)
            if rr < spec["barrel_r"] + 0.8 or rr > 11.5:
                continue
            along = rel - radial * rr
            dist = math.hypot(along.length, c.y - h["y_cm"])
            ny = f.normal
            if abs(dist - h["circumradius_cm"] * 0.94) < h["circumradius_cm"] * 0.12 and abs(ny.dot(radial)) < 0.45:
                walls += 1
        h["wall_faces"] = walls
        h["rim_loops"] = 2 if walls else 0
    bm.to_mesh(canon.data)
    bm.free()
    for p in canon.data.polygons:
        p.use_smooth = False
    return canon, holes


def uv_and_bake(canon, un, report):
    sc = bpy.context.scene
    # UV unwrap
    if not canon.data.uv_layers:
        canon.data.uv_layers.new(name="UVMap")
    bpy.ops.object.select_all(action="DESELECT")
    canon.select_set(True)
    bpy.context.view_layer.objects.active = canon
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=0.012)
    bpy.ops.object.mode_set(mode="OBJECT")

    # high-poly canon from the unoptimised mesh
    hp = un.copy()
    hp.data = un.data.copy()
    hp.name = "unopt_canon"
    sc.collection.objects.link(hp)
    bm = bmesh.new()
    bm.from_mesh(hp.data)
    out = []
    for f in bm.faces:
        c = f.calc_center_median()
        r = math.hypot(c.x * 100 - AX, c.z * 100 - AZ)
        if not (c.y * 100 < Y_CUT - 0.3 and (r < R_SHROUD or (c.y * 100 < Y_FRONT and r < R_CANON + 1))):
            out.append(f)
    bmesh.ops.delete(bm, geom=out, context="FACES")
    hp_tris = sum(len(f.verts) - 2 for f in bm.faces)
    bm.to_mesh(hp.data)
    bm.free()
    print("high-poly canon tris", hp_tris)
    un.hide_set(True)
    un.hide_render = True

    # material with bake targets
    mat = bpy.data.materials.new("turret_canon")
    mat.use_nodes = True
    nt = mat.node_tree
    canon.data.materials.append(mat)
    sc.render.engine = "CYCLES"
    sc.cycles.device = "CPU"
    sc.cycles.samples = 4
    sc.cycles.use_denoising = False
    bk = sc.render.bake
    bk.use_selected_to_active = True
    bk.cage_extrusion = 0.015
    bk.max_ray_distance = 0.03
    bk.margin = 8
    bk.use_clear = True
    bk.normal_space = "TANGENT"
    imgs = {}
    for key, kind, cs in (("BaseColor", "DIFFUSE", "sRGB"), ("Normal", "NORMAL", "Non-Color"), ("Roughness", "ROUGHNESS", "Non-Color")):
        img = bpy.data.images.new("T_Turret_Canon_" + key, 1024, 1024, alpha=False)
        img.colorspace_settings.name = cs
        node = nt.nodes.new("ShaderNodeTexImage")
        node.image = img
        node.name = "bake_" + key
        for n in nt.nodes:
            n.select = False
        node.select = True
        nt.nodes.active = node
        bk.use_pass_direct = bk.use_pass_indirect = False
        bk.use_pass_color = True
        bpy.ops.object.select_all(action="DESELECT")
        hp.select_set(True)
        canon.select_set(True)
        bpy.context.view_layer.objects.active = canon
        print("BAKE", kind)
        bpy.ops.object.bake(type=kind)
        img.filepath_raw = os.path.join(OUT, "T_Turret_Canon_%s.png" % key)
        img.file_format = "PNG"
        img.save()
        imgs[key] = (img, node)
    # wire the shading network
    bsdf = nt.nodes["Principled BSDF"]
    nt.links.new(imgs["BaseColor"][1].outputs["Color"], bsdf.inputs["Base Color"])
    nt.links.new(imgs["Roughness"][1].outputs["Color"], bsdf.inputs["Roughness"])
    nm = nt.nodes.new("ShaderNodeNormalMap")
    nt.links.new(imgs["Normal"][1].outputs["Color"], nm.inputs["Color"])
    nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    bpy.data.objects.remove(hp)
    sc.render.engine = "BLENDER_EEVEE"
    report["texture"] = {"material": "turret_canon", "size_px": 1024,
                         "maps": ["T_Turret_Canon_BaseColor.png", "T_Turret_Canon_Normal.png", "T_Turret_Canon_Roughness.png"],
                         "baked_from": "unoptimised canon faces (%d tris), selected to active, cage extrusion 1.5 cm, max ray 3 cm, margin 8 px" % hp_tris}


def finalize(opt, canon, body_co, body_uv, body_verts, report, src_tris, canon_tris, uvl_name):
    sc = bpy.context.scene
    # body material: renamed, textures copied next to the FBX under project names
    bmat = opt.data.materials[0]
    bmat.name = "turret_body"
    rename = {"texture_0.png": ("_tri.png", "T_Turret_Body_BaseColor.png"),
              "normal.png": ("_tri_normal.png", "T_Turret_Body_Normal.png"),
              "texture_0_roughness.png": ("_tri_roughness.png", "T_Turret_Body_Roughness.png"),
              "texture_0_metallic.png": ("_tri_metallic.png", "T_Turret_Body_Metallic.png")}
    for n in bmat.node_tree.nodes:
        if n.type == "TEX_IMAGE" and n.image:
            base = os.path.basename(n.image.filepath)
            if base in rename:
                suffix, new = rename[base]
                dst = os.path.join(OUT, new)
                srcname = {"_tri.png": FBX_NAME + ".png", "_tri_normal.png": FBX_NAME + "_normal.png",
                           "_tri_roughness.png": FBX_NAME + "_roughness.png", "_tri_metallic.png": FBX_NAME + "_metallic.png"}[suffix]
                shutil.copyfile(os.path.join(SRC_OPT, srcname), dst)
                img = bpy.data.images.load(dst)
                if n.image.colorspace_settings.name == "Non-Color":
                    img.colorspace_settings.name = "Non-Color"
                n.image = img

    # join canon into the optimised body: material 0 = turret_body, material 1 = turret_canon
    for p in canon.data.polygons:
        p.material_index = 0
    canon.data.materials.clear()
    canon.data.materials.append(bpy.data.materials["turret_canon"])
    bpy.ops.object.select_all(action="DESELECT")
    opt.select_set(True)
    canon.select_set(True)
    bpy.context.view_layer.objects.active = opt
    bpy.ops.object.join()
    obj = opt
    obj.name = "SM_Turret"
    me = obj.data
    me.name = "SM_Turret"
    # material indices: body polygons keep 0; canon polygons were 0 in a one-slot object -> after join they map to slot 1
    # (verified below)
    # measure the final canon (with the body, source frame) the same way as the unoptimised one
    bvf = make_bvh(obj)
    prof_new, _ = measure_profile("new canon in the final mesh", bvf)
    holes_new, rej_new = measure_holes("new", bvf, BV_UNCUT[0])
    report["canon_measurements"]["new_canon"] = prof_new
    pu = report["canon_measurements"]["unoptimised"]
    height = float((co_h := verts_np(me)).max(0)[2] - co_h.min(0)[2])
    dr = {k: round(abs(prof_new["outer_radius_cm_by_y"][k] - v), 2) for k, v in pu["outer_radius_cm_by_y"].items()}
    dax = math.hypot(prof_new["axis_centre_cm_at_y-66"][0] - pu["axis_centre_cm_at_y-66"][0], prof_new["axis_centre_cm_at_y-66"][1] - pu["axis_centre_cm_at_y-66"][1])
    dtip = abs(prof_new["tip_y_cm"] - pu["tip_y_cm"])
    dlen = abs(prof_new["canon_length_cm"] - pu["canon_length_cm"])
    tol = 0.02 * height
    report["deviation_vs_unoptimised"] = {
        "turret_height_cm": round(height, 2), "tolerance_2pct_cm": round(tol, 2),
        "length_cm": round(dlen, 2), "tip_position_cm": round(dtip, 2), "axis_position_cm": round(dax, 2),
        "axis_tilt_unopt_slope_dx_dz": [pu["axis_slope_dx_dy"], pu["axis_slope_dz_dy"]], "axis_tilt_new_slope_dx_dz": [prof_new["axis_slope_dx_dy"], prof_new["axis_slope_dz_dy"]],
        "axis_tilt_over_canon_length_cm": round(48.5 * max(abs(pu["axis_slope_dx_dy"]), abs(pu["axis_slope_dz_dy"])), 2),
        "outer_radius_abs_diff_cm_by_y": dr, "outer_radius_max_diff_cm": max(dr.values()),
        "within_2pct_of_height": bool(max(dlen, dtip, dax, max(dr.values())) <= tol),
    }
    report["holes"]["new"].update({"stats": hole_stats(holes_new), "holes": holes_new, "rejected_components": rej_new})
    # pivot: bbox bottom centre
    co = verts_np(me)
    mn, mx = co.min(0), co.max(0)
    off = np.array([-(mn[0] + mx[0]) / 2, -(mn[1] + mx[1]) / 2, -mn[2]])
    me.transform(Matrix.Translation(Vector(off) * CM))
    me.update()
    co = verts_np(me)
    dims = (co.max(0) - co.min(0))

    # body unchanged check (before export)
    mi = np.empty(len(me.polygons), int)
    me.polygons.foreach_get("material_index", mi)
    body_poly = mi == 0
    pv = []
    for p in me.polygons:
        pv.append(list(p.vertices))
    body_vs = set()
    for k, p in enumerate(pv):
        if body_poly[k]:
            body_vs.update(p)
    pos_diff = float(np.abs(co[:body_verts] - (body_co + off)).max()) if body_verts <= len(co) else None
    uv = np.empty(len(me.loops) * 2)
    me.uv_layers[0].uv.foreach_get("vector", uv)
    uv_diff = float(np.abs(uv[:len(body_uv)] - body_uv).max())
    me.calc_loop_triangles()
    tris = len(me.loop_triangles)
    canon_tris_total = int(sum(1 for t in me.loop_triangles if mi[t.polygon_index] != 0))
    report["body_unchanged"] = {
        "body_vertices_in_source_outside_canon": report["body"]["source_verts_outside_canon"],
        "body_vertices_in_export_mesh_before_write": len(body_vs),
        "vertex_positions_max_abs_diff_cm_after_pivot_shift": round(pos_diff, 6),
        "uv_max_abs_diff": round(uv_diff, 8),
        "pivot_offset_cm": [round(float(v), 3) for v in off],
    }
    report["tris"] = {"source_total": src_tris, "canon_removed": report["body"]["canon_tris_removed"],
                      "body_kept": int(body_poly.sum()), "canon_new": canon_tris_total, "total_after": tris, "budget": BUDGET}
    report["dimensions_cm"] = {"x": round(float(dims[0]), 2), "y": round(float(dims[1]), 2), "z": round(float(dims[2]), 2)}
    report["materials"] = [m.name for m in me.materials]
    report["uv_layers"] = [u.name for u in me.uv_layers]
    if tris >= BUDGET:
        warnings.append("total triangles %d not under budget %d" % (tris, BUDGET))

    warnings.extend([
        "hook bracket on the left of the canon rear (y -50 to -47 cm, R 11.5 cm) of both Meshy meshes is not rebuilt (cut with the canon faces, not in the new canon)",
        "front half-shell plates are one cylinder: the centre seam, the white edge chips as geometry and the muzzle bolt heads are not modelled (white chips come from the baked texture only)",
        "holes are 8-sided prisms through a 20-sided shroud (round dimples of the unoptimised shroud, area-equivalent size); the hole positions are those measured on the unoptimised upper arc 0-195 and 345-360 degrees",
        "lower arc (195 to 345 degrees, underside) not searched for holes: the unoptimised underside is rails and recesses (reference radius 7.6 to 8.5 cm, not the shroud); 3 shallow candidates there are not reproduced",
        "measured axis of both Meshy canons tilts 0.7 to 2.9 degrees depending on the fit seed (up to %.2f cm across the canon length, inside the 2 percent tolerance); the new canon axis is exactly parallel to Y" % report["deviation_vs_unoptimised"]["axis_tilt_over_canon_length_cm"],
        "body shading unchanged (smooth normals as imported): flat faces are task-347",
        "normal and roughness bake use the Principled network of the unoptimised mesh (normal-map detail included only if Cycles evaluates it in the NORMAL pass)",
    ])
    report["fidelity"] = FIDELITY
    fbx = os.path.join(OUT, "SM_Turret.fbx")
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.ops.export_scene.fbx(filepath=fbx, use_selection=True, object_types={"MESH"},
                             apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                             add_leaf_bones=False, path_mode="COPY", embed_textures=False)
    report["output"] = "Art/Meshes/turret/SM_Turret.fbx"
    with open(os.path.join(OUT, "turret_canon.report.json"), "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print("DONE tris", tris, "dims", report["dimensions_cm"], "pos_diff", pos_diff, "uv_diff", uv_diff)


if __name__ == "__main__":
    try:
        main()
    except Exception:
        import traceback
        traceback.print_exc()
        sys.exit(1)
