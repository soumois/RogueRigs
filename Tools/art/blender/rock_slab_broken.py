"""Broken variants of the rock slab dual-grid pieces (task-230-06). Blender route, deterministic, no Unreal import.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/rock_slab_broken.py -- [--out-dir Art/Meshes/rock-slab] [--no-preview]
Writes SM_RockSlabBroken_<Shape>_01.fbx, SM_RockSlabBroken.report.json and previews.
The constants, outlines, rim band and side walls come from rock_slab_pieces.py itself (its source is executed up to main(), so
every border crossing point is identical to the intact pieces). Only the top body is replaced:
  - top cut into thick angular plates (prisms PLATE_T thick, gaps of ~GAP between them), cells of one fixed 3 x 3 grid that is
    the same for every shape; its vertices on the piece borders are fixed (-H, -12, 0, 12, H on every side, no jitter), so
    neighbours match exactly and a plate vertex on a border has the same position in every shape and rotation;
  - rim band (R=1) and side walls (G=1) copied unchanged from the intact piece;
  - dark base cap (B=1) at z = CAP_Z (2 uu under the minimum top height of 60) over the whole piece footprint up to the outline.
Vertex colour: R=1 plate tops and rim band, G=1 plate edges and side walls, B=1 base cap. UV0 planar (x/100, y/100), UV1 LightMap.
The sag is NOT modelled: the material does it (WPO). It is simulated here for the previews only."""
import os, sys
_src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "rock_slab_pieces.py"), encoding="utf-8").read()
exec(compile(_src[:_src.index("def main():")], "rock_slab_pieces.py(prefix)", "exec"))   # H, TOP, MASKS, CANON, outline_pts, build_piece, finish, rot, case_of ...

PLATE_T = 12.0      # plate thickness (uu)
GAP = 1.6           # gap between plates (uu), each free vertex moves GAP/2 toward its plate centre
CAP_Z = 58.0        # base cap height (minimum top is 60)
MIN_TOP = 60.0
BORDER_PTS = (-12.0, 0.0, 12.0)
BUDGET = (80, 150)
SHAPES = list(MASKS)
NAME = "SM_RockSlabBroken_%s_01"
EPS = 1e-4


# ---------------------------------------------------------------- 2D helpers
def clip_convex(poly, tri):
    """Sutherland-Hodgman: convex polygon `poly` clipped by convex CCW polygon `tri` (lists of (x, y))."""
    out = list(poly)
    for k in range(len(tri)):
        a, b = tri[k], tri[(k + 1) % len(tri)]
        inp, out = out, []
        if not inp:
            break
        def side(p): return (b[0] - a[0]) * (p[1] - a[1]) - (b[1] - a[1]) * (p[0] - a[0])
        for i in range(len(inp)):
            p, q = inp[i], inp[(i + 1) % len(inp)]
            sp, sq = side(p), side(q)
            if sp >= -1e-9: out.append(p)
            if (sp >= -1e-9) != (sq >= -1e-9):
                t = sp / (sp - sq)
                out.append((p[0] + t * (q[0] - p[0]), p[1] + t * (q[1] - p[1])))
    return out


def area(poly):
    return 0.5 * sum(p[0] * q[1] - q[0] * p[1] for p, q in zip(poly, poly[1:] + poly[:1]))


def ccw(poly):
    return poly if area(poly) > 0 else poly[::-1]


def dedupe(poly):
    out = []
    for p in poly:
        if not out or math.hypot(p[0] - out[-1][0], p[1] - out[-1][1]) > 1e-5:
            out.append(p)
    if len(out) > 1 and math.hypot(out[0][0] - out[-1][0], out[0][1] - out[-1][1]) <= 1e-5:
        out.pop()
    return out


def seg_dist(p, a, b):
    ax, ay, bx, by = a[0], a[1], b[0], b[1]
    dx, dy = bx - ax, by - ay
    t = max(0.0, min(1.0, ((p[0] - ax) * dx + (p[1] - ay) * dy) / (dx * dx + dy * dy))) if dx * dx + dy * dy > 0 else 0.0
    return math.hypot(p[0] - ax - t * dx, p[1] - ay - t * dy)


def on_border(p):
    return abs(abs(p[0]) - H) < EPS or abs(abs(p[1]) - H) < EPS


def grid_cells(seed=9001):
    """3 x 3 irregular grid, identical for every shape. Border vertices fixed, the 4 interior ones jittered (fixed seed)."""
    r = random.Random(seed)
    xs = (-H, -12.0, 12.0, H)
    V = [[[xs[i], xs[j]] for i in range(4)] for j in range(4)]   # V[j][i]
    for j in (1, 2):
        for i in (1, 2):
            V[j][i][0] += r.uniform(-10.0, 10.0); V[j][i][1] += r.uniform(-10.0, 10.0)
    cells = []
    for j in range(3):
        for i in range(3):
            cells.append([tuple(V[j][i]), tuple(V[j][i + 1]), tuple(V[j + 1][i + 1]), tuple(V[j + 1][i])])
    return cells


def merge_fragments(frs):
    """merge polygons that share an identical edge (the ear-clip diagonals of the body), so a cell gives one plate."""
    def same(a, b): return math.hypot(a[0] - b[0], a[1] - b[1]) < 1e-4
    frs = [list(f) for f in frs]
    changed = True
    while changed:
        changed = False
        for ia in range(len(frs)):
            for ib in range(ia + 1, len(frs)):
                A, B = frs[ia], frs[ib]
                hit = None
                for i in range(len(A)):
                    p, q = A[i], A[(i + 1) % len(A)]
                    for j in range(len(B)):
                        if same(B[j], q) and same(B[(j + 1) % len(B)], p):
                            hit = (i, j); break
                    if hit: break
                if hit:
                    i, j = hit
                    na, nb = len(A), len(B)
                    m = [A[(i + 1 + t) % na] for t in range(na)] + [B[(j + 2 + t) % nb] for t in range(nb - 2)]
                    frs[ia] = m; del frs[ib]; changed = True
                    break
            if changed: break
    return frs


def insert_border_pts(poly):
    out = []
    for p, q in zip(poly, poly[1:] + poly[:1]):
        out.append(p)
        for ax in (0, 1):
            for s in (-H, H):
                if abs(p[ax] - s) < EPS and abs(q[ax] - s) < EPS:
                    o = 1 - ax
                    lo, hi = sorted((p[o], q[o]))
                    mids = [t for t in BORDER_PTS if lo + EPS < t < hi - EPS]
                    mids.sort(reverse=p[o] > q[o])
                    for t in mids:
                        m = [0.0, 0.0]; m[ax] = s; m[o] = t
                        out.append(tuple(m))
    return out


# ---------------------------------------------------------------- build
def build_broken(shape):
    ib = build_piece(shape)                         # intact piece: rim band, side walls and the body triangles
    seed = {"full": 1, "edge": 2, "outer": 3, "inner": 4, "diagonal": 5}[shape] * 7919
    b = Builder("broken_" + shape, seed)
    body = []
    for f in ib.kind_faces["top"]:
        pts = [(l.vert.co.x, l.vert.co.y) for l in f.loops]
        for k in range(1, len(pts) - 1):
            body.append(ccw([pts[0], pts[k], pts[k + 1]]))
    # rim band + side walls: unchanged
    for f in ib.kind_faces.get("rim", []):
        b.face([tuple(l.vert.co) for l in f.loops], (1, 0, 0), up=1, kind="rim")
    for f in ib.kind_faces.get("side", []):
        b.face([tuple(l.vert.co) for l in f.loops], (0, 1, 0), out=Vector((f.normal.x, f.normal.y, 0)), kind="side")
        # (colour G=1)
    # boundary of the body = edges used by exactly one body triangle
    cnt = {}
    for t in body:
        for p, q in zip(t, t[1:] + t[:1]):
            k = tuple(sorted(((round(p[0], 4), round(p[1], 4)), (round(q[0], 4), round(q[1], 4)))))
            cnt[k] = cnt.get(k, 0) + 1
    bedges = [k for k, v in cnt.items() if v == 1]
    def on_boundary(p):
        return any(seg_dist(p, a, c) < EPS for a, c in bedges)
    # base cap: body + rim band footprint at CAP_Z (up to the outline)
    cap = []
    for f in ib.kind_faces.get("rim", []):
        pts = [(l.vert.co.x, l.vert.co.y) for l in f.loops]
        for k in range(1, len(pts) - 1):
            cap.append([pts[0], pts[k], pts[k + 1]])
    cap += body
    for t in cap:
        b.face([(p[0], p[1], CAP_Z) for p in t], (0, 0, 1), up=1, kind="cap")
    # plates
    nplates = 0
    for cell in grid_cells(9001 + seed):
        frags = []
        for t in body:
            fr = dedupe(clip_convex(cell, t))
            if len(fr) >= 3 and abs(area(fr)) > 6.0:
                frags.append(ccw(fr))
        for fr in merge_fragments(frags):
            fr = dedupe(insert_border_pts(fr))
            # drop collinear vertices that are neither on a border nor on the outline
            keep = []
            for a_, p_, c_ in zip(fr[-1:] + fr[:-1], fr, fr[1:] + fr[:1]):
                col_ = abs((p_[0] - a_[0]) * (c_[1] - a_[1]) - (p_[1] - a_[1]) * (c_[0] - a_[0])) < 1e-6
                if col_ and not on_border(p_) and not on_boundary(p_): continue
                keep.append(p_)
            fr = keep
            cx = sum(p[0] for p in fr) / len(fr); cy = sum(p[1] for p in fr) / len(fr)
            pts = []
            for p in fr:
                if on_border(p) or on_boundary(p):
                    pts.append(p); continue
                d = math.hypot(cx - p[0], cy - p[1])
                s = min(GAP / 2, d * 0.4) / d if d > 1e-6 else 0.0
                pts.append((p[0] + (cx - p[0]) * s, p[1] + (cy - p[1]) * s))
            n = len(pts)
            nplates += 1
            top = [(p[0], p[1], TOP) for p in pts]
            for tri_ in ear_clip(pts):
                b.face([(v.x, v.y, TOP) for v in tri_], (1, 0, 0), up=1, kind="plate_top")
            ctr = Vector((cx, cy, TOP - PLATE_T / 2))
            for k in range(n):
                p, q = pts[k], pts[(k + 1) % n]
                mid = ((p[0] + q[0]) / 2, (p[1] + q[1]) / 2)
                if on_boundary(mid) and on_boundary(p) and on_boundary(q):
                    continue                  # edge against the outline or a piece border: neighbour or rim covers it
                quad = [(p[0], p[1], TOP), (q[0], q[1], TOP), (q[0], q[1], TOP - PLATE_T), (p[0], p[1], TOP - PLATE_T)]
                outv = Vector((mid[0] - cx, mid[1] - cy, 0))
                b.face([quad[0], quad[1], quad[2]], (0, 1, 0), out=outv, kind="plate_side")
                b.face([quad[0], quad[2], quad[3]], (0, 1, 0), out=outv, kind="plate_side")
    b.nplates = nplates
    return b


# ---------------------------------------------------------------- preview sag (simulation of the material WPO)
SAG_MAX = 45.0
LAYOUT = ["########",   # row 7 (top of the picture)
          "########",   # intact wall behind
          "########",
          "########",   # row 4: the damaged row (cols 1-4), void in front of it
          "#.......",
          "##....##",   # cols 6,7 rows 1-2: two touching damaged cells (6,2),(6,1)
          "###...##",
          "###..###"]
DAMAGE = {(1, 4): 1.0, (2, 4): 1.0, (3, 4): 0.9, (4, 4): 1.0, (6, 2): 0.9, (6, 1): 1.0}


def make_world():
    rows = LAYOUT[::-1]
    W, Hh = len(rows[0]), len(rows)
    def solid(c, r): return 1 if 0 <= c < W and 0 <= r < Hh and rows[r][c] == "#" else 0
    def dmg(c, r):
        if solid(c, r): return DAMAGE.get((c, r), 0.0)
        ns = [DAMAGE.get((c + dc, r + dr), 0.0) for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)) if solid(c + dc, r + dr)]
        return max(ns) if ns else 0.0
    empties = [(c, r) for r in range(-2, Hh + 2) for c in range(-2, W + 2) if not solid(c, r)]
    return solid, dmg, empties


def sag_at(x, y, solid, dmg, empties):
    u, v = x / 67.0 - 0.5, y / 67.0 - 0.5          # cell coordinates: cell (c, r) centre at (c, r)
    c0, r0 = math.floor(u), math.floor(v)
    fu, fv = u - c0, v - r0
    D = (dmg(c0, r0) * (1 - fu) * (1 - fv) + dmg(c0 + 1, r0) * fu * (1 - fv) + dmg(c0, r0 + 1) * (1 - fu) * fv + dmg(c0 + 1, r0 + 1) * fu * fv)
    dE = 1e9
    for c, r in empties:
        dx = max(abs(x - (c + 0.5) * 67.0) - H, 0.0); dy = max(abs(y - (r + 0.5) * 67.0) - H, 0.0)
        dE = min(dE, math.hypot(dx, dy))
    f = (1.0 - min(dE / 100.0, 1.0)) ** 2
    return SAG_MAX * D * f


# ---------------------------------------------------------------- main
def tri_stats(me, uvname="LightMap"):
    uv1 = me.uv_layers[uvname].data
    boxes = []
    for p in me.polygons:
        uvs = [uv1[li].uv for li in p.loop_indices]
        boxes.append((min(u[0] for u in uvs), min(u[1] for u in uvs), max(u[0] for u in uvs), max(u[1] for u in uvs)))
    overlaps = 0; mingap = 9.0
    for i in range(len(boxes)):
        for j in range(i + 1, len(boxes)):
            a, c = boxes[i], boxes[j]
            gap = max(c[0] - a[2], a[0] - c[2], c[1] - a[3], a[1] - c[3])
            if gap < 0: overlaps += 1
            mingap = min(mingap, gap)
    bmin = min(min(q[0], q[1]) for q in boxes); bmax = max(max(q[2], q[3]) for q in boxes)
    ok = overlaps == 0 and bmin * 256 >= 2 - 1e-6 and (1 - bmax) * 256 >= 2 - 1e-6 and mingap * 256 >= 2 - 1e-6
    return {"triangles": len(boxes), "overlapping_pairs": overlaps, "bounds": [round(bmin, 4), round(bmax, 4)], "min_gap_px_at_256": round(mingap * 256, 3), "pass": bool(ok)}


def border_set(bm, zmin=None):
    s = set()
    for f in bm.faces:
        for l in f.loops:
            p = l.vert.co
            if abs(abs(p.x) - H) < EPS or abs(abs(p.y) - H) < EPS:
                s.add((round(p.x, 3), round(p.y, 3), round(p.z, 3)))
    return s


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    report = {"task": "task-230-06-rock-plate-overlay (broken pieces)", "script": "Tools/art/blender/rock_slab_broken.py", "units": "cm (1 uu = 1 cm)",
              "constants": {"plate_thickness": PLATE_T, "plate_gap": GAP, "cap_z": CAP_Z, "min_top": MIN_TOP, "border_vertices_per_side": [-H] + list(BORDER_PTS) + [H]},
              "shapes": {}, "warnings": []}
    builders, intact = {}, {}
    for shape in SHAPES:
        ib = build_piece(shape); finish(ib); intact[shape] = ib
        b = build_broken(shape)
        N = finish(b)
        builders[shape] = b
        name = NAME % shape.capitalize()
        ob = to_object(b, name)
        bpy.ops.object.select_all(action="DESELECT"); ob.select_set(True)
        bpy.context.view_layer.objects.active = ob
        bpy.ops.export_scene.fbx(filepath=os.path.join(OUTDIR, name + ".fbx"), use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                                 mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
        me = ob.data
        tris = len(me.polygons)
        xs = [v.co.x for v in me.vertices]; ys = [v.co.y for v in me.vertices]; zs = [v.co.z for v in me.vertices]
        ca = me.color_attributes["Color"].data
        cnt = {"R": 0, "G": 0, "B": 0}; bad = 0
        for p in me.polygons:
            cs = {tuple(round(c, 3) for c in ca[li].color[:3]) for li in p.loop_indices}
            if len(cs) != 1: bad += 1; continue
            c = next(iter(cs))
            if c == (1.0, 0.0, 0.0): cnt["R"] += 1
            elif c == (0.0, 1.0, 0.0): cnt["G"] += 1
            elif c == (0.0, 0.0, 1.0): cnt["B"] += 1
            else: bad += 1
            zc = [me.vertices[i].co.z for i in p.vertices]
            if c == (0.0, 0.0, 1.0) and (max(zc) - min(zc) > 1e-4 or p.normal.z <= 0): bad += 1
            if c == (1.0, 0.0, 0.0) and (max(zc) - min(zc) > 1e-4 or p.normal.z <= 0): bad += 1
        uvc = tri_stats(me)
        uv0 = me.uv_layers["UVMap"].data
        uv0_ok = all(abs(uv0[l.index].uv[0] - me.vertices[l.vertex_index].co.x / 100) < 1e-5 and abs(uv0[l.index].uv[1] - me.vertices[l.vertex_index].co.y / 100) < 1e-5 for l in me.loops)
        # intact border crossings are all present in the broken piece (rim + walls copied)
        ib_set = border_set(intact[shape].bm); bb_set = border_set(b.bm)
        report["shapes"][shape] = {
            "file": "Art/Meshes/rock-slab/%s.fbx" % name, "tris": tris, "budget": list(BUDGET), "within_budget": BUDGET[0] <= tris <= BUDGET[1],
            "intact_tris": len(intact[shape].bm.faces), "plates": b.nplates,
            "dimensions_cm": [round(max(xs) - min(xs), 2), round(max(ys) - min(ys), 2), round(max(zs) - min(zs), 2)],
            "bounds_min": [round(min(xs), 2), round(min(ys), 2), round(min(zs), 2)], "bounds_max": [round(max(xs), 2), round(max(ys), 2), round(max(zs), 2)],
            "materials": ["M_RockSlab"], "uv_layers": [u.name for u in me.uv_layers],
            "uv_checks": {"UVMap_planar_xy_over_100": bool(uv0_ok), "LightMap": uvc},
            "vertex_colour": {"attribute": "Color (corner, float)", "export": "colors_type=LINEAR", "R1_plate_tops_and_rim": cnt["R"], "G1_edges_and_walls": cnt["G"], "B1_base_cap": cnt["B"],
                              "inconsistent_faces": bad, "pass": bad == 0},
            "intact_border_vertices_missing_in_broken": sorted(ib_set - bb_set)[:5], "intact_border_vertices_kept": ib_set <= bb_set,
            "all_flat_shaded": all(not p.use_smooth for p in me.polygons),
        }
        w = report["shapes"][shape]
        if not w["within_budget"]: report["warnings"].append("%s tris %d outside %s" % (shape, tris, list(BUDGET)))
        if not uvc["pass"]: report["warnings"].append("%s LightMap check failed" % shape)
        if bad: report["warnings"].append("%s vertex colour" % shape)
        if not w["intact_border_vertices_kept"]: report["warnings"].append("%s intact border vertices not kept" % shape)
        bpy.data.objects.remove(ob)

    # ---- seam check: every piece broken, placed over the test layout of rock_slab_pieces.py; shared border vertex sets must match
    TL = ["###..###", "###..###", "##.#....", "##..##..", ".....##.", "###.####", "###..#..", "###...#."]
    trows = TL[::-1]
    def tcell(c, r): return 1 if 0 <= c < 8 and 0 <= r < 8 and trows[r][c] == "#" else 0
    placed = {}
    for j in range(9):
        for i in range(9):
            m = tcell(i - 1, j - 1) | tcell(i, j - 1) << 1 | tcell(i, j) << 2 | tcell(i - 1, j) << 3
            cs = case_of(m)
            if cs: placed[(i, j)] = cs
    def world_border(i, j, bmesh_):
        shape, k = placed[(i, j)]
        pts = set()
        for f in bmesh_[shape].bm.faces:
            for l in f.loops:
                p = rot(tuple(l.vert.co), k)
                pts.add((round(p[0] + i * 67.0, 3), round(p[1] + j * 67.0, 3), round(p[2], 3)))
        return pts
    cache = {key: world_border(key[0], key[1], builders) for key in placed}
    pairs = mism = 0; compared = 0
    for (i, j) in placed:
        for (di, dj) in ((1, 0), (0, 1)):
            q = (i + di, j + dj)
            if q not in placed: continue
            pairs += 1
            if di: line = lambda p: abs(p[0] - (i + 0.5) * 67.0) < 2e-3
            else: line = lambda p: abs(p[1] - (j + 0.5) * 67.0) < 2e-3
            a = {p for p in cache[(i, j)] if line(p) and p[2] >= 99.9}      # top vertices (plate tops, rim)
            c = {p for p in cache[q] if line(p) and p[2] >= 99.9}
            ac = {p for p in cache[(i, j)] if line(p) and abs(p[2] - CAP_Z) < 1e-3}
            cc = {p for p in cache[q] if line(p) and abs(p[2] - CAP_Z) < 1e-3}
            compared += len(a) + len(ac)
            if a != c or ac != cc: mism += 1
    report["seam_check"] = {"method": "all 5 broken shapes placed with the marching-squares rotation over the 8 x 8 layout of rock_slab_pieces.py (%d pieces); for each adjacent pair the vertices on the shared border line at z = 100 (plate tops, rim) and at the cap height are compared as sets (1 mm rounding)" % len(placed),
                            "adjacent_pairs": pairs, "border_vertices_compared": compared, "mismatching_pairs": mism, "pass": mism == 0,
                            "guarantee": "fixed grid border vertices (-33.5, -12, 0, 12, 33.5 on every side, no jitter) + intact outline crossing at the border middle; plate vertices on a border are never inset"}
    if mism: report["warnings"].append("seam mismatch")
    report["fbx_export"] = {"apply_scale_options": "FBX_SCALE_UNITS", "global_scale": 1.0, "axes": "Blender default", "colors_type": "LINEAR", "mesh_smooth_type": "FACE"}
    report["sag_preview"] = {"note": "simulation only (the material does the real sag by WPO)", "rule": "sag = SAG_MAX x D x f; D = bilinear damage of the 4 corner cells at the vertex (empty corner cells take the max damage of their solid 4-neighbours), f = (1 - min(dE/100, 1))^2 with dE = distance to the nearest empty cell; applied to vertices whose original z >= 80 (plates, rim, top row of the walls), shifted down by min(sag, 40) so the top stays >= 60",
                             "SAG_MAX": SAG_MAX, "layout_rows_top_to_bottom": LAYOUT, "damage": {"%d,%d" % k: v for k, v in DAMAGE.items()}}
    if PREVIEW:
        report["preview"] = previews(builders, intact)
    with open(os.path.join(OUTDIR, "SM_RockSlabBroken.report.json"), "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print(json.dumps({"tris": {s: report["shapes"][s]["tris"] for s in SHAPES}, "seam": report["seam_check"]["pass"], "warnings": report["warnings"]}))


# ---------------------------------------------------------------- previews (same camera and light as rock_slab_pieces.py)
def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


def mat_intact():
    m = bpy.data.materials.new("PV_intact"); m.use_nodes = True
    nt = m.node_tree; bsdf = nt.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 0.9
    at = nt.nodes.new("ShaderNodeAttribute"); at.attribute_name = "Color"; at.attribute_type = "GEOMETRY"
    sep = nt.nodes.new("ShaderNodeSeparateColor"); nt.links.new(at.outputs["Color"], sep.inputs["Color"])
    m1 = nt.nodes.new("ShaderNodeMix"); m1.data_type = "RGBA"; m1.inputs[6].default_value = srgb("#8E89A8"); m1.inputs[7].default_value = srgb("#C4BFDB")
    nt.links.new(sep.outputs["Red"], m1.inputs[0])
    m2 = nt.nodes.new("ShaderNodeMix"); m2.data_type = "RGBA"; m2.inputs[7].default_value = srgb("#55516F")
    nt.links.new(m1.outputs[2], m2.inputs[6]); nt.links.new(sep.outputs["Green"], m2.inputs[0])
    nt.links.new(m2.outputs[2], bsdf.inputs["Base Color"])
    return m


def mat_broken():
    """R=1 plate tops/rim = rock top tone, G=1 edges/walls = side tone, B=1 base cap = dark."""
    m = bpy.data.materials.new("PV_broken"); m.use_nodes = True
    nt = m.node_tree; bsdf = nt.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 0.9
    at = nt.nodes.new("ShaderNodeAttribute"); at.attribute_name = "Color"; at.attribute_type = "GEOMETRY"
    sep = nt.nodes.new("ShaderNodeSeparateColor"); nt.links.new(at.outputs["Color"], sep.inputs["Color"])
    m1 = nt.nodes.new("ShaderNodeMix"); m1.data_type = "RGBA"; m1.inputs[6].default_value = srgb("#55516F"); m1.inputs[7].default_value = srgb("#8E89A8")
    nt.links.new(sep.outputs["Red"], m1.inputs[0])
    m2 = nt.nodes.new("ShaderNodeMix"); m2.data_type = "RGBA"; m2.inputs[7].default_value = srgb("#55516F")
    nt.links.new(m1.outputs[2], m2.inputs[6]); nt.links.new(sep.outputs["Green"], m2.inputs[0])
    m3 = nt.nodes.new("ShaderNodeMix"); m3.data_type = "RGBA"; m3.inputs[7].default_value = srgb("#25223A")
    nt.links.new(m2.outputs[2], m3.inputs[6]); nt.links.new(sep.outputs["Blue"], m3.inputs[0])
    nt.links.new(m3.outputs[2], bsdf.inputs["Base Color"])
    return m


def setup_render(w, h):
    sc = bpy.context.scene
    for nm in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = nm; break
        except TypeError:
            continue
    sc.render.resolution_x, sc.render.resolution_y = w, h
    sc.render.image_settings.file_format = "PNG"
    sc.view_settings.view_transform = "Standard"; sc.view_settings.look = "None"
    wd = bpy.data.worlds.new("W"); wd.use_nodes = True; sc.world = wd
    bgn = wd.node_tree.nodes["Background"]; bgn.inputs["Color"].default_value = srgb("#C8C8CC"); bgn.inputs["Strength"].default_value = 0.35
    return sc


def aim(cam, target, pitch_deg, azim_deg, dist):
    p, a = math.radians(pitch_deg), math.radians(azim_deg)
    d = Vector((math.sin(a) * math.cos(p), math.cos(a) * math.cos(p), -math.sin(p)))
    cam.location = Vector(target) - d * dist
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()


def render_one(sc, path, cam_spec):
    cam_d = bpy.data.cameras.new("Cam"); cam_d.clip_start = 1; cam_d.clip_end = 6000
    cam_d.sensor_fit = "HORIZONTAL"; cam_d.lens_unit = "FOV"; cam_d.angle = math.radians(cam_spec["hfov"])
    cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam)
    aim(cam, cam_spec["target"], cam_spec["pitch"], cam_spec["azim"], cam_spec["dist"])
    sc.camera = cam
    sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 1.0
    sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
    sun.rotation_euler = cam.rotation_euler.copy()
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    bpy.data.objects.remove(cam); bpy.data.objects.remove(sun)


def layout_object(name, items, sagger=None, mat=None):
    """items: (builder, k, cx, cy); sagger(x, y) -> sag or None. Vertices of original z >= 80 move down by min(sag, 40)."""
    bm = bmesh.new(); col = bm.loops.layers.float_color.new("Color")
    for src_b, k, cx, cy in items:
        scol = src_b.bm.loops.layers.float_color["Color"]
        for f in src_b.bm.faces:
            vs = []
            for l in f.loops:
                p = rot(tuple(l.vert.co), k)
                x, y, z = p[0] + cx, p[1] + cy, p[2]
                if sagger and z >= 80.0:
                    z -= min(sagger(x, y), TOP - MIN_TOP)
                vs.append(bm.verts.new((x, y, z)))
            try:
                nf = bm.faces.new(vs)
            except ValueError:
                continue
            nf.smooth = False
            for nl, l in zip(nf.loops, f.loops):
                nl[col] = l[scol]
    me = bpy.data.meshes.new(name); bm.to_mesh(me); bm.free()
    ob = bpy.data.objects.new(name, me); bpy.context.scene.collection.objects.link(ob)
    me.materials.append(mat)
    return ob


def previews(builders, intact):
    os.makedirs(PREV_DIR, exist_ok=True)
    mi, mb = mat_intact(), mat_broken()
    floor_m = bpy.data.materials.new("FLOOR"); floor_m.use_nodes = True
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = srgb("#B9B9BE")
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 1.0
    fl = bpy.data.meshes.new("Floor"); fl.from_pydata([(-3000, -3000, 0), (4000, -3000, 0), (4000, 4000, 0), (-3000, 4000, 0)], [], [(0, 1, 2, 3)])
    flo = bpy.data.objects.new("Floor", fl); bpy.context.scene.collection.objects.link(flo); fl.materials.append(floor_m)
    out = {}
    sc = setup_render(1920, 1080)
    # --- layout with simulated sag
    solid, dmg, empties = make_world()
    rows = LAYOUT[::-1]
    def cell(c, r): return solid(c, r)
    placed = {}
    for j in range(9):
        for i in range(9):
            m = cell(i - 1, j - 1) | cell(i, j - 1) << 1 | cell(i, j) << 2 | cell(i - 1, j) << 3
            cs = case_of(m)
            if cs: placed[(i, j)] = cs
    brk, itc = [], []
    for (i, j), (shape, k) in placed.items():
        cd = [DAMAGE.get((i - 1, j - 1), 0), DAMAGE.get((i, j - 1), 0), DAMAGE.get((i, j), 0), DAMAGE.get((i - 1, j), 0)]
        (brk if any(cd) else itc).append((builders[shape] if any(cd) else intact[shape], k, i * 67.0, j * 67.0))
    sagger = lambda x, y: sag_at(x, y, solid, dmg, empties)
    ob_i = layout_object("LayI", itc, None, mi)
    ob_b = layout_object("LayB", brk, sagger, mb)
    out["broken_pieces_in_layout"] = len(brk); out["intact_pieces_in_layout"] = len(itc)
    cen = (268.0, 268.0, 0.0)
    for nm, spec in (("rockbroken_layout_game_cam.png", {"hfov": 90.0, "pitch": 60.0, "azim": 0.0, "target": cen, "dist": 640.0}),
                     ("rockbroken_layout_three_quarter.png", {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": cen, "dist": 1050.0})):
        p = os.path.join(PREV_DIR, nm); render_one(sc, p, spec); out[nm[:-4]] = p
    # a closer look at the damaged row junction
    p = os.path.join(PREV_DIR, "rockbroken_row_closeup.png")
    render_one(sc, p, {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": (168.0, 300.0, 60.0), "dist": 520.0}); out["row_closeup"] = p
    bpy.data.objects.remove(ob_i); bpy.data.objects.remove(ob_b)
    # --- the 5 pieces alone, no sag and full sag (d = 1 everywhere), three-quarter
    sc = setup_render(1600, 900)
    for shape in SHAPES:
        for tag, sg in (("nosag", None), ("fullsag", lambda x, y: SAG_MAX)):
            o = layout_object("P", [(builders[shape], 0, 0.0, 0.0)], sg, mb)
            p = os.path.join(PREV_DIR, "rockbroken_%s_%s.png" % (shape, tag))
            render_one(sc, p, {"hfov": 40.0, "pitch": 35.0, "azim": 32.0, "target": (0, 0, 40.0), "dist": 330.0}); out["%s_%s" % (shape, tag)] = p
            bpy.data.objects.remove(o)
    out["camera"] = "layout hfov 90 pitch 60 azim 0 dist 640 (game) and hfov 50 pitch 38 azim 28 dist 1050 (three-quarter), target (268,268,0); same camera-following sun and world as the slab previews"
    return out


main()
