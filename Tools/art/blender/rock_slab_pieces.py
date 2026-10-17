"""Rock slab dual-grid pieces (task-230-02). Blender route, deterministic, no Unreal import.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/rock_slab_pieces.py -- [--out-dir Art/Meshes/rock-slab] [--no-preview]
Writes SM_RockSlab_<Shape>_01.fbx (full, edge, outer, inner, diagonal), SM_RockSlab.report.json and previews.
Units: 1 Blender unit = 1 cm (= 1 uu). Piece = 67 x 67 centred on a cell corner; its 4 corners are the 4 cell centres.
Canonical orientations (Blender axes, corners A(-,-) B(+,-) C(+,+) D(-,+)); the engine rotates by k x 90 deg about Z (x,y)->(-y,x):
  full: A B C D rock; edge: B C rock (rock on +X); outer: C rock; inner: B C D rock (notch at A); diagonal: A C rock (joined saddle).
Seams: every outline crosses a piece border at the middle of the border, perpendicular to it, and the 3 side rows and the rim
inner point at that crossing are fixed offsets along the border (no jitter), so any two pieces share exact vertices."""
import bpy, bmesh, math, os, sys, json, random
from mathutils import Vector, Euler

H = 33.5            # half piece = half cell (67 uu)
TOP = 100.0         # rock top above the floor top
RIM_W = 4.0         # rim band width on the top
ROW_OFF = (0.0, 4.0, 9.0)      # side rows: outward offset (flare) at z = TOP, mid, 0
ROW_Z_MID = 48.0
MASKS = {"full": 15, "edge": 6, "outer": 4, "inner": 14, "diagonal": 5}
CANON = {"A": (-H, -H), "B": (H, -H), "C": (H, H), "D": (-H, H)}
BUDGET = {"full": 2, "edge": 48, "outer": 48, "inner": 64, "diagonal": 96}
SEG = {"edge": 6, "outer": 6, "inner": 8, "diagonal": 6}

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
OUTDIR = os.path.join(ROOT, "Art", "Meshes", "rock-slab")
if "--out-dir" in argv:
    OUTDIR = os.path.abspath(argv[argv.index("--out-dir") + 1])
PREVIEW = "--no-preview" not in argv
PREV_DIR = os.path.join(OUTDIR, "preview")


# ---------------------------------------------------------------- outlines
def outline_pts(kind, n):
    if kind == "edge":
        pts = []
        for i in range(n + 1):
            u = i / n
            x = 5.0 * math.sin(math.pi * u) ** 2 * math.cos(1.5 * math.pi * u + 0.8)
            pts.append(Vector((x, -H + 2 * H * u)))
        return [(pts, Vector((H, 0)))]
    if kind == "outer":   # rock at C, outline (0,H) -> (H,0)
        c = CANON["C"]
        pts = []
        for i in range(n + 1):
            t = (math.pi / 2) * i / n
            r = H * (1.0 + 0.17 * math.sin(2 * t) ** 2 * math.cos(3.1 * t + 0.4))
            pts.append(Vector((c[0] - r * math.cos(t), c[1] - r * math.sin(t))))
        return [(pts, Vector(c))]
    if kind == "inner":   # notch at A, outline (0,-H) -> (-H,0), rock outside
        a = CANON["A"]
        pts = []
        for i in range(n + 1):
            t = (math.pi / 2) * i / n
            r = H * (1.0 + 0.16 * math.sin(2 * t) ** 2 * math.cos(2.6 * t + 1.9))
            pts.append(Vector((a[0] + r * math.cos(t), a[1] + r * math.sin(t))))
        return [(pts, Vector(CANON["C"]))]
    if kind == "diagonal":  # notches at B (0,-H)->(H,0) and D (0,H)->(-H,0); rock A and C joined
        out = []
        for corner, ref, amp, ph, fr in (("B", "D", 0.15, 0.9, 2.8), ("D", "B", -0.14, 2.2, 3.4)):
            c = CANON[corner]
            pts = []
            for i in range(n + 1):
                t = (math.pi / 2) * i / n
                r = H * (1.0 + amp * math.sin(2 * t) ** 2 * math.cos(fr * t + ph))
                # B notch: angle measured from -Y border toward +X axis; D notch: mirror through centre
                if corner == "B":
                    p = Vector((c[0] - r * math.cos(t), c[1] + r * math.sin(t)))   # t=0 -> (0,-H), t=90 -> (H,0)
                else:
                    p = Vector((c[0] + r * math.cos(t), c[1] - r * math.sin(t)))   # t=0 -> (0,H), t=90 -> (-H,0)
                pts.append(p)
            out.append((pts, Vector(CANON[ref])))
        return out
    raise ValueError(kind)


def inward_normals(pts, ref):
    ns = []
    for i, p in enumerate(pts):
        if i in (0, len(pts) - 1):   # exact: along the border toward the rock
            if abs(abs(p.y) - H) < 1e-6:
                ns.append(Vector((math.copysign(1.0, ref.x - p.x), 0.0)))
            else:
                ns.append(Vector((0.0, math.copysign(1.0, ref.y - p.y))))
            continue
        t = (pts[i + 1] - pts[i - 1]).normalized()
        n = Vector((-t.y, t.x))
        if n.dot(ref - p) < 0:
            n = -n
        ns.append(n)
    return ns


def ear_clip(poly):
    """2D ear clipping of a simple polygon (list of Vector), returns triangles counter-clockwise (own code: bmesh ear clip misbehaved)."""
    pts = [Vector(p) for p in poly]
    area = sum(a.x * c.y - c.x * a.y for a, c in zip(pts, pts[1:] + pts[:1]))
    idx = list(range(len(pts)))
    if area < 0:
        idx.reverse()
    def cross(o, a, c):
        return (a.x - o.x) * (c.y - o.y) - (a.y - o.y) * (c.x - o.x)
    def inside(p, a, c, d):
        return cross(a, c, p) > 1e-9 and cross(c, d, p) > 1e-9 and cross(d, a, p) > 1e-9
    tris = []
    while len(idx) > 3:
        n = len(idx); best = None
        for k in range(n):
            i0, i1, i2 = idx[k - 1], idx[k], idx[(k + 1) % n]
            if cross(pts[i0], pts[i1], pts[i2]) <= 1e-9:
                continue
            if any(inside(pts[j], pts[i0], pts[i1], pts[i2]) for j in idx if j not in (i0, i1, i2)):
                continue
            best = k; break
        if best is None:   # only collinear leftovers: drop the flattest vertex
            best = min(range(n), key=lambda k: abs(cross(pts[idx[k - 1]], pts[idx[k]], pts[idx[(k + 1) % n]])))
            idx.pop(best); continue
        tris.append([pts[idx[best - 1]], pts[idx[best]], pts[idx[(best + 1) % n]]])
        idx.pop(best)
    tris.append([pts[i] for i in idx])
    return tris


# ---------------------------------------------------------------- builder
class Builder:
    def __init__(self, name, seed):
        self.name = name
        self.bm = bmesh.new()
        self.rng = random.Random(seed)
        self.uv0 = self.bm.loops.layers.uv.new("UVMap")
        self.uv1 = self.bm.loops.layers.uv.new("LightMap")
        self.col = self.bm.loops.layers.float_color.new("Color")
        self.seam_pts = []   # outline-related vertices (rim + side rows), for the seam check
        self.kind_faces = {}

    def face(self, pos, rgb, up=None, out=None, kind="top"):
        """pos: list of Vector (x,y,z). Winding forced so the normal points along `up` (z) or `out` (xy direction)."""
        p = [Vector(v) for v in pos]
        nrm = Vector((0, 0, 0))   # Newell normal (robust for concave polygons)
        for a, c in zip(p, p[1:] + p[:1]):
            nrm += Vector(((a.y - c.y) * (a.z + c.z), (a.z - c.z) * (a.x + c.x), (a.x - c.x) * (a.y + c.y)))
        if up is not None and nrm.z * up < 0:
            p.reverse()
        if out is not None and nrm.dot(out) < 0:
            p.reverse()
        vs = [self.bm.verts.new(v) for v in p]
        f = self.bm.faces.new(vs)
        for l in f.loops:
            l[self.col] = (rgb[0], rgb[1], rgb[2], 1.0)
            l[self.uv0].uv = (l.vert.co.x / 100.0, l.vert.co.y / 100.0)
        f.smooth = False
        self.kind_faces.setdefault(kind, []).append(f)
        return f


def build_piece(shape):
    seed = {"full": 1, "edge": 2, "outer": 3, "inner": 4, "diagonal": 5}[shape] * 7919
    b = Builder(shape, seed)
    TOPC, RIMC, SIDEC = (0, 0, 0), (1, 0, 0), (0, 1, 0)
    if shape == "full":
        b.face([(-H, -H, TOP), (H, -H, TOP), (H, H, TOP), (-H, H, TOP)], TOPC, up=1, kind="top")
        return b
    n = SEG[shape]
    outlines = outline_pts(shape, n)
    body_chains = []
    for pts, ref in outlines:
        ins = inward_normals(pts, ref)
        P = [Vector((p.x, p.y, TOP)) for p in pts]
        Q = [Vector((p.x + RIM_W * nn.x, p.y + RIM_W * nn.y, TOP)) for p, nn in zip(pts, ins)]
        # side rows: outward = -inward
        rows = []
        for r, off in enumerate(ROW_OFF):
            row = []
            for i, (p, nn) in enumerate(zip(pts, ins)):
                end = i in (0, n)
                o, z = off, (TOP if r == 0 else ROW_Z_MID if r == 1 else 0.0)
                tx = 0.0
                if r == 1 and not end:
                    o += b.rng.uniform(-3.0, 3.0); z += b.rng.uniform(-10.0, 10.0)
                    tx = b.rng.uniform(-3.0, 3.0)
                elif r == 2 and not end:
                    o += b.rng.uniform(-1.5, 1.5); tx = b.rng.uniform(-2.5, 2.5)
                tang = Vector((-nn.y, nn.x))
                row.append(Vector((p.x - nn.x * o + tang.x * tx, p.y - nn.y * o + tang.y * tx, z)))
            rows.append(row)
        for i in range(n):
            b.face([P[i], P[i + 1], Q[i + 1], Q[i]], RIMC, up=1, kind="rim")
            for r in range(2):
                a0, a1, b0, b1 = rows[r][i], rows[r][i + 1], rows[r + 1][i], rows[r + 1][i + 1]
                mid = (a0 + a1 + b0 + b1) / 4
                outd = Vector((-ins[i].x - ins[i + 1].x, -ins[i].y - ins[i + 1].y, 0.0))
                # quad split in two triangles (flat facets), alternate the diagonal for a faceted look
                tris = ([a0, a1, b1], [a0, b1, b0]) if (i + r) % 2 == 0 else ([a0, a1, b0], [a1, b1, b0])
                for tr in tris:
                    b.face(tr, SIDEC, out=outd, kind="side")
        for r in rows:
            b.seam_pts += [tuple(v) for v in r]
        b.seam_pts += [tuple(v) for v in P] + [tuple(v) for v in Q]
        body_chains.append(Q)
    # top body polygon (rim inner chain + piece corners), counter-clockwise
    if shape == "edge":
        poly = body_chains[0] + [Vector((H, H, TOP)), Vector((H, -H, TOP))]
    elif shape == "outer":
        poly = body_chains[0] + [Vector((H, H, TOP))]
    elif shape == "inner":
        poly = body_chains[0] + [Vector((-H, H, TOP)), Vector((H, H, TOP)), Vector((H, -H, TOP))]
    else:
        poly = body_chains[0] + [Vector((H, H, TOP))] + body_chains[1] + [Vector((-H, -H, TOP))]
    for tri in ear_clip(poly):
        b.face(tri, TOPC, up=1, kind="top")
    return b


def finish(b):
    """triangulate side/rim quads (already triangles/quads), pack LightMap, return stats."""
    bm = b.bm
    bmesh.ops.triangulate(bm, faces=list(bm.faces), quad_method="BEAUTY", ngon_method="EAR_CLIP")
    for f in bm.faces:
        f.smooth = False
        if f.normal.z < -1e-9 and all(abs(v.co.z - TOP) < 1e-6 for v in f.verts):   # ear clip may flip flat top triangles
            f.normal_flip()
    bm.normal_update()
    tris = list(bm.faces)
    N = max(1, math.ceil(math.sqrt(len(tris))))
    margin = 2.0 / 256.0 + 0.002
    cell = 1.0 / N
    loc = []
    for f in tris:
        p = [l.vert.co.copy() for l in f.loops]
        e1 = p[1] - p[0]
        u = e1.normalized()
        nrm = e1.cross(p[2] - p[0])
        v = nrm.cross(u).normalized() if nrm.length > 1e-9 else Vector((0, 0, 1)).cross(u)
        c = [(0.0, 0.0), (e1.length, 0.0), ((p[2] - p[0]).dot(u), (p[2] - p[0]).dot(v))]
        mnx, mny = min(a[0] for a in c), min(a[1] for a in c)
        loc.append([(a[0] - mnx, a[1] - mny) for a in c])
    emax = max(max(max(a[0] for a in c), max(a[1] for a in c)) for c in loc)
    s = (cell - 2 * margin) / emax
    for k, (f, c) in enumerate(zip(tris, loc)):
        ox, oy = (k % N) * cell + margin, (k // N) * cell + margin
        for l, a in zip(f.loops, c):
            l[b.uv1].uv = (ox + a[0] * s, oy + a[1] * s)
    return N, s


def to_object(b, name):
    me = bpy.data.meshes.new(name)
    b.bm.to_mesh(me)
    me.update()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    mat = bpy.data.materials.new("M_RockSlab")
    me.materials.append(mat)
    return ob


# ---------------------------------------------------------------- main
def rot(p, k):
    x, y = p[0], p[1]
    for _ in range(k % 4):
        x, y = -y, x
    return (x, y) + tuple(p[2:])


def case_of(mask):
    if mask == 0:
        return None
    for shape, m in MASKS.items():
        for k in range(4):
            rm = 0
            for i in range(4):
                if m >> i & 1:
                    rm |= 1 << ((i + k) % 4)
            if rm == mask:
                return shape, k
    raise ValueError(mask)


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    report = {"task": "task-230-02-rock-slab-pieces", "script": "Tools/art/blender/rock_slab_pieces.py", "units": "cm (1 uu = 1 cm)",
              "piece": {"size_uu": [67, 67], "top_z_uu": TOP, "pivot": "piece centre (a cell corner), z = 0 floor top, nothing below", "half": H},
              "canonical_orientation": {"corners_blender_xy": {k: list(v) for k, v in CANON.items()},
                                        "rock_corners": {"full": "A B C D", "edge": "B C (rock on +X)", "outer": "C", "inner": "B C D (notch at A)", "diagonal": "A C (joined saddle)"},
                                        "rotation": "k x 90 deg about Z, (x,y)->(-y,x): corner A->B->C->D->A; Unreal import flips Y, verify the first placement"},
              "shapes": {}, "warnings": []}
    builders = {}
    for shape in MASKS:
        b = build_piece(shape)
        N, s = finish(b)
        builders[shape] = b
        name = "SM_RockSlab_%s_01" % shape.capitalize()
        ob = to_object(b, name)
        bpy.ops.object.select_all(action="DESELECT"); ob.select_set(True)
        bpy.context.view_layer.objects.active = ob
        path = os.path.join(OUTDIR, name + ".fbx")
        bpy.ops.export_scene.fbx(filepath=path, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                                 mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
        me = ob.data
        tris = len(me.polygons)
        xs = [v.co.x for v in me.vertices]; ys = [v.co.y for v in me.vertices]; zs = [v.co.z for v in me.vertices]
        dims = [round(max(xs) - min(xs), 2), round(max(ys) - min(ys), 2), round(max(zs) - min(zs), 2)]
        # vertex colour check
        ca = me.color_attributes["Color"].data
        bad = 0; rim_tris = side_tris = top_tris = 0
        for p in me.polygons:
            cs = [tuple(round(c, 3) for c in ca[li].color[:3]) for li in p.loop_indices]
            if len(set(cs)) != 1:
                bad += 1
            r, g, _ = cs[0]
            if r == 1 and g == 0: rim_tris += 1
            elif g == 1 and r == 0: side_tris += 1
            elif r == 0 and g == 0: top_tris += 1
            else: bad += 1
        # rim tris must be on top plane, side tris must not be flat top
        for p in me.polygons:
            zc = [me.vertices[i].co.z for i in p.vertices]
            ca0 = ca[p.loop_indices[0]].color
            if ca0[0] > 0.5 and max(zc) - min(zc) > 1e-4: bad += 1
            if (ca0[0] < 0.5 and ca0[1] < 0.5) and max(zc) - min(zc) > 1e-4: bad += 1
            if p.normal.z < -1e-6 and ca0[1] < 0.5: bad += 1   # top and rim must face up
        # UV check
        uv1 = me.uv_layers["LightMap"].data
        boxes = []
        for p in me.polygons:
            uvs = [uv1[li].uv for li in p.loop_indices]
            boxes.append((min(u[0] for u in uvs), min(u[1] for u in uvs), max(u[0] for u in uvs), max(u[1] for u in uvs)))
        overlaps = 0; mingap = 9.0
        for i in range(len(boxes)):
            for j in range(i + 1, len(boxes)):
                a, c = boxes[i], boxes[j]
                gx = max(c[0] - a[2], a[0] - c[2]); gy = max(c[1] - a[3], a[1] - c[3])
                gap = max(gx, gy)
                if gap < 0: overlaps += 1
                mingap = min(mingap, gap)
        bmin = min(min(b_[0], b_[1]) for b_ in boxes); bmax = max(max(b_[2], b_[3]) for b_ in boxes)
        uv_ok = overlaps == 0 and bmin >= 0 and bmax <= 1 and mingap * 256 >= 2 - 1e-6 and bmin * 256 >= 2 - 1e-6 and (1 - bmax) * 256 >= 2 - 1e-6
        uv0 = me.uv_layers["UVMap"].data
        normals_down = sum(1 for p in me.polygons if p.normal.z < -1e-6)   # side facets leaning inward (overhang)
        sides = [p for p in me.polygons if ca[p.loop_indices[0]].color[1] > 0.5]
        report["shapes"][shape] = {
            "file": "Art/Meshes/rock-slab/%s.fbx" % name, "tris": tris, "budget": BUDGET[shape], "within_budget": tris <= BUDGET[shape],
            "dimensions_cm": dims, "bounds_min": [round(min(xs), 2), round(min(ys), 2), round(min(zs), 2)], "bounds_max": [round(max(xs), 2), round(max(ys), 2), round(max(zs), 2)],
            "materials": ["M_RockSlab"], "uv_layers": [u.name for u in me.uv_layers],
            "uv_checks": {"UVMap": "planar top-down projection, x and y / 100 (world position drives the top in the material)",
                          "LightMap": {"triangles": tris, "overlapping_pairs": overlaps, "bounds": [round(bmin, 4), round(bmax, 4)], "min_gap_px_at_256": round(mingap * 256, 3), "min_border_px_at_256": round(min(bmin, 1 - bmax) * 256, 3), "pass": bool(uv_ok)}},
            "vertex_colour": {"attribute": "Color (corner, float)", "export": "colors_type=LINEAR", "rim_tris_R1": rim_tris, "side_tris_G1": side_tris, "top_tris_0": top_tris, "inconsistent_faces": bad, "pass": bad == 0,
                              "rule": "R=1 only on the flat rim band along the outline; G=1 only on side faces; top body 0,0"},
            "all_flat_shaded": all(not p.use_smooth for p in me.polygons),
            "top_flat_z": sorted(set(round(me.vertices[i].co.z, 4) for p in me.polygons if ca[p.loop_indices[0]].color[1] < 0.5 for i in p.vertices)),
            "downward_facing_tris": normals_down,
        }
        if tris > BUDGET[shape]:
            report["warnings"].append("%s over budget" % shape)
        if not uv_ok:
            report["warnings"].append("%s LightMap check failed" % shape)
        bpy.data.objects.remove(ob)

    # ---- test layout and seam check
    LAYOUT = ["###..###",   # row 7 (top of the picture)
              "###..###",
              "##.#....",
              "##..##..",
              ".....##.",
              "###.####",
              "###..#..",
              "###...#."]
    rows = LAYOUT[::-1]   # index 0 = y row 0
    W = len(rows[0]); Hh = len(rows)
    def cell(c, r):
        return 1 if 0 <= c < W and 0 <= r < Hh and rows[r][c] == "#" else 0
    placed = {}
    used = {}
    for j in range(Hh + 1):
        for i in range(W + 1):
            m = cell(i - 1, j - 1) | cell(i, j - 1) << 1 | cell(i, j) << 2 | cell(i - 1, j) << 3
            cs = case_of(m)
            if cs:
                placed[(i, j)] = cs
                used[cs[0]] = used.get(cs[0], 0) + 1
    def world_pts(i, j):
        shape, k = placed[(i, j)]
        return [(rot(p, k)[0] + i * 67.0, rot(p, k)[1] + j * 67.0, p[2]) for p in builders[shape].seam_pts]
    cache = {key: world_pts(*key) for key in placed}
    pairs = 0; mism = 0; maxdev = 0.0; crossings = 0
    for (i, j) in placed:
        for (di, dj) in ((1, 0), (0, 1)):
            q = (i + di, j + dj)
            if q not in placed:
                # neighbour empty: the shared border must hold no outline vertex only if corners equal; skip
                continue
            pairs += 1
            if di == 1:
                line = lambda p: abs(p[0] - (i * 67.0 + H * 0 + 33.5 + 0)) < 1e-4 if False else abs(p[0] - (i + 0.5) * 67.0) < 1e-4
                key = lambda p: (round(p[1], 3), round(p[2], 3))
            else:
                line = lambda p: abs(p[1] - (j + 0.5) * 67.0) < 1e-4
                key = lambda p: (round(p[0], 3), round(p[2], 3))
            # vertices of A and B lying on the shared border line
            a = set(key(p) for p in cache[(i, j)] if line(p))
            bb = set(key(p) for p in cache[q] if line(p))
            crossings += len(a)
            if a != bb:
                mism += 1
    report["seam_check"] = {
        "method": "test layout 8 x 8 cells (%s), pieces placed with rotation k x 90 deg from the marching-squares case; for every pair of adjacent placed pieces the rim and side-row vertices lying on the shared border are compared as sets (3 mm rounding, exact in practice)" % " / ".join(LAYOUT),
        "pieces_placed": len(placed), "shape_counts": used, "adjacent_pairs_checked": pairs, "border_vertices_compared": crossings, "mismatching_pairs": mism,
        "all_5_shapes_used": sorted(used) == sorted(MASKS), "pass": bool(mism == 0 and sorted(used) == sorted(MASKS)),
        "guarantee": "the outline crosses a border only at its middle (0,+-33.5 / +-33.5,0), perpendicular to it; the rim inner point and the 3 side rows (z 100, 48, 0; flare 0, 4, 9 uu) at that point are fixed offsets along the border with no jitter, independent of shape and rotation",
    }
    if mism: report["warnings"].append("seam mismatch")

    # ---- round-trip check of one exported FBX
    # (re-import in a clean scene: tris, vertex colour, UV layers survive)
    rt = {}
    for shape in MASKS:
        name = "SM_RockSlab_%s_01" % shape.capitalize()
        before = set(bpy.data.objects)
        bpy.ops.import_scene.fbx(filepath=os.path.join(OUTDIR, name + ".fbx"))
        new = [o for o in bpy.data.objects if o not in before and o.type == "MESH"][0]
        me = new.data
        mx = [new.matrix_world @ v.co for v in me.vertices]
        rt[shape] = {"tris": len(me.polygons), "colour_attributes": [c.name for c in me.color_attributes], "uv_layers": [u.name for u in me.uv_layers],
                     "height_after_import": round(max(v.z for v in mx) - min(v.z for v in mx), 3), "object_scale": [round(x, 4) for x in new.scale]}
        for o in [o for o in bpy.data.objects if o not in before]:
            bpy.data.objects.remove(o)
    report["fbx_roundtrip_blender_import"] = rt
    report["fbx_export"] = {"apply_scale_options": "FBX_SCALE_UNITS", "global_scale": 1.0, "axes": "Blender default", "colors_type": "LINEAR", "mesh_smooth_type": "FACE"}
    report["variant"] = "_01 only; the system may add _02 ... later"
    report["layout"] = {"rows_top_to_bottom": LAYOUT, "placed": {"%d,%d" % k: [v[0], v[1] * 90] for k, v in placed.items()}}
    if PREVIEW:
        report["preview"] = previews(builders, placed)
    with open(os.path.join(OUTDIR, "SM_RockSlab.report.json"), "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print(json.dumps({k: (v if k != "layout" else "...") for k, v in report.items() if k in ("shapes", "seam_check", "warnings")}, indent=1)[:6000])


# ---------------------------------------------------------------- previews
def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


def make_material():
    m = bpy.data.materials.new("PV")
    m.use_nodes = True
    nt = m.node_tree
    bsdf = nt.nodes["Principled BSDF"]
    bsdf.inputs["Roughness"].default_value = 0.9
    at = nt.nodes.new("ShaderNodeAttribute"); at.attribute_name = "Color"; at.attribute_type = "GEOMETRY"
    sep = nt.nodes.new("ShaderNodeSeparateColor")
    nt.links.new(at.outputs["Color"], sep.inputs["Color"])
    mix1 = nt.nodes.new("ShaderNodeMix"); mix1.data_type = "RGBA"
    mix1.inputs[6].default_value = srgb("#8E89A8"); mix1.inputs[7].default_value = srgb("#C4BFDB")
    nt.links.new(sep.outputs["Red"], mix1.inputs[0])
    mix2 = nt.nodes.new("ShaderNodeMix"); mix2.data_type = "RGBA"
    mix2.inputs[7].default_value = srgb("#55516F")
    nt.links.new(mix1.outputs[2], mix2.inputs[6]); nt.links.new(sep.outputs["Green"], mix2.inputs[0])
    nt.links.new(mix2.outputs[2], bsdf.inputs["Base Color"])
    return m


def combined_object(name, items, builders):
    """items: list of (shape, k, cx, cy) -> one mesh object (positions transformed, colours kept)."""
    bm = bmesh.new()
    col = bm.loops.layers.float_color.new("Color")
    for shape, k, cx, cy in items:
        src = builders[shape].bm
        scol = src.loops.layers.float_color["Color"]
        for f in src.faces:
            vs = []
            for l in f.loops:
                p = rot(tuple(l.vert.co), k)
                vs.append(bm.verts.new((p[0] + cx, p[1] + cy, p[2])))
            nf = bm.faces.new(vs)
            nf.smooth = False
            for nl, l in zip(nf.loops, f.loops):
                nl[col] = l[scol]
    me = bpy.data.meshes.new(name); bm.to_mesh(me); bm.free()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    return ob


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
    """pitch = degrees below the horizon the camera looks; azim 0 = looking toward +Y."""
    p, a = math.radians(pitch_deg), math.radians(azim_deg)
    d = Vector((math.sin(a) * math.cos(p), math.cos(a) * math.cos(p), -math.sin(p)))   # view direction
    cam.location = Vector(target) - d * dist
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()


def render_one(sc, objs, path, cam_spec):
    for o in objs: o.hide_render = False
    cam_d = bpy.data.cameras.new("Cam"); cam_d.clip_start = 1; cam_d.clip_end = 6000
    cam_d.sensor_fit = "HORIZONTAL"
    cam_d.lens_unit = "FOV"; cam_d.angle = math.radians(cam_spec["hfov"])
    cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam)
    aim(cam, cam_spec["target"], cam_spec["pitch"], cam_spec["azim"], cam_spec["dist"])
    sc.camera = cam
    sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 1.0
    sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
    sun.rotation_euler = cam.rotation_euler.copy()     # camera-following sun: same rotation as the camera
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    bpy.data.objects.remove(cam); bpy.data.objects.remove(sun)


def previews(builders, placed):
    os.makedirs(PREV_DIR, exist_ok=True)
    mat = make_material()
    floor_m = bpy.data.materials.new("FLOOR"); floor_m.use_nodes = True
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = srgb("#B9B9BE")
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 1.0
    out = {}
    # layout
    items = [(s, k, i * 67.0, j * 67.0) for (i, j), (s, k) in placed.items()]
    lay = combined_object("Layout", items, builders); lay.data.materials.append(mat)
    fl = bpy.data.meshes.new("Floor"); fl.from_pydata([(-3000, -3000, 0), (4000, -3000, 0), (4000, 4000, 0), (-3000, 4000, 0)], [], [(0, 1, 2, 3)])
    flo = bpy.data.objects.new("Floor", fl); bpy.context.scene.collection.objects.link(flo); fl.materials.append(floor_m)
    sc = setup_render(1920, 1080)
    cen = (268.0, 268.0, 0.0)
    game = {"hfov": 90.0, "pitch": 60.0, "azim": 0.0, "target": cen, "dist": 640.0}
    p = os.path.join(PREV_DIR, "rockslab_layout_game_cam.png"); render_one(sc, [lay, flo], p, game); out["layout_game_cam"] = p
    tq = {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": cen, "dist": 1050.0}
    p = os.path.join(PREV_DIR, "rockslab_layout_three_quarter.png"); render_one(sc, [lay, flo], p, tq); out["layout_three_quarter"] = p
    bpy.data.objects.remove(lay)
    # per piece: three-quarter + game-cam steep, each piece in canonical orientation, with a tiny frame of neighbours not needed
    sc.render.resolution_x, sc.render.resolution_y = 1000, 1000
    for shape in MASKS:
        o = combined_object("P_" + shape, [(shape, 0, 0.0, 0.0)], builders); o.data.materials.append(mat)
        p = os.path.join(PREV_DIR, "rockslab_%s_three_quarter.png" % shape)
        render_one(sc, [o, flo], p, {"hfov": 40.0, "pitch": 35.0, "azim": 32.0, "target": (0, 0, 40.0), "dist": 330.0}); out[shape + "_three_quarter"] = p
        p = os.path.join(PREV_DIR, "rockslab_%s_game_cam.png" % shape)
        render_one(sc, [o, flo], p, {"hfov": 40.0, "pitch": 60.0, "azim": 0.0, "target": (0, 0, 40.0), "dist": 330.0}); out[shape + "_game_cam"] = p
        bpy.data.objects.remove(o)
    out["camera"] = {"layout_game_cam": game, "layout_three_quarter": tq, "pieces": "hfov 40, pitch 35 azim 32 (three-quarter) or pitch 60 azim 0 (steep), target (0,0,40), distance 330",
                     "pitch_meaning": "degrees below the horizon, azim 0 = looking toward +Y, perspective, horizontal FOV, sensor fit horizontal",
                     "layout_resolution": [1920, 1080], "piece_resolution": [1000, 1000]}
    out["light"] = {"type": "sun, energy 1.0, same rotation as the camera (camera-following)", "world": "colour #C8C8CC strength 0.35 (also the background colour of the sky)", "floor": "plane z = 0, #B9B9BE, roughness 1",
                    "view_transform": "Standard", "engine": "EEVEE", "colours": "top #8E89A8, rim #C4BFDB (R), side #55516F (G); preview tones only"}
    return out


main()
