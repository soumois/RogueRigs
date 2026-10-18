"""Broken variants of the rock slab dual-grid pieces (task-230-06). Blender route, deterministic, no Unreal import.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/rock_slab_broken.py -- [--out-dir Art/Meshes/rock-slab] [--no-preview]
Writes SM_RockSlabBroken_<Shape>_01.fbx and _02.fbx (two different cuts), SM_RockSlabBroken.report.json and previews.
The constants, outlines, rim band and side walls come from rock_slab_pieces.py itself (its source is executed up to main(), so
every border crossing point is identical to the intact pieces). Only the top body is replaced:
  - top cut into thick angular plates of different sizes and shapes (random chords through the square, per shape and variant
    seed); the only vertices allowed on a piece border are -H, -12, 0, 12, H on every side, so neighbours match exactly;
    each plate is a prism PLATE_T thick with ~GAP gaps to its neighbours;
  - rim band copied unchanged from the intact piece, coded R = 0.5 (plate tops R = 1); side walls G = 1; base cap B = 1 at CAP_Z;
  - per plate rigid-motion data in UV2 (PlateA) and UV3 (PlateB), see CHANNELS.
The sag is NOT modelled: the material does it (WPO). It is simulated here for the previews only, plate by plate (rigid)."""
import os, sys
_src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "rock_slab_pieces.py"), encoding="utf-8").read()
exec(compile(_src[:_src.index("def main():")], "rock_slab_pieces.py(prefix)", "exec"))   # H, TOP, MASKS, CANON, outline_pts, build_piece, finish, rot, case_of ...

PLATE_T = 12.0      # plate thickness (uu)
GAP = 1.8           # gap between plates (uu), each free vertex moves GAP/2 toward its plate centre
CAP_Z = 58.0        # base cap height (minimum top is 60)
MIN_TOP = 60.0
BORDER_PTS = (-12.0, 0.0, 12.0)
BUDGET = (70, 230)   # owner: about 80-200
SHAPES = list(MASKS)
VARIANTS = (1, 2)
NAME = "SM_RockSlabBroken_%s_%02d"
EPS = 1e-4
N_CUTS = 9          # chords per piece (8 polygons before clipping)
MIN_CELL = 80.0     # smallest cell area (uu^2)

CHANNELS = {
    "UV0 'UVMap'": "planar top-down, (x/100, y/100) in piece-local XY",
    "UV1 'LightMap'": "per-triangle packing, no overlap, 2 px border at 256",
    "UV2 'PlateA'": "(plate centre X / 100, plate centre Y / 100) in piece-local XY (before the instance rotation), identical on every vertex of a plate (top and sides); (0, 0) on rim, walls and cap",
    "UV3 'PlateB'": "(plate seed 0..1, plate radius / 100) with radius = largest distance centre -> plate vertex; (-1, 0) on rim, walls and cap: UV3.x < 0 means 'not a plate'",
    "vertex colour 'Color'": "R = 1 plate tops, R = 0.5 rim band, G = 1 plate edges and side walls, B = 1 base cap (all others 0); read R with thresholds 0.25 / 0.75 (export quantises to 8 bit)",
    "import": "enable full precision UVs in the Unreal static mesh import (UV2 holds values in [-0.5, 0.5], half precision would give ~0.05 uu error); keep 4 UV channels",
}


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


def border_edge(p, q):
    """edge lying on one of the 4 piece border lines"""
    return any(abs(p[a] - sgn) < EPS and abs(q[a] - sgn) < EPS for a in (0, 1) for sgn in (-H, H))


def random_cut(seed):
    """Irregular convex cells: random chords through the square. A chord end on a piece border is snapped to a fixed
    border point (-12, 0, 12); on an interior edge it is free. Same seed -> same cut."""
    r = random.Random(seed)
    cells = [[(-H, -H), (H, -H), (H, H), (-H, H)]]
    tries = 0
    while len(cells) < N_CUTS + 1 and tries < 400:
        tries += 1
        w = [max(area(c), 1.0) ** 1.5 for c in cells]
        ci = r.choices(range(len(cells)), weights=w)[0]
        poly = cells[ci]
        n = len(poly)
        i, j = sorted(r.sample(range(n), 2))
        pts = []
        ok = True
        for e in (i, j):
            p, q = poly[e], poly[(e + 1) % n]
            if border_edge(p, q):
                ax = 0 if abs(p[0] - q[0]) < EPS else 1
                o = 1 - ax
                lo, hi = sorted((p[o], q[o]))
                cand = [t for t in BORDER_PTS if lo + 1.0 < t < hi - 1.0]
                if not cand: ok = False; break
                t = r.choice(cand)
                m = [0.0, 0.0]; m[ax] = p[ax]; m[o] = t
                pts.append(tuple(m))
            else:
                t = r.uniform(0.25, 0.75)
                pts.append((p[0] + t * (q[0] - p[0]), p[1] + t * (q[1] - p[1])))
        if not ok: continue
        A, B = pts
        c1 = [A] + [poly[(i + 1 + k) % n] for k in range(j - i)] + [B]
        c2 = [B] + [poly[(j + 1 + k) % n] for k in range(n - (j - i))] + [A]
        c1, c2 = dedupe(c1), dedupe(c2)
        if len(c1) < 3 or len(c2) < 3 or area(c1) < MIN_CELL or area(c2) < MIN_CELL: continue
        cells[ci] = c1; cells.append(c2)
    return [ccw(c) for c in cells]


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
def build_broken(shape, variant):
    ib = build_piece(shape)                         # intact piece: rim band, side walls and the body triangles
    seed = {"full": 1, "edge": 2, "outer": 3, "inner": 4, "diagonal": 5}[shape] * 7919
    b = Builder("broken_" + shape, seed)
    b.uv2 = b.bm.loops.layers.uv.new("PlateA")
    b.uv3 = b.bm.loops.layers.uv.new("PlateB")
    rng = random.Random(seed * 31 + variant * 104729)
    def setplate(f, cx, cy, sd, rad):
        for l in f.loops:
            l[b.uv2].uv = (cx / 100.0, cy / 100.0)
            l[b.uv3].uv = (sd, rad / 100.0)
    def nonplate(f):
        for l in f.loops:
            l[b.uv2].uv = (0.0, 0.0)
            l[b.uv3].uv = (-1.0, 0.0)
    body = []
    for f in ib.kind_faces["top"]:
        pts = [(l.vert.co.x, l.vert.co.y) for l in f.loops]
        for k in range(1, len(pts) - 1):
            body.append(ccw([pts[0], pts[k], pts[k + 1]]))
    for f in ib.kind_faces.get("rim", []):
        nonplate(b.face([tuple(l.vert.co) for l in f.loops], (0.5, 0, 0), up=1, kind="rim"))
    for f in ib.kind_faces.get("side", []):
        nonplate(b.face([tuple(l.vert.co) for l in f.loops], (0, 1, 0), out=Vector((f.normal.x, f.normal.y, 0)), kind="side"))
    cnt = {}
    for t in body:
        for p, q in zip(t, t[1:] + t[:1]):
            k = tuple(sorted(((round(p[0], 4), round(p[1], 4)), (round(q[0], 4), round(q[1], 4)))))
            cnt[k] = cnt.get(k, 0) + 1
    bedges = [k for k, v in cnt.items() if v == 1]
    def on_boundary(p):
        return any(seg_dist(p, a, c) < EPS for a, c in bedges)
    cap = []
    for f in ib.kind_faces.get("rim", []):
        pts = [(l.vert.co.x, l.vert.co.y) for l in f.loops]
        for k in range(1, len(pts) - 1):
            cap.append([pts[0], pts[k], pts[k + 1]])
    cap += body
    for t in cap:
        nonplate(b.face([(p[0], p[1], CAP_Z) for p in t], (0, 0, 1), up=1, kind="cap"))
    b.plates = []
    for cell in random_cut(seed + variant * 7777):
        frags = []
        for t in body:
            fr = dedupe(clip_convex(cell, t))
            if len(fr) >= 3 and abs(area(fr)) > 6.0:
                frags.append(ccw(fr))
        for fr in merge_fragments(frags):
            fr = dedupe(insert_border_pts(fr))
            keep = []
            for a_, p_, c_ in zip(fr[-1:] + fr[:-1], fr, fr[1:] + fr[:1]):
                col_ = abs((p_[0] - a_[0]) * (c_[1] - a_[1]) - (p_[1] - a_[1]) * (c_[0] - a_[0])) < 1e-6
                if col_ and not on_border(p_) and not on_boundary(p_): continue
                keep.append(p_)
            fr = keep
            if len(fr) < 3: continue
            A_ = area(fr); cx = sum((p[0] + q[0]) * (p[0] * q[1] - q[0] * p[1]) for p, q in zip(fr, fr[1:] + fr[:1])) / (6 * A_)
            cy = sum((p[1] + q[1]) * (p[0] * q[1] - q[0] * p[1]) for p, q in zip(fr, fr[1:] + fr[:1])) / (6 * A_)
            pts = []
            for p in fr:
                if on_border(p) or on_boundary(p):
                    pts.append(p); continue
                d = math.hypot(cx - p[0], cy - p[1])
                s = min(GAP / 2, d * 0.4) / d if d > 1e-6 else 0.0
                pts.append((p[0] + (cx - p[0]) * s, p[1] + (cy - p[1]) * s))
            n = len(pts)
            sd = rng.random()
            rad = max(math.hypot(p[0] - cx, p[1] - cy) for p in pts)
            b.plates.append((cx, cy, sd, rad))
            for tri_ in ear_clip(pts):
                setplate(b.face([(v.x, v.y, TOP) for v in tri_], (1, 0, 0), up=1, kind="plate_top"), cx, cy, sd, rad)
            for k in range(n):
                p, q = pts[k], pts[(k + 1) % n]
                if border_edge(p, q):
                    continue                  # edge on a piece border: the neighbour plate/piece is there
                quad = [(p[0], p[1], TOP), (q[0], q[1], TOP), (q[0], q[1], TOP - PLATE_T), (p[0], p[1], TOP - PLATE_T)]
                outv = Vector(((p[0] + q[0]) / 2 - cx, (p[1] + q[1]) / 2 - cy, 0))
                for tri_ in ([quad[0], quad[1], quad[2]], [quad[0], quad[2], quad[3]]):
                    setplate(b.face(tri_, (0, 1, 0), out=outv, kind="plate_side"), cx, cy, sd, rad)
    b.nplates = len(b.plates)
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
    intact = [(c, r) for r in range(Hh) for c in range(W) if solid(c, r) and DAMAGE.get((c, r), 0.0) == 0.0]
    return solid, dmg, empties, intact


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


def plate_motion(cx, cy, sd, rad, world):
    """rigid motion of a plate as the material will compute it. Returns (hinge point, dir to the nearest intact cell, slope, height drop).
    g = damage x void factor at the plate centre; hinge = side of the plate facing the nearest intact cell, so the plate stays level
    there and tilts down away from it; seed -> extra random tilt and a random height drop, both scaled by g (0 next to intact rock)."""
    solid, dmg, empties, intact = world
    g = sag_at(cx, cy, solid, dmg, empties) / SAG_MAX
    s2 = (math.sin(sd * 127.1 + 311.7) * 43758.5453) % 1.0
    best = min(intact, key=lambda c: math.hypot((c[0] + 0.5) * 67.0 - cx, (c[1] + 0.5) * 67.0 - cy)) if intact else (0, 0)
    dx, dy = (best[0] + 0.5) * 67.0 - cx, (best[1] + 0.5) * 67.0 - cy
    L = math.hypot(dx, dy) or 1.0
    d = (dx / L, dy / L)
    k = min(1.0, 2.0 * g)
    total = SAG_MAX * g * (0.7 + 0.6 * s2)
    slope = min(total / (2.0 * rad), 0.75) + (sd - 0.5) * 0.45 * k
    slope = max(slope, 0.02 * g)
    hoff = s2 * 9.0 * k
    ph = (cx + d[0] * rad, cy + d[1] * rad)
    return ph, d, slope, hoff


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


def border_set(bm):
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
    report = {"task": "task-230-06-rock-plate-overlay (broken pieces, rigid plates)", "script": "Tools/art/blender/rock_slab_broken.py", "units": "cm (1 uu = 1 cm)",
              "constants": {"plate_thickness": PLATE_T, "plate_gap": GAP, "cap_z": CAP_Z, "min_top": MIN_TOP, "border_vertices_per_side": [-H] + list(BORDER_PTS) + [H], "cuts_per_piece": N_CUTS},
              "channels": CHANNELS, "shapes": {}, "warnings": []}
    builders, intact = {}, {}
    for shape in SHAPES:
        ib = build_piece(shape); finish(ib); intact[shape] = ib
        for var in VARIANTS:
            b = build_broken(shape, var)
            finish(b)
            builders[(shape, var)] = b
            name = NAME % (shape.capitalize(), var)
            ob = to_object(b, name)
            bpy.ops.object.select_all(action="DESELECT"); ob.select_set(True)
            bpy.context.view_layer.objects.active = ob
            bpy.ops.export_scene.fbx(filepath=os.path.join(OUTDIR, name + ".fbx"), use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                                     mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
            me = ob.data
            tris = len(me.polygons)
            xs = [v.co.x for v in me.vertices]; ys = [v.co.y for v in me.vertices]; zs = [v.co.z for v in me.vertices]
            ca = me.color_attributes["Color"].data
            cnt = {"R1": 0, "R05": 0, "G": 0, "B": 0}; bad = 0
            for p in me.polygons:
                cs = {tuple(round(c, 3) for c in ca[li].color[:3]) for li in p.loop_indices}
                if len(cs) != 1: bad += 1; continue
                c = next(iter(cs))
                zc = [me.vertices[i].co.z for i in p.vertices]
                flat = max(zc) - min(zc) < 1e-4
                if c == (1.0, 0.0, 0.0): cnt["R1"] += 1; bad += 0 if flat and p.normal.z > 0 else 1
                elif c == (0.5, 0.0, 0.0): cnt["R05"] += 1; bad += 0 if flat and p.normal.z > 0 else 1
                elif c == (0.0, 1.0, 0.0): cnt["G"] += 1
                elif c == (0.0, 0.0, 1.0): cnt["B"] += 1; bad += 0 if flat and p.normal.z > 0 else 1
                else: bad += 1
            uvc = tri_stats(me)
            uv0 = me.uv_layers["UVMap"].data
            uv0_ok = all(abs(uv0[l.index].uv[0] - me.vertices[l.vertex_index].co.x / 100) < 1e-5 and abs(uv0[l.index].uv[1] - me.vertices[l.vertex_index].co.y / 100) < 1e-5 for l in me.loops)
            # plate data: one value per plate on all its faces; non-plate faces carry (0,0) / (-1,0)
            u2 = me.uv_layers["PlateA"].data; u3 = me.uv_layers["PlateB"].data
            plate_vals = {(round(u2[l].uv[0], 5), round(u2[l].uv[1], 5), round(u3[l].uv[0], 5), round(u3[l].uv[1], 5)) for l in range(len(u2)) if u3[l].uv[0] >= 0}
            nonplate_ok = all(u3[l].uv[0] == -1.0 and u2[l].uv[0] == 0.0 for l in range(len(u3)) if u3[l].uv[0] < 0)
            ib_set = border_set(intact[shape].bm); bb_set = border_set(b.bm)
            key = "%s_%02d" % (shape, var)
            report["shapes"][key] = {
                "file": "Art/Meshes/rock-slab/%s.fbx" % name, "tris": tris, "budget": list(BUDGET), "within_budget": BUDGET[0] <= tris <= BUDGET[1],
                "intact_tris": len(intact[shape].bm.faces), "plates": b.nplates, "plate_data_distinct": len(plate_vals),
                "dimensions_cm": [round(max(xs) - min(xs), 2), round(max(ys) - min(ys), 2), round(max(zs) - min(zs), 2)],
                "bounds_min": [round(min(xs), 2), round(min(ys), 2), round(min(zs), 2)], "bounds_max": [round(max(xs), 2), round(max(ys), 2), round(max(zs), 2)],
                "materials": ["M_RockSlab"], "uv_layers": [u.name for u in me.uv_layers],
                "uv_checks": {"UVMap_planar_xy_over_100": bool(uv0_ok), "LightMap": uvc, "plate_data_one_per_plate": len(plate_vals) == b.nplates, "nonplate_marker_ok": bool(nonplate_ok)},
                "vertex_colour": {"attribute": "Color (corner, float)", "export": "colors_type=LINEAR", "R1_plate_tops": cnt["R1"], "R0.5_rim": cnt["R05"], "G1_edges_and_walls": cnt["G"], "B1_base_cap": cnt["B"],
                                  "inconsistent_faces": bad, "pass": bad == 0},
                "intact_border_vertices_kept": ib_set <= bb_set,
                "all_flat_shaded": all(not p.use_smooth for p in me.polygons),
            }
            w = report["shapes"][key]
            if not w["within_budget"]: report["warnings"].append("%s tris %d outside %s" % (key, tris, list(BUDGET)))
            if not uvc["pass"]: report["warnings"].append("%s LightMap check failed" % key)
            if bad: report["warnings"].append("%s vertex colour" % key)
            if not w["intact_border_vertices_kept"]: report["warnings"].append("%s intact border vertices not kept" % key)
            if not (w["uv_checks"]["plate_data_one_per_plate"] and nonplate_ok): report["warnings"].append("%s plate UV data" % key)
            bpy.data.objects.remove(ob)

    # ---- seam check: all pieces broken, variants mixed, placed over the test layout of rock_slab_pieces.py
    TL = ["###..###", "###..###", "##.#....", "##..##..", ".....##.", "###.####", "###..#..", "###...#."]
    trows = TL[::-1]
    def tcell(c, r): return 1 if 0 <= c < 8 and 0 <= r < 8 and trows[r][c] == "#" else 0
    placed = {}
    for j in range(9):
        for i in range(9):
            m = tcell(i - 1, j - 1) | tcell(i, j - 1) << 1 | tcell(i, j) << 2 | tcell(i - 1, j) << 3
            cs = case_of(m)
            if cs: placed[(i, j)] = cs
    def world_border(i, j):
        shape, k = placed[(i, j)]
        pts = set()
        for f in builders[(shape, 1 + (i + j) % 2)].bm.faces:
            for l in f.loops:
                p = rot(tuple(l.vert.co), k)
                pts.add((round(p[0] + i * 67.0, 3), round(p[1] + j * 67.0, 3), round(p[2], 3)))
        return pts
    cache = {key: world_border(*key) for key in placed}
    pairs = mism = compared = 0
    for (i, j) in placed:
        for (di, dj) in ((1, 0), (0, 1)):
            q = (i + di, j + dj)
            if q not in placed: continue
            pairs += 1
            if di: line = lambda p: abs(p[0] - (i + 0.5) * 67.0) < 2e-3
            else: line = lambda p: abs(p[1] - (j + 0.5) * 67.0) < 2e-3
            a = {p for p in cache[(i, j)] if line(p) and p[2] >= 99.9}
            c = {p for p in cache[q] if line(p) and p[2] >= 99.9}
            ac = {p for p in cache[(i, j)] if line(p) and abs(p[2] - CAP_Z) < 1e-3}
            cc = {p for p in cache[q] if line(p) and abs(p[2] - CAP_Z) < 1e-3}
            compared += len(a) + len(ac)
            if a != c or ac != cc: mism += 1
    report["seam_check"] = {"method": "all 5 broken shapes, variants 01/02 alternating ((i+j) parity), placed with the marching-squares rotation over the 8 x 8 layout of rock_slab_pieces.py (%d pieces); for each adjacent pair the vertices on the shared border line at z = 100 (plate tops, rim) and at the cap height are compared as sets (1 mm rounding)" % len(placed),
                            "adjacent_pairs": pairs, "border_vertices_compared": compared, "mismatching_pairs": mism, "pass": mism == 0,
                            "guarantee": "the only vertices on a piece border are -33.5, -12, 0, 12, 33.5 (cut chords end there, missing ones are inserted), never inset; the outline crosses the border at the middle as in the intact pieces"}
    if mism: report["warnings"].append("seam mismatch")
    report["fbx_export"] = {"apply_scale_options": "FBX_SCALE_UNITS", "global_scale": 1.0, "axes": "Blender default", "colors_type": "LINEAR", "mesh_smooth_type": "FACE"}
    report["material_rigid_tilt"] = {
        "note": "simulated in the previews exactly like this; the material (WPO) must do the same, per vertex of a plate (UV3.x >= 0), from UV2/UV3 and world data",
        "inputs": "plate centre C = rot(UV2 * 100) + instance position (world XY); seed s = UV3.x; radius R = UV3.y * 100; g = D(C) * f(C) in 0..1 (D bilinear corner damage, f = (1 - min(dE/100,1))^2, dE distance to the nearest empty cell)",
        "hinge": "dir = unit vector from C toward the nearest intact solid cell centre; hinge point = C + dir * R, hinge axis = perpendicular to dir",
        "per_plate": "s2 = frac(sin(s*127.1 + 311.7) * 43758.5453); k = min(1, 2g); slope = max(min(SAG_MAX*g*(0.7+0.6*s2)/(2R), 0.75) + (s - 0.5)*0.45*k, 0.02*g); hoff = s2*9*k",
        "per_vertex": "u = dot(hinge - v_xy, dir) (distance behind the hinge, >= 0); drop = min(hoff + u*slope, TOP - 60); z = z - drop (same drop for top and bottom vertices of the plate: rigid tilt around the hinge)",
        "non_plate": "rim and the top row of the walls (UV3.x < 0, original z >= 80): per-vertex sag = SAG_MAX * D * f at the vertex, capped so z >= 60; cap and lower walls fixed",
        "SAG_MAX": SAG_MAX,
        "layout_rows_top_to_bottom": LAYOUT, "damage": {"%d,%d" % k: v for k, v in DAMAGE.items()}}
    if PREVIEW:
        report["preview"] = previews(builders, intact)
    with open(os.path.join(OUTDIR, "SM_RockSlabBroken.report.json"), "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print(json.dumps({"tris": {k: v["tris"] for k, v in report["shapes"].items()}, "seam": report["seam_check"]["pass"], "warnings": report["warnings"]}))


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
    """R = 1 plate top (top tone), R = 0.5 rim (light rim tone), G edges/walls, B base cap."""
    m = bpy.data.materials.new("PV_broken"); m.use_nodes = True
    nt = m.node_tree; bsdf = nt.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 0.9
    at = nt.nodes.new("ShaderNodeAttribute"); at.attribute_name = "Color"; at.attribute_type = "GEOMETRY"
    sep = nt.nodes.new("ShaderNodeSeparateColor"); nt.links.new(at.outputs["Color"], sep.inputs["Color"])
    def gt(v):
        n = nt.nodes.new("ShaderNodeMath"); n.operation = "GREATER_THAN"; n.inputs[1].default_value = v
        nt.links.new(sep.outputs["Red"], n.inputs[0]); return n
    rimf, topf = gt(0.25), gt(0.75)
    m1 = nt.nodes.new("ShaderNodeMix"); m1.data_type = "RGBA"; m1.inputs[6].default_value = srgb("#55516F"); m1.inputs[7].default_value = srgb("#C4BFDB")
    nt.links.new(rimf.outputs[0], m1.inputs[0])
    m1b = nt.nodes.new("ShaderNodeMix"); m1b.data_type = "RGBA"; m1b.inputs[7].default_value = srgb("#8E89A8")
    nt.links.new(m1.outputs[2], m1b.inputs[6]); nt.links.new(topf.outputs[0], m1b.inputs[0])
    m2 = nt.nodes.new("ShaderNodeMix"); m2.data_type = "RGBA"; m2.inputs[7].default_value = srgb("#55516F")
    nt.links.new(m1b.outputs[2], m2.inputs[6]); nt.links.new(sep.outputs["Green"], m2.inputs[0])
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


def layout_object(name, items, world=None, mat=None):
    """items: (builder, k, cx, cy). world=None: no sag. Plates (UV3.x >= 0) move rigidly (plate_motion); rim and wall top row
    (original z >= 80, not plates) get the per-vertex sag."""
    bm = bmesh.new(); col = bm.loops.layers.float_color.new("Color")
    cache = {}
    for src_b, k, cx, cy in items:
        scol = src_b.bm.loops.layers.float_color["Color"]
        has_plate = hasattr(src_b, "uv3")
        for f in src_b.bm.faces:
            plate = None
            if world and has_plate:
                l0 = f.loops[0]
                a, c = l0[src_b.uv2].uv, l0[src_b.uv3].uv
                if c[0] >= 0:
                    pc = rot((a[0] * 100.0, a[1] * 100.0), k)
                    wx, wy = pc[0] + cx, pc[1] + cy
                    key = (round(wx, 3), round(wy, 3), round(c[0], 5))
                    if key not in cache:
                        cache[key] = plate_motion(wx, wy, c[0], c[1] * 100.0, world)
                    plate = cache[key]
            vs = []
            for l in f.loops:
                p = rot(tuple(l.vert.co), k)
                x, y, z = p[0] + cx, p[1] + cy, p[2]
                if world:
                    if plate is not None:
                        ph, d, slope, hoff = plate
                        u = max((ph[0] - x) * d[0] + (ph[1] - y) * d[1], 0.0)
                        z -= min(hoff + u * slope, TOP - MIN_TOP)
                    elif z >= 80.0:
                        z -= min(sag_at(x, y, world[0], world[1], world[2]), TOP - MIN_TOP)
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
    world = make_world()
    solid = world[0]
    placed = {}
    for j in range(9):
        for i in range(9):
            m = solid(i - 1, j - 1) | solid(i, j - 1) << 1 | solid(i, j) << 2 | solid(i - 1, j) << 3
            cs = case_of(m)
            if cs: placed[(i, j)] = cs
    brk, itc = [], []
    for (i, j), (shape, k) in placed.items():
        cd = [DAMAGE.get((i - 1, j - 1), 0), DAMAGE.get((i, j - 1), 0), DAMAGE.get((i, j), 0), DAMAGE.get((i - 1, j), 0)]
        if any(cd): brk.append((builders[(shape, 1 + (i * 3 + j) % 2)], k, i * 67.0, j * 67.0))
        else: itc.append((intact[shape], k, i * 67.0, j * 67.0))
    ob_i = layout_object("LayI", itc, None, mi)
    ob_b = layout_object("LayB", brk, world, mb)
    out["broken_pieces_in_layout"] = len(brk); out["intact_pieces_in_layout"] = len(itc)
    cen = (268.0, 268.0, 0.0)
    for nm, spec in (("rockbroken_layout_game_cam.png", {"hfov": 90.0, "pitch": 60.0, "azim": 0.0, "target": cen, "dist": 640.0}),
                     ("rockbroken_layout_three_quarter.png", {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": cen, "dist": 1050.0}),
                     ("rockbroken_row_closeup.png", {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": (168.0, 300.0, 60.0), "dist": 520.0}),
                     ("rockbroken_row_closeup_game_cam.png", {"hfov": 60.0, "pitch": 60.0, "azim": 0.0, "target": (168.0, 300.0, 60.0), "dist": 420.0}),
                     ("rockbroken_pair_closeup.png", {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": (440.0, 130.0, 60.0), "dist": 420.0})):
        p = os.path.join(PREV_DIR, nm); render_one(sc, p, spec); out[nm[:-4]] = p
    bpy.data.objects.remove(ob_i); bpy.data.objects.remove(ob_b)
    # all 5 shapes, both cuts, no sag, three-quarter
    items = []
    for vi, var in enumerate(VARIANTS):
        for si, shape in enumerate(SHAPES):
            items.append((builders[(shape, var)], 0, si * 100.0, -vi * 100.0))
    o = layout_object("Cuts", items, None, mb)
    p = os.path.join(PREV_DIR, "rockbroken_cuts_nosag.png")
    render_one(sc, p, {"hfov": 50.0, "pitch": 55.0, "azim": 0.0, "target": (200.0, -50.0, 40.0), "dist": 700.0}); out["cuts_nosag"] = p
    bpy.data.objects.remove(o)
    out["camera"] = "layout hfov 90 pitch 60 azim 0 dist 640 (game) and hfov 50 pitch 38 azim 28 dist 1050 (three-quarter), target (268,268,0); same camera-following sun and world as the slab previews"
    return out


main()
