"""Broken variants of the rock slab dual-grid pieces, version 4: fractured volume (task-230-06). Blender route, deterministic, no Unreal import.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/rock_slab_broken.py -- [--out-dir Art/Meshes/rock-slab] [--no-preview]
Writes SM_RockSlabBroken_<Shape>_01.fbx and _02.fbx (two different cuts), SM_RockSlabBroken.report.json and previews.
The constants, outlines and rim band come from rock_slab_pieces.py itself (its source is executed up to main(), so the outline and
every border crossing point are those of the intact pieces). The whole footprint of the piece is cut like a Voronoi diagram into
solid blocks (about 10 per 67 x 67 cell of area, a minimum inscribed size, seeds retried until no sliver is left):
  - each block is a closed prism: top at z = 100, vertical faces, bottom at z = 0; along the void outline the foot of the wall follows
    the foot of the facets of the intact piece of the same variant number (task-480: 9 uu at the borders, so the outline and its border
    end points are the intact ones); the broken top keeps the flat 4 uu rim footprint of task-230, so the cut does not change;
  - the only vertices allowed on a piece border are -H, -12, 0, 12, H on every side (the cut snaps to them), plus the rim ends
    already on the intact border, so neighbouring pieces match exactly; border faces exist too, so a block is closed on its own;
  - the rim band is part of the block top: every top is (0, 0, 0) like an intact top (task-480, no chamfer glow mark), side faces G = 1 with the block seed in R (facet tint of M_RockSlab), bottoms B = 1;
  - per block rigid-motion data in UV2 (BlockA), UV3 (BlockB), UV4 (BlockC), see CHANNELS.
The sag is NOT modelled: the material does it (WPO). It is simulated here for the previews only, block by block (rigid)."""
import os, sys
_src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "rock_slab_pieces.py"), encoding="utf-8").read()
exec(compile(_src[:_src.index("def main():")], "rock_slab_pieces.py(prefix)", "exec"))   # H, TOP, MASKS, CANON, outline_pts, build_piece, finish, rot, case_of ...
from mathutils.bvhtree import BVHTree

MIN_TOP = 60.0
FLARE = ROW_OFF[2]       # outward push of the wall foot along the void outline (9 uu, as the intact pieces)
BORDER_PTS = (-12.0, 0.0, 12.0)
SNAP = (-H, -12.0, 0.0, 12.0, H)
BUDGET_TRIS = (300, 450)   # owner estimate, provisional
SHAPES = list(MASKS)
VARIANTS = (1, 2)
NAME = "SM_RockSlabBroken_%s_%02d"
EPS = 1e-4
CELL_AREA = 67.0 * 67.0
BLOCKS_PER_CELL = 11.0     # target density (owner: about 8 to 12 per cell)
MIN_BLOCKS = 3
ACCEPT_INR = 6.0           # a seed is accepted at once when every block has an inscribed circle of at least this radius (uu)
HARD_INR = 4.5             # minimum inscribed radius ever accepted: no block narrower than 9 uu
MAX_TRY = 120

CHANNELS = {
    "UV0 'UVMap'": "planar top-down, (x/100, y/100) in piece-local XY",
    "UV1 'LightMap'": "per-triangle packing, no overlap, 2 px border at 256",
    "UV2 'BlockA'": "(block centre X / 100, block centre Y / 100) in piece-local XY (before the instance rotation) = the block pivot position; identical on every vertex of a block (top, sides, bottom)",
    "UV3 'BlockB'": "(block seed 0..1, block radius / 100) with radius = largest distance centre -> block top vertex; identical on every vertex of a block; every face belongs to a block (UV3.x >= 0 everywhere)",
    "UV4 'BlockC'": "(rim flag, block index / 16): rim flag 1 = the block touches the void outline (its wall faces the void and slumps), 0 = inner block; index = order of the block in the piece",
    "vertex colour 'Color'": "(0, 0, 0) block tops including the rim band (no chamfer mark, task-480), G = 1 side faces with R = block seed (facet tint), B = 1 bottoms",
    "import": "enable full precision UVs in the Unreal static mesh import (UV2 holds values in [-0.5, 0.5], half precision would give ~0.05 uu error); keep 5 UV channels (UV0..UV4); the importer's auto lightmap channel must not overwrite UV4",
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


def rnd(poly):
    return [(round(p[0], 4) + 0.0, round(p[1], 4) + 0.0) for p in poly]


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


def cross3(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def on_border(p):
    return abs(abs(p[0]) - H) < EPS or abs(abs(p[1]) - H) < EPS


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


def snap_pt(p):
    x, y = p
    if abs(abs(x) - H) < 1e-6:
        x = math.copysign(H, x); y = min(SNAP, key=lambda t: abs(t - y))
    if abs(abs(y) - H) < 1e-6:
        y = math.copysign(H, y); x = min(SNAP, key=lambda t: abs(t - x))
    return (x, y)


def halfplane(poly, a, b):
    """keep the part of `poly` closer to site a than to site b"""
    m = ((a[0] + b[0]) / 2, (a[1] + b[1]) / 2); n = (a[0] - b[0], a[1] - b[1])
    def s(p): return (p[0] - m[0]) * n[0] + (p[1] - m[1]) * n[1]
    out = []
    for i in range(len(poly)):
        p, q = poly[i], poly[(i + 1) % len(poly)]
        sp, sq = s(p), s(q)
        if sp >= 0: out.append(p)
        if (sp >= 0) != (sq >= 0):
            t = sp / (sp - sq)
            out.append((p[0] + t * (q[0] - p[0]), p[1] + t * (q[1] - p[1])))
    return out


def in_tri(p, t):
    return cross3(t[0], t[1], p) >= -1e-9 and cross3(t[1], t[2], p) >= -1e-9 and cross3(t[2], t[0], p) >= -1e-9


# ---------------------------------------------------------------- footprint of an intact piece
def footprint(shape, variant=1):
    """convex triangles (CCW) of the body, of the rim band, and the outline chains with the foot displacement of the intact wall"""
    ib = build_piece(shape, variant, flat_rim=True)
    body, rim = [], []
    for f in ib.kind_faces["top"]:
        pts = [(l.vert.co.x, l.vert.co.y) for l in f.loops]
        for k in range(1, len(pts) - 1): body.append(ccw([pts[0], pts[k], pts[k + 1]]))
    for f in ib.kind_faces.get("rim", []):
        pts = [(l.vert.co.x, l.vert.co.y) for l in f.loops]
        for k in range(1, len(pts) - 1): rim.append(ccw([pts[0], pts[k], pts[k + 1]]))
    outl = []
    for pts, foot in ib.foot_chains:
        outl.append(([(p.x, p.y) for p in pts], [(f[0] - p.x, f[1] - p.y) for p, f in zip(pts, foot)]))
    return body, rim, outl


def foot_disp(p, outl):
    """horizontal push of the wall foot at a top point lying on the void outline (linear along each outline segment), else (0, 0)"""
    for pts, ds in outl:
        for i in range(len(pts) - 1):
            a, c = pts[i], pts[i + 1]
            if seg_dist(p, a, c) < 2e-3:
                L = math.hypot(c[0] - a[0], c[1] - a[1])
                t = min(1.0, math.hypot(p[0] - a[0], p[1] - a[1]) / L) if L > 0 else 0.0
                return (ds[i][0] + (ds[i + 1][0] - ds[i][0]) * t, ds[i][1] + (ds[i + 1][1] - ds[i][1]) * t)
    return (0.0, 0.0)


# ---------------------------------------------------------------- Voronoi blocks
def merge_convex(frs):
    """merge fragments (same kind) sharing an identical edge when the union stays convex"""
    def same(a, b): return math.hypot(a[0] - b[0], a[1] - b[1]) < 1e-4
    def convex(poly):
        n = len(poly)
        return all(cross3(poly[i - 1], poly[i], poly[(i + 1) % n]) >= -1e-6 for i in range(n))
    frs = [[list(f[0]), f[1]] for f in frs]
    changed = True
    while changed:
        changed = False
        for ia in range(len(frs)):
            for ib in range(ia + 1, len(frs)):
                A, B = frs[ia][0], frs[ib][0]
                if frs[ia][1] != frs[ib][1]: continue
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
                    if convex(m):
                        frs[ia][0] = m; del frs[ib]; changed = True
                        break
            if changed: break
    return frs


def simplify(poly, prot):
    changed = True
    while changed and len(poly) > 3:
        changed = False
        for i in range(len(poly)):
            a, b, c = poly[i - 1], poly[i], poly[(i + 1) % len(poly)]
            if seg_dist(b, a, c) < 4e-3 and (round(b[0], 4), round(b[1], 4)) not in prot:
                poly = poly[:i] + poly[i + 1:]; changed = True; break
    return poly


def fan(poly):
    n = len(poly)
    best = max(range(n), key=lambda i: cross3(poly[i - 1], poly[i], poly[(i + 1) % n]))
    pts = poly[best:] + poly[:best]
    return [(pts[0], pts[k], pts[k + 1]) for k in range(1, n - 1) if abs(area([pts[0], pts[k], pts[k + 1]])) > 5e-3]


def weld(tris, prot, tol=0.05):
    """merge vertices closer than tol (near-parallel clip lines make 0.003 uu slivers); a protected or border vertex is the representative"""
    pts = sorted({p for t in tris for p in t[:3]}, key=lambda p: (p not in prot and not on_border(p), p))
    rep = {}
    for p in pts:
        for q in rep.values():
            if math.hypot(p[0] - q[0], p[1] - q[1]) < tol:
                rep[p] = q; break
        else:
            rep[p] = p
    out = []
    for t in tris:
        a, b, c = rep[t[0]], rep[t[1]], rep[t[2]]
        if a != b and b != c and a != c and area([a, b, c]) > 5e-3: out.append((a, b, c, t[3]))
    return out


def tfix(tris, extra=()):
    """split triangles whose edge carries another vertex of the block, so the top is a watertight triangulation of its outline"""
    V = list({p for t in tris for p in t[:3]} | {(round(p[0], 4) + 0.0, round(p[1], 4) + 0.0) for p in extra})
    out, work = [], list(tris)
    while work:
        t = work.pop()
        done = True
        for e in range(3):
            a, b, c = t[e], t[(e + 1) % 3], t[(e + 2) % 3]
            for v in V:
                if v == a or v == b or v == c: continue
                if seg_dist(v, a, b) < 2e-3 and math.hypot(v[0] - a[0], v[1] - a[1]) > 0.04 and math.hypot(v[0] - b[0], v[1] - b[1]) > 0.04:
                    for nt in ((a, v, c, t[3]), (v, b, c, t[3])):
                        if abs(area(list(nt[:3]))) > 5e-3: work.append(nt)
                    done = False; break
            if not done: break
        if done: out.append(t)
    return out


def make_block(cell, body, rim, outl):
    """cell: Voronoi polygon (CCW, border points not inserted, may be concave after snapping: it is cut into convex parts)"""
    ext = insert_border_pts(cell)
    prot = {(round(p[0], 4), round(p[1], 4)) for t in body + rim for p in t} | {(round(p[0], 4), round(p[1], 4)) for p in ext}
    if all(cross3(cell[q - 1], cell[q], cell[(q + 1) % len(cell)]) >= -1e-6 for q in range(len(cell))):
        parts = [cell]
    else:
        parts = [rnd([(v.x, v.y) for v in t]) for t in ear_clip(cell)]
    frs = []
    for part in parts:
        for kind, tris in ((0, body), (1, rim)):
            for t in tris:
                fr = dedupe(rnd(clip_convex(part, t)))
                if len(fr) >= 3 and abs(area(fr)) > 1e-4: frs.append([ccw(fr), kind])
    if not frs: return None
    frs = merge_convex(frs)
    tris = []
    for fr, kind in frs:
        poly = simplify(dedupe(rnd(fr)), prot)
        for t in fan(poly): tris.append((t[0], t[1], t[2], kind))
    tris = weld(tris, prot)
    tris = tfix(tris, [p for p in ext if any(in_tri(p, t) for t in body + rim)])
    E = {}
    for t in tris:
        for e in range(3): E[(t[e], t[(e + 1) % 3])] = E.get((t[e], t[(e + 1) % 3]), 0) + 1
    if any(v > 1 for v in E.values()): return None          # overlapping fragments: invalid cell
    bnd = [(a, c) for (a, c) in E if (c, a) not in E]
    A = sum(area(list(t[:3])) for t in tris)
    cx = sum(area(list(t[:3])) * (t[0][0] + t[1][0] + t[2][0]) / 3 for t in tris) / A
    cy = sum(area(list(t[:3])) * (t[0][1] + t[1][1] + t[2][1]) / 3 for t in tris) / A
    rad = max(math.hypot(p[0] - cx, p[1] - cy) for t in tris for p in t[:3])
    void_side = any(foot_disp(((a[0] + c[0]) / 2, (a[1] + c[1]) / 2), outl) != (0.0, 0.0) for a, c in bnd)
    return {"tris": tris, "bnd": bnd, "area": A, "cx": cx, "cy": cy, "rad": rad, "void": bool(void_side), "inr": 0.0}


def inradius(blk, step=2.0):
    xs = [p[0] for t in blk["tris"] for p in t[:3]]; ys = [p[1] for t in blk["tris"] for p in t[:3]]
    best = 0.0
    x = min(xs) + step / 2
    while x < max(xs):
        y = min(ys) + step / 2
        while y < max(ys):
            if any(in_tri((x, y), t) for t in blk["tris"]):
                d = min(seg_dist((x, y), a, c) for a, c in blk["bnd"])
                if d > best: best = d
            y += step
        x += step
    return best


def make_blocks(shape, variant, body, rim, outl):
    tot = sum(area(t) for t in body + rim)
    n = max(MIN_BLOCKS, round(tot / CELL_AREA * BLOCKS_PER_CELL))
    base = {"full": 1, "edge": 2, "outer": 3, "inner": 4, "diagonal": 5}[shape] * 7919 + variant * 104729
    tris_all = body + rim
    xs = [p[0] for t in tris_all for p in t]; ys = [p[1] for t in tris_all for p in t]
    best = None
    tries = 0
    while True:
        for att in range(MAX_TRY):
            tries += 1
            rng = random.Random(base * 1009 + att + n * 77)
            def rpt():
                while True:
                    p = (rng.uniform(min(xs), max(xs)), rng.uniform(min(ys), max(ys)))
                    if any(in_tri(p, t) for t in tris_all): return p
            sites = [rpt()]
            while len(sites) < n:
                bc, bd = None, -1.0
                for _ in range(14):
                    c = rpt(); d = min(math.hypot(c[0] - s[0], c[1] - s[1]) for s in sites)
                    if d > bd: bc, bd = c, d
                sites.append(bc)
            blocks = []; bad_cell = False
            SQ = [(-H, -H), (H, -H), (H, H), (-H, H)]
            for i, a in enumerate(sites):
                poly = SQ[:]
                for j, c in enumerate(sites):
                    if i == j: continue
                    poly = halfplane(poly, a, c)
                    if len(poly) < 3: break
                if len(poly) < 3: continue
                poly = dedupe(rnd([snap_pt(p) for p in poly]))
                if len(poly) < 3 or area(poly) < 1.0: continue
                poly = ccw(poly)
                blk = make_block(poly, body, rim, outl)
                if blk and blk["area"] > 5.0: blocks.append(blk)
            if bad_cell or not blocks: continue
            if abs(sum(b["area"] for b in blocks) - tot) > 0.5: continue    # cells must tile the footprint
            for blk in blocks: blk["inr"] = inradius(blk)
            score = min(b["inr"] for b in blocks)
            if best is None or score > best[0]: best = (score, blocks, n, tries)
            if score >= ACCEPT_INR: return blocks, n, tries
        if best and best[0] >= HARD_INR:
            return best[1], best[2], best[3]
        n = max(MIN_BLOCKS, n - 1)
        if n == MIN_BLOCKS and tries > 4 * MAX_TRY:
            return best[1], best[2], best[3]


# ---------------------------------------------------------------- build
def build_broken(shape, variant):
    body, rim, outl = footprint(shape, variant)
    blocks, nsites, tries = make_blocks(shape, variant, body, rim, outl)
    seed = {"full": 1, "edge": 2, "outer": 3, "inner": 4, "diagonal": 5}[shape] * 7919
    b = Builder("broken_" + shape, seed)
    b.uv2 = b.bm.loops.layers.uv.new("BlockA")
    b.uv3 = b.bm.loops.layers.uv.new("BlockB")
    b.uv4 = b.bm.loops.layers.uv.new("BlockC")
    rng = random.Random(seed * 31 + variant * 104729)
    info = []
    def put(f, blk, sd, idx):
        for l in f.loops:
            l[b.uv2].uv = (blk["cx"] / 100.0, blk["cy"] / 100.0)
            l[b.uv3].uv = (sd, blk["rad"] / 100.0)
            l[b.uv4].uv = (1.0 if blk["void"] else 0.0, idx / 16.0)
    def tri3(face_args, pts3, blk, sd, idx, **kw):
        n = Vector((0, 0, 0))
        for a, c in zip(pts3, pts3[1:] + pts3[:1]):
            n += Vector(((a[1] - c[1]) * (a[2] + c[2]), (a[2] - c[2]) * (a[0] + c[0]), (a[0] - c[0]) * (a[1] + c[1])))
        if n.length < 1e-6: return None
        f = b.face(pts3, *face_args, **kw)
        put(f, blk, sd, idx)
        return f
    for idx, blk in enumerate(blocks):
        sd = rng.random()
        for (p0, p1, p2, kind) in blk["tris"]:
            tri3(((0, 0, 0),), [(p[0], p[1], TOP) for p in (p0, p1, p2)], blk, sd, idx, up=1, kind="top")
        for (a, c) in blk["bnd"]:
            da, dc = foot_disp(a, outl), foot_disp(c, outl)
            A, C = (a[0], a[1], TOP), (c[0], c[1], TOP)
            A0, C0 = (a[0] + da[0], a[1] + da[1], 0.0), (c[0] + dc[0], c[1] + dc[1], 0.0)
            # winding from the top triangle order, not from the geometry: a skewed facet foot may fold a short wall, the prism stays closed
            for tr in ([C, A, A0], [C, A0, C0]):
                tri3(((sd, 1, 0),), tr, blk, sd, idx, kind="side")
        for (p0, p1, p2, kind) in blk["tris"]:
            pts = []
            for p in (p2, p1, p0):
                d = foot_disp(p, outl); pts.append((p[0] + d[0], p[1] + d[1], 0.0))
            tri3(((0, 0, 1),), pts, blk, sd, idx, kind="cap")
        info.append({"id": idx, "centre_cm": [round(blk["cx"], 2), round(blk["cy"], 2)], "radius_cm": round(blk["rad"], 2), "seed": round(sd, 4),
                     "top_area_uu2": round(blk["area"], 1), "min_width_uu": round(2 * blk["inr"], 1), "touches_void_outline": blk["void"],
                     "top_tris": len(blk["tris"]), "boundary_edges": len(blk["bnd"])})
    b.blocks = info
    b.nsites = nsites
    b.tries = tries
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


def make_world(layout=LAYOUT, damage=DAMAGE):
    rows = layout[::-1]
    W, Hh = len(rows[0]), len(rows)
    def solid(c, r): return 1 if 0 <= c < W and 0 <= r < Hh and rows[r][c] == "#" else 0
    def dmg(c, r):
        if solid(c, r): return damage.get((c, r), 0.0)
        ns = [damage.get((c + dc, r + dr), 0.0) for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)) if solid(c + dc, r + dr)]
        return max(ns) if ns else 0.0
    empties = [(c, r) for r in range(-2, Hh + 2) for c in range(-2, W + 2) if not solid(c, r)]
    intact = [(c, r) for r in range(Hh) for c in range(W) if solid(c, r) and damage.get((c, r), 0.0) == 0.0]
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


def block_motion(cx, cy, sd, rad, world, force_g=None):
    """rigid motion of a block as the material will compute it. Returns (hinge point, dir to the nearest intact cell, slope, height drop).
    g = damage x void factor at the block centre (force_g overrides it: full-sag stress test); hinge = side of the block facing the
    nearest intact cell, so the block stays level there and tilts down away from it; seed -> extra random tilt and a random height
    drop, both scaled by g (0 next to intact rock)."""
    solid, dmg, empties, intact = world
    g = (sag_at(cx, cy, solid, dmg, empties) / SAG_MAX) if force_g is None else force_g
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


def border_set(bm, zs=(TOP, 0.0)):
    s = set()
    for f in bm.faces:
        for l in f.loops:
            p = l.vert.co
            if (abs(abs(p.x) - H) < EPS or abs(abs(p.y) - H) < EPS) and any(abs(p.z - z) < 1e-3 for z in zs):
                s.add((round(p.x, 3), round(p.y, 3), round(p.z, 3)))
    return s


def closed_check(me):
    """per block (faces grouped by their block data): every directed edge occurs once and its reverse once (welded at 1/1000 uu),
    z spans 0 to 100"""
    u2 = me.uv_layers["BlockA"].data; u3 = me.uv_layers["BlockB"].data
    groups = {}
    for p in me.polygons:
        l0 = p.loop_indices[0]
        groups.setdefault((round(u2[l0].uv[0], 5), round(u2[l0].uv[1], 5), round(u3[l0].uv[0], 5)), []).append(p)
    res = []
    for key, ps in groups.items():
        E = {}; zmin, zmax = 1e9, -1e9
        for p in ps:
            vs = [tuple(round(c * 1000) for c in me.vertices[i].co) for i in p.vertices]
            for i in p.vertices:
                z = me.vertices[i].co.z; zmin = min(zmin, z); zmax = max(zmax, z)
            for a, c in zip(vs, vs[1:] + vs[:1]): E[(a, c)] = E.get((a, c), 0) + 1
        closed = all(v == 1 and E.get((c, a), 0) == 1 for (a, c), v in E.items())
        res.append({"closed": closed, "z_min": round(zmin, 3), "z_max": round(zmax, 3), "faces": len(ps)})
    return res


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    report = {"task": "task-230-06-rock-plate-overlay (version 4: broken pieces as a fractured volume of closed prisms)", "script": "Tools/art/blender/rock_slab_broken.py", "units": "cm (1 uu = 1 cm)",
              "constants": {"foot_flare_uu": FLARE, "min_top": MIN_TOP, "border_vertices_per_side": [-H] + list(BORDER_PTS) + [H], "blocks_per_cell_target": BLOCKS_PER_CELL,
                            "min_block_radius_accept": ACCEPT_INR, "min_block_radius_hard": HARD_INR},
              "channels": CHANNELS, "shapes": {}, "warnings": []}
    builders, intact = {}, {}
    for shape in SHAPES:
        ib = build_piece(shape, 1); finish(ib); intact[shape] = ib
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
            cnt = {"R1": 0, "G": 0, "B": 0}; bad = 0
            for p in me.polygons:
                cs = {tuple(round(c, 3) for c in ca[li].color[:3]) for li in p.loop_indices}
                if len(cs) != 1: bad += 1; continue
                c = next(iter(cs))
                zc = [me.vertices[i].co.z for i in p.vertices]
                flat = max(zc) - min(zc) < 1e-4
                if c == (0.0, 0.0, 0.0): cnt["R1"] += 1; bad += 0 if flat and p.normal.z > 0 and abs(zc[0] - TOP) < 1e-4 else 1
                elif c[1] == 1.0 and c[2] == 0.0: cnt["G"] += 1
                elif c == (0.0, 0.0, 1.0): cnt["B"] += 1   # a skewed facet foot may fold a bottom triangle: hidden, so not an error
                else: bad += 1
            uvc = tri_stats(me)
            uv0 = me.uv_layers["UVMap"].data
            uv0_ok = all(abs(uv0[l.index].uv[0] - me.vertices[l.vertex_index].co.x / 100) < 1e-5 and abs(uv0[l.index].uv[1] - me.vertices[l.vertex_index].co.y / 100) < 1e-5 for l in me.loops)
            cl = closed_check(me)
            u3 = me.uv_layers["BlockB"].data
            ib_set = border_set(intact[shape].bm, (0.0,)); bb_set = border_set(b.bm, (0.0,))   # foot only: the intact top edge is chamfered, the broken one is not
            key = "%s_%02d" % (shape, var)
            inr = [bl["min_width_uu"] for bl in b.blocks]
            report["shapes"][key] = {
                "file": "Art/Meshes/rock-slab/%s.fbx" % name, "tris": tris, "budget": list(BUDGET_TRIS), "within_budget": BUDGET_TRIS[0] <= tris <= BUDGET_TRIS[1],
                "intact_tris": len(intact[shape].bm.faces), "blocks": len(b.blocks), "block_seed_search": {"sites_requested": b.nsites, "attempts": b.tries},
                "face_split": {"top": cnt["R1"], "sides": cnt["G"], "bottoms": cnt["B"]},
                "dimensions_cm": [round(max(xs) - min(xs), 2), round(max(ys) - min(ys), 2), round(max(zs) - min(zs), 2)],
                "bounds_min": [round(min(xs), 2), round(min(ys), 2), round(min(zs), 2)], "bounds_max": [round(max(xs), 2), round(max(ys), 2), round(max(zs), 2)],
                "materials": ["M_RockSlab"], "uv_layers": [u.name for u in me.uv_layers],
                "uv_checks": {"UVMap_planar_xy_over_100": bool(uv0_ok), "LightMap": uvc, "every_face_has_block_data": all(u3[l].uv[0] >= 0 for l in range(len(u3)))},
                "vertex_colour": {"attribute": "Color (corner, float)", "export": "colors_type=LINEAR", "block_tops_000": cnt["R1"], "G1_sides": cnt["G"], "B1_bottoms": cnt["B"],
                                  "inconsistent_faces": bad, "pass": bad == 0},
                "closed_prisms": {"blocks": len(cl), "all_closed": all(c["closed"] for c in cl), "all_reach_z0": all(abs(c["z_min"]) < 1e-3 for c in cl),
                                  "all_top_at_100": all(abs(c["z_max"] - TOP) < 1e-3 for c in cl)},
                "min_block_width_uu": min(inr), "block_list": b.blocks,
                "intact_border_vertices_kept": ib_set <= bb_set,
                "all_flat_shaded": all(not p.use_smooth for p in me.polygons),
            }
            w = report["shapes"][key]
            if not w["within_budget"]: report["warnings"].append("%s tris %d outside the provisional budget %s" % (key, tris, list(BUDGET_TRIS)))
            if not uvc["pass"]: report["warnings"].append("%s LightMap check failed" % key)
            if bad: report["warnings"].append("%s vertex colour" % key)
            if not w["intact_border_vertices_kept"]: report["warnings"].append("%s intact border vertices not kept" % key)
            if not (w["closed_prisms"]["all_closed"] and w["closed_prisms"]["all_reach_z0"] and w["closed_prisms"]["all_top_at_100"]): report["warnings"].append("%s block not closed / not reaching the ground" % key)
            if min(inr) < 2 * HARD_INR - 1e-6: report["warnings"].append("%s block narrower than %.1f uu" % (key, 2 * HARD_INR))
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
            ac = {p for p in cache[(i, j)] if line(p) and abs(p[2]) < 1e-3}
            cc = {p for p in cache[q] if line(p) and abs(p[2]) < 1e-3}
            compared += len(a) + len(ac)
            if a != c or ac != cc: mism += 1
    # shapes against each other: every (shape, rotation) of the 5 shapes, one 8 x 8 layout; plus all 25 shape pairs sharing a border line are covered by the layout above
    shapes_seen = sorted({placed[k][0] for k in placed})
    report["seam_check"] = {"method": "all 5 broken shapes, variants 01/02 alternating ((i+j) parity), placed with the marching-squares rotation over the 8 x 8 layout of rock_slab_pieces.py (%d pieces, shapes present: %s); for each adjacent pair the vertices on the shared border line at z = 100 (block tops, rim) and at z = 0 (feet) are compared as sets (1 mm rounding)" % (len(placed), shapes_seen),
                            "adjacent_pairs": pairs, "border_vertices_compared": compared, "mismatching_pairs": mism, "pass": mism == 0,
                            "guarantee": "the only vertices on a piece border are -33.5, -12, 0, 12, 33.5 plus the rim ends of the intact piece (4 uu from the outline crossing); the outline crosses the border at the middle as in the intact pieces",
                            "intact_vs_broken": "z = 0 border vertices of every intact piece are contained in its broken variants (intact_border_vertices_kept per piece). The intact wall has rows at z 85.77 and 48 at the border end, off the straight broken wall by at most 1 uu; the intact top edge is chamfered and the broken one is not, so the closed block end stands 8 world uu above an intact chamfer at a joint"}
    if mism: report["warnings"].append("seam mismatch")
    report["fbx_export"] = {"apply_scale_options": "FBX_SCALE_UNITS", "global_scale": 1.0, "axes": "Blender default", "colors_type": "LINEAR", "mesh_smooth_type": "FACE"}
    report["material_rigid_tilt"] = {
        "note": "simulated in the previews exactly like this; the material (WPO) must do the same, per vertex of a block, from UV2/UV3 and world data",
        "inputs": "block centre C = rot(UV2 * 100) + instance position (world XY); seed s = UV3.x; radius R = UV3.y * 100; rim flag = UV4.x; g = D(C) * f(C) in 0..1 (D bilinear corner damage, f = (1 - min(dE/100,1))^2, dE distance to the nearest empty cell)",
        "hinge": "dir = unit vector from C toward the nearest intact solid cell centre; hinge point = C + dir * R, hinge axis = perpendicular to dir",
        "per_block": "s2 = frac(sin(s*127.1 + 311.7) * 43758.5453); k = min(1, 2g); slope = max(min(SAG_MAX*g*(0.7+0.6*s2)/(2R), 0.75) + (s - 0.5)*0.45*k, 0.02*g); hoff = s2*9*k",
        "per_vertex": "u = dot(hinge - v_xy, dir) (distance behind the hinge, >= 0); drop = min(hoff + u*slope, TOP - 60); z = z - drop (same drop for top and bottom vertices of the block: the prism keeps its shape, only z moves, so a block never opens a hole and its neighbour side shows in the gap; the feet sink below the floor)",
        "SAG_MAX": SAG_MAX,
        "layout_rows_top_to_bottom": LAYOUT, "damage": {"%d,%d" % k: v for k, v in DAMAGE.items()}}
    report["sag_check"] = sag_check(builders)
    if report["sag_check"]["holes"] or report["sag_check"]["min_top_z"] < MIN_TOP - 1e-3: report["warnings"].append("sag check failed")
    if PREVIEW:
        report["preview"] = previews(builders, intact)
    with open(os.path.join(OUTDIR, "SM_RockSlabBroken.report.json"), "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print(json.dumps({"tris": {k: v["tris"] for k, v in report["shapes"].items()}, "blocks": {k: v["blocks"] for k, v in report["shapes"].items()},
                      "min_width": {k: v["min_block_width_uu"] for k, v in report["shapes"].items()},
                      "closed": {k: v["closed_prisms"]["all_closed"] and v["closed_prisms"]["all_reach_z0"] for k, v in report["shapes"].items()},
                      "seam": report["seam_check"]["pass"], "sag": report["sag_check"], "warnings": report["warnings"]}))


def sag_check(builders):
    """full-sag stress test: every piece of the 8 x 8 seam layout broken (variants alternating), every block forced to g = 1
    (force_g) so each one takes its maximum drop. Vertical rays on a 3 uu grid (offset so they never run along an edge): wherever the
    rest layout has rock, the sagged layout must also be hit, at z >= 60."""
    TL = ["###..###", "###..###", "##.#....", "##..##..", ".....##.", "###.####", "###..#..", "###...#."]
    dmg = {(c, r): 1.0 for r in range(8) for c in range(8) if TL[::-1][r][c] == "#"}
    world = make_world(TL, dmg)
    solid = world[0]
    placed = {}
    for j in range(9):
        for i in range(9):
            m = solid(i - 1, j - 1) | solid(i, j - 1) << 1 | solid(i, j) << 2 | solid(i - 1, j) << 3
            cs = case_of(m)
            if cs: placed[(i, j)] = cs
    items = [(builders[(shape, 1 + (i + j) % 2)], k, i * 67.0, j * 67.0) for (i, j), (shape, k) in placed.items()]
    mat = bpy.data.materials.new("SC")
    rest = layout_object("SagRest", items, None, mat)
    sag = layout_object("SagFull", items, world, mat, force_g=1.0)
    dg = bpy.context.evaluated_depsgraph_get()
    bv_r = BVHTree.FromObject(rest, dg); bv_s = BVHTree.FromObject(sag, dg)
    zmin_top = min(min(v.co.z for v in [sag.data.vertices[i] for i in p.vertices]) for p in sag.data.polygons if p.normal.z > 0.5 and sag.data.color_attributes["Color"].data[p.loop_indices[0]].color[2] < 0.5)
    drops = []
    n = holes = low = 0
    x = -20.0 + 0.37
    while x < 8 * 67.0 + 40:
        y = -20.0 + 0.61
        while y < 8 * 67.0 + 40:
            h0 = bv_r.ray_cast(Vector((x, y, 400.0)), Vector((0, 0, -1)))
            if h0[0] is not None and h0[0].z > TOP - 1e-3:
                n += 1
                h1 = bv_s.ray_cast(Vector((x, y, 400.0)), Vector((0, 0, -1)))
                if h1[0] is None: holes += 1
                elif h1[0].z < MIN_TOP - 1e-3: low += 1
                else: drops.append(h0[0].z - h1[0].z)
            y += 3.0
        x += 3.0
    bpy.data.objects.remove(rest); bpy.data.objects.remove(sag)
    return {"method": "all %d pieces of the 8 x 8 seam layout broken, every block forced to g = 1 (full sag), vertical rays on a 3 uu grid" % len(placed),
            "rays_over_rock": n, "holes": holes, "hits_below_60": low, "min_top_z": round(zmin_top, 3),
            "max_drop_uu": round(max(drops), 2) if drops else 0.0, "mean_drop_uu": round(sum(drops) / len(drops), 2) if drops else 0.0,
            "pass": holes == 0 and low == 0 and zmin_top >= MIN_TOP - 1e-3}


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
    """R = 1 block top (top tone), R = 0.5 rim (light rim tone), G sides, B bottoms. Back faces culled (faces of neighbouring blocks touch back to back)."""
    m = bpy.data.materials.new("PV_broken"); m.use_nodes = True
    m.use_backface_culling = True
    nt = m.node_tree; bsdf = nt.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 0.9
    at = nt.nodes.new("ShaderNodeAttribute"); at.attribute_name = "Color"; at.attribute_type = "GEOMETRY"
    sep = nt.nodes.new("ShaderNodeSeparateColor"); nt.links.new(at.outputs["Color"], sep.inputs["Color"])
    def gt(v):
        n = nt.nodes.new("ShaderNodeMath"); n.operation = "GREATER_THAN"; n.inputs[1].default_value = v
        nt.links.new(sep.outputs["Red"], n.inputs[0]); return n
    rimf, topf = gt(0.25), gt(0.75)
    m1 = nt.nodes.new("ShaderNodeMix"); m1.data_type = "RGBA"; m1.inputs[6].default_value = srgb("#8E89A8"); m1.inputs[7].default_value = srgb("#8E89A8")
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


def layout_object(name, items, world=None, mat=None, force_g=None):
    """items: (builder, k, cx, cy). world=None: no sag. Every face of a broken piece belongs to a block (UV3.x >= 0) and moves with it
    (block_motion): the same z drop on all its vertices, so the prism stays closed."""
    bm = bmesh.new(); col = bm.loops.layers.float_color.new("Color")
    cache = {}
    for src_b, k, cx, cy in items:
        scol = src_b.bm.loops.layers.float_color["Color"]
        has_block = hasattr(src_b, "uv3")
        for f in src_b.bm.faces:
            blkm = None
            if world and has_block:
                l0 = f.loops[0]
                a, c = l0[src_b.uv2].uv, l0[src_b.uv3].uv
                pc = rot((a[0] * 100.0, a[1] * 100.0), k)
                wx, wy = pc[0] + cx, pc[1] + cy
                key = (round(wx, 3), round(wy, 3), round(c[0], 5))
                if key not in cache:
                    cache[key] = block_motion(wx, wy, c[0], c[1] * 100.0, world, force_g)
                blkm = cache[key]
            vs = []
            for l in f.loops:
                p = rot(tuple(l.vert.co), k)
                x, y, z = p[0] + cx, p[1] + cy, p[2]
                if blkm is not None:
                    ph, d, slope, hoff = blkm
                    u = max((ph[0] - x) * d[0] + (ph[1] - y) * d[1], 0.0)
                    z -= min(hoff + u * slope, TOP - MIN_TOP)
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
                     ("rockbroken_pair_closeup.png", {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": (440.0, 130.0, 60.0), "dist": 420.0}),
                     ("rockbroken_side_columns.png", {"hfov": 50.0, "pitch": 16.0, "azim": 14.0, "target": (168.0, 262.0, 55.0), "dist": 330.0})):
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
    out["camera"] = "layout hfov 90 pitch 60 azim 0 dist 640 (game) and hfov 50 pitch 38 azim 28 dist 1050 (three-quarter), target (268,268,0); side columns hfov 50 pitch 16 azim 14 dist 330 target (168,262,55); same camera-following sun and world as the slab previews"
    return out


main()
