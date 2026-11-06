"""Rock slab dual-grid pieces (task-230-02, chamfer and facets task-480-01). Blender route, deterministic, no Unreal import.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/rock_slab_pieces.py -- [--out-dir Art/Meshes/rock-slab] [--no-preview]
Writes SM_RockSlab_<Shape>_01.fbx (full, edge, outer, inner, diagonal), SM_RockSlab_<Shape>_02.fbx (all but full, which has no side), SM_RockSlab.report.json and previews.
Units: 1 Blender unit = 1 cm (= 1 uu). Piece = 67 x 67 centred on a cell corner; its 4 corners are the 4 cell centres.
Canonical orientations (Blender axes, corners A(-,-) B(+,-) C(+,+) D(-,+)); the engine rotates by k x 90 deg about Z (x,y)->(-y,x):
  full: A B C D rock; edge: B C rock (rock on +X); outer: C rock; inner: B C D rock (notch at A); diagonal: A C rock (joined saddle).
Seams: every outline crosses a piece border at the middle of the border, perpendicular to it, and the side rows and the chamfer
inner point at that crossing are fixed offsets along the border (no jitter), so any two pieces and variants share exact vertices.
Top edge (task-480): the outer edge of the top drops CHAMFER_H along the outline (same outline seen from above, top height kept) and
rises to the top over RIM_W inward; these chamfer triangles carry vertex colour R = 1 (the emissive mark of M_RockSlab) and B going from
0 at the top to 1 at the outer edge (position across the chamfer).
Sides (task-480): large facets of irregular width between columns on outline vertices, interior columns offset and skewed, end
columns fixed. Side faces carry G = 1, a per-facet random value in R that M_RockSlab turns into the facet tint, and B = 1 - z / TOP
(foot darkening). The material reads these instead of world heights: ObjectPositionWS does not give the piece height reliably."""
import bpy, bmesh, math, os, sys, json, random
from mathutils import Vector, Euler

H = 33.5            # half piece = half cell (67 uu)
TOP = 100.0         # rock top above the floor top
Z_SCALE = 153.0 / TOP   # engine z scale of the pieces (BlockHeight / RockPieceMeshHeight); only used to size the chamfer in world uu
RIM_W = 8.0         # chamfer width on the top (world uu, x and y are not scaled)
CHAMFER_H = 8.0 / Z_SCALE   # chamfer drop in mesh units = 8 world uu, about 5 % of the block height
RIM_FLAT_W = 4.0    # flat rim band of the footprint that rock_slab_broken.py cuts (keeps its blocks as they were)
Z_CHAMFER = TOP - CHAMFER_H
LIP_DROP = 9.0      # z drop of the facet start row under the chamfer (mesh units): the outline curve meets the straight facet tops there
LIP_OFF = 1.5       # outward offset of the facet start row at a piece border
ROW_OFF = (LIP_OFF, 4.0, 9.0)  # side columns at a piece border: outward offset at the facet start, mid and foot
ROW_Z_MID = 48.0
FACET_MIN_W, FACET_MAX_W = 16.0, 46.0   # facet width limits along the outline (chord, uu); widths in steps of the outline segments
PIECE_VARIANTS = {"full": (1,), "edge": (1, 2), "outer": (1, 2), "inner": (1, 2), "diagonal": (1, 2)}
MASKS = {"full": 15, "edge": 6, "outer": 4, "inner": 14, "diagonal": 5}
CANON = {"A": (-H, -H), "B": (H, -H), "C": (H, H), "D": (-H, H)}
BUDGET = {"full": 2, "edge": 48, "outer": 48, "inner": 64, "diagonal": 96}
PIECE_NAME = "SM_RockSlab_%s_%02d"
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


def facet_bounds(pts, rng, avoid=None):
    """indices of the facet columns along an outline chain: 0, ..., n, consecutive columns FACET_MIN_W to FACET_MAX_W apart (chord)"""
    n = len(pts) - 1
    best = None
    for _ in range(400):
        cols = [0]
        while cols[-1] < n:
            opts = [j for j in range(cols[-1] + 1, n + 1) if FACET_MIN_W <= (pts[j] - pts[cols[-1]]).length <= FACET_MAX_W]
            if not opts:
                cols = None
                break
            cols.append(rng.choice(opts))
        if cols and cols != avoid:
            return cols
        if cols:
            best = cols
    return best or [0, n]


def build_piece(shape, variant=1, flat_rim=False):
    """variant: facet layout (1, 2). flat_rim: the footprint of rock_slab_broken.py (flat rim band RIM_FLAT_W at the top height), same outline and foot chain."""
    seed = {"full": 1, "edge": 2, "outer": 3, "inner": 4, "diagonal": 5}[shape] * 7919
    b = Builder(shape, seed)
    b.foot_chains = []    # per outline chain: (outline points, foot position (x, y) under each of them), for the broken variants
    b.facets = 0
    rng = random.Random(seed * 13 + 4001)
    TOPC, RIMC = (0, 0, 0), (1, 0, 0)
    if shape == "full":
        b.face([(-H, -H, TOP), (H, -H, TOP), (H, H, TOP), (-H, H, TOP)], TOPC, up=1, kind="top")
        return b
    n = SEG[shape]
    outlines = outline_pts(shape, n)
    body_chains = []
    for pts, ref in outlines:
        raw_ins = inward_normals(pts, ref)
        # the test against ref can turn a normal outward where the wobble leans far from ref (a chamfer wing outside the outline):
        # along the chain each normal keeps the side of the previous one. The flat footprint of the broken cut keeps the raw normals.
        ins = [raw_ins[0]]
        for nn in raw_ins[1:]:
            ins.append(nn if nn.dot(ins[-1]) >= 0 else -nn)
        # variant 2 draws after variant 1 from the same stream, so it never repeats its columns
        cols = None
        for v in range(1, variant + 1):
            cols = facet_bounds(pts, rng, avoid=cols)
            state = [(rng.uniform(0.0, 3.5), rng.uniform(-1.5, 4.0), rng.uniform(-2.0, 5.0), rng.uniform(-14.0, 14.0), rng.uniform(-3.0, 3.0), rng.uniform(-6.0, 6.0),
                      2.5 if rng.random() < 0.35 else 0.0) for _ in range(n + 1)]
            tints = [rng.random() for _ in range(n + 1)]
        zp = TOP if flat_rim else Z_CHAMFER
        P = [Vector((p.x, p.y, zp)) for p in pts]
        rw = RIM_FLAT_W if flat_rim else RIM_W
        Q = [Vector((p.x + rw * nn.x, p.y + rw * nn.y, TOP)) for p, nn in zip(pts, raw_ins if flat_rim else ins)]
        def at(i, off, tx, z):
            p, nn = pts[i], ins[i]
            tang = Vector((-nn.y, nn.x))
            return Vector((p.x - nn.x * off + tang.x * tx, p.y - nn.y * off + tang.y * tx, z))
        col = {}
        for i in cols:
            if i in (0, n):      # end column on a piece border: fixed, shared by every shape and variant
                col[i] = (at(i, ROW_OFF[0], 0.0, Z_CHAMFER - LIP_DROP), at(i, ROW_OFF[1], 0.0, ROW_Z_MID), at(i, ROW_OFF[2], 0.0, 0.0))
            else:
                dl, dm, df, dz, tm, tf, ridge = state[i]
                col[i] = (at(i, LIP_OFF + dl + ridge, 0.0, Z_CHAMFER - LIP_DROP), at(i, ROW_OFF[1] + dm + ridge, tm, ROW_Z_MID + dz), at(i, ROW_OFF[2] + df + ridge, tf, 0.0))
        for i in range(n):
            fr = b.face([P[i], P[i + 1], Q[i + 1], Q[i]], RIMC, up=1, kind="rim")
            if not flat_rim:
                for l in fr.loops:
                    l[b.col] = (1.0, 0.0, 1.0 if l.vert.co.z < TOP - 1e-3 else 0.0, 1.0)
        foot = [None] * (n + 1)
        for fi in range(len(cols) - 1):
            c0, c1 = cols[fi], cols[fi + 1]
            b.facets += 1
            colr = (tints[c0], 1, 0)
            outd = Vector((-ins[c0].x - ins[c1].x, -ins[c0].y - ins[c1].y, 0.0))
            for i in range(c0, c1 + 1):
                t = (i - c0) / (c1 - c0)
                f = col[c0][2].lerp(col[c1][2], t)
                foot[i] = (f.x, f.y)
            # one winding for the whole facet, read from its large quad: a lip triangle can lean almost flat, and forcing each
            # triangle outward by its own normal turned some of them face down, leaving a hole seen from above
            e1, e2 = col[c1][0] - col[c0][0], col[c1][2] - col[c0][0]
            flip = e1.cross(e2).dot(outd) < 0
            def side_tri(tr):
                fs = b.face(tr[::-1] if flip else tr, colr, kind="side")
                for l in fs.loops:
                    l[b.col] = (colr[0], 1.0, 1.0 - l.vert.co.z / TOP, 1.0)
            # lip: the outline polyline fans onto the straight start of the facet, just under the chamfer
            m = (c0 + c1) // 2
            for i in range(c0, c1):
                side_tri([P[i], P[i + 1], col[c0][0] if i < m else col[c1][0]])
            side_tri([P[m], col[c1][0], col[c0][0]])
            # the facet itself: two rows, the kink height differs per column
            for r in range(2):
                a0, a1, b0, b1 = col[c0][r], col[c1][r], col[c0][r + 1], col[c1][r + 1]
                tris = ([a0, a1, b1], [a0, b1, b0]) if (fi + r) % 2 == 0 else ([a0, a1, b0], [a1, b1, b0])
                for tr in tris:
                    side_tri(tr)
        for i in cols:
            b.seam_pts += [tuple(v) for v in col[i]]
        b.seam_pts += [tuple(v) for v in P] + [tuple(v) for v in Q]
        b.foot_chains.append((pts, foot))
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
    for shape, var in [(sh, v) for sh in MASKS for v in PIECE_VARIANTS[sh]]:
        b = build_piece(shape, var)
        N, s = finish(b)
        builders[(shape, var)] = b
        name = PIECE_NAME % (shape.capitalize(), var)
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
            cs = [tuple(round(c, 3) for c in ca[li].color[:2]) for li in p.loop_indices]     # B varies inside a face
            if len(set(cs)) != 1:
                bad += 1
            r, g = cs[0]
            if g == 1: side_tris += 1
            elif r == 1 and g == 0: rim_tris += 1
            elif r == 0 and g == 0: top_tris += 1
            else: bad += 1
        # top tris must be on the top plane, chamfer tris must stay between the chamfer foot and the top and face up
        for p in me.polygons:
            zc = [me.vertices[i].co.z for i in p.vertices]
            ca0 = ca[p.loop_indices[0]].color
            if ca0[1] < 0.5 and ca0[0] > 0.5 and (min(zc) < Z_CHAMFER - 1e-3 or p.normal.z < 0.5): bad += 1
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
        report["shapes"]["%s_%02d" % (shape, var)] = {
            "file": "Art/Meshes/rock-slab/%s.fbx" % name, "facets": b.facets, "tris": tris, "budget": BUDGET[shape], "within_budget": tris <= BUDGET[shape],
            "dimensions_cm": dims, "bounds_min": [round(min(xs), 2), round(min(ys), 2), round(min(zs), 2)], "bounds_max": [round(max(xs), 2), round(max(ys), 2), round(max(zs), 2)],
            "materials": ["M_RockSlab"], "uv_layers": [u.name for u in me.uv_layers],
            "uv_checks": {"UVMap": "planar top-down projection, x and y / 100 (world position drives the top in the material)",
                          "LightMap": {"triangles": tris, "overlapping_pairs": overlaps, "bounds": [round(bmin, 4), round(bmax, 4)], "min_gap_px_at_256": round(mingap * 256, 3), "min_border_px_at_256": round(min(bmin, 1 - bmax) * 256, 3), "pass": bool(uv_ok)}},
            "vertex_colour": {"attribute": "Color (corner, float)", "export": "colors_type=LINEAR", "rim_tris_R1": rim_tris, "side_tris_G1": side_tris, "top_tris_0": top_tris, "inconsistent_faces": bad, "pass": bad == 0,
                              "rule": "R=1, G=0 only on the chamfer along the outline, B = position across it (0 top, 1 outer edge); G=1 only on side faces, R = facet random value, B = 1 - z / 100; top body 0,0,0"},
            "all_flat_shaded": all(not p.use_smooth for p in me.polygons),
            "top_and_chamfer_z": sorted(set(round(me.vertices[i].co.z, 4) for p in me.polygons if ca[p.loop_indices[0]].color[1] < 0.5 for i in p.vertices)),
            "outline_seen_from_above": "the chamfer foot is the outline of task-230-02 (outline_pts unchanged)",
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
    def variant_at(shape, i, j):
        vs = PIECE_VARIANTS[shape]
        return vs[(i * 3 + j) % len(vs)]
    for j in range(Hh + 1):
        for i in range(W + 1):
            m = cell(i - 1, j - 1) | cell(i, j - 1) << 1 | cell(i, j) << 2 | cell(i - 1, j) << 3
            cs = case_of(m)
            if cs:
                placed[(i, j)] = cs
                used[cs[0]] = used.get(cs[0], 0) + 1
    def world_pts(i, j):
        shape, k = placed[(i, j)]
        return [(rot(p, k)[0] + i * 67.0, rot(p, k)[1] + j * 67.0, p[2]) for p in builders[(shape, variant_at(shape, i, j))].seam_pts]
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
        "method": "test layout 8 x 8 cells (%s), pieces placed with rotation k x 90 deg from the marching-squares case; for every pair of adjacent placed pieces (variants mixed by position) the chamfer and side-column vertices lying on the shared border are compared as sets (3 mm rounding, exact in practice)" % " / ".join(LAYOUT),
        "pieces_placed": len(placed), "shape_counts": used, "adjacent_pairs_checked": pairs, "border_vertices_compared": crossings, "mismatching_pairs": mism,
        "all_5_shapes_used": sorted(used) == sorted(MASKS), "pass": bool(mism == 0 and sorted(used) == sorted(MASKS)),
        "guarantee": "the outline crosses a border only at its middle (0,+-33.5 / +-33.5,0), perpendicular to it; the chamfer points and the side column at that point (z 94.77 offset 0, z 85.77 offset 1.5, z 48 offset 4, z 0 offset 9) are fixed offsets along the border with no jitter, independent of shape, variant and rotation; facet offsets apply only to interior columns",
    }
    if mism: report["warnings"].append("seam mismatch")

    # ---- round-trip check of one exported FBX
    # (re-import in a clean scene: tris, vertex colour, UV layers survive)
    rt = {}
    for shape, var in [(sh, v) for sh in MASKS for v in PIECE_VARIANTS[sh]]:
        name = PIECE_NAME % (shape.capitalize(), var)
        before = set(bpy.data.objects)
        bpy.ops.import_scene.fbx(filepath=os.path.join(OUTDIR, name + ".fbx"))
        new = [o for o in bpy.data.objects if o not in before and o.type == "MESH"][0]
        me = new.data
        mx = [new.matrix_world @ v.co for v in me.vertices]
        rt["%s_%02d" % (shape, var)] = {"tris": len(me.polygons), "colour_attributes": [c.name for c in me.color_attributes], "uv_layers": [u.name for u in me.uv_layers],
                     "height_after_import": round(max(v.z for v in mx) - min(v.z for v in mx), 3), "object_scale": [round(x, 4) for x in new.scale]}
        for o in [o for o in bpy.data.objects if o not in before]:
            bpy.data.objects.remove(o)
    report["fbx_roundtrip_blender_import"] = rt
    report["fbx_export"] = {"apply_scale_options": "FBX_SCALE_UNITS", "global_scale": 1.0, "axes": "Blender default", "colors_type": "LINEAR", "mesh_smooth_type": "FACE"}
    report["variant"] = "_01 and _02 for edge, outer, inner, diagonal (same outline, different facets); full has no side, _01 only"
    report["layout"] = {"rows_top_to_bottom": LAYOUT, "placed": {"%d,%d" % k: [v[0], v[1] * 90] for k, v in placed.items()}}
    if PREVIEW:
        report["preview"] = previews(builders, placed, variant_at)
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
    mix1.inputs[6].default_value = srgb("#8E89A8"); mix1.inputs[7].default_value = srgb("#F2A03A")
    nt.links.new(sep.outputs["Red"], mix1.inputs[0])
    ramp = nt.nodes.new("ShaderNodeValToRGB")     # facet tint from R on side faces, preview tones only
    ramp.color_ramp.interpolation = "CONSTANT"
    ramp.color_ramp.elements[0].color = srgb("#2E2442"); ramp.color_ramp.elements[1].position = 0.45; ramp.color_ramp.elements[1].color = srgb("#4A3A68")
    e = ramp.color_ramp.elements.new(0.85); e.color = srgb("#6A5690")
    nt.links.new(sep.outputs["Red"], ramp.inputs["Fac"])
    mix2 = nt.nodes.new("ShaderNodeMix"); mix2.data_type = "RGBA"
    nt.links.new(ramp.outputs["Color"], mix2.inputs[7])
    nt.links.new(mix1.outputs[2], mix2.inputs[6]); nt.links.new(sep.outputs["Green"], mix2.inputs[0])
    nt.links.new(mix2.outputs[2], bsdf.inputs["Base Color"])
    return m


def combined_object(name, items, builders):
    """items: list of ((shape, variant), k, cx, cy) -> one mesh object (positions transformed, colours kept)."""
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
    sun.rotation_euler = (math.radians(50.0), 0.0, math.radians(cam_spec["azim"] + 55.0))     # fixed offset from the camera azimuth, so facets differ in tone
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    bpy.data.objects.remove(cam); bpy.data.objects.remove(sun)


def previews(builders, placed, variant_at):
    os.makedirs(PREV_DIR, exist_ok=True)
    mat = make_material()
    floor_m = bpy.data.materials.new("FLOOR"); floor_m.use_nodes = True
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = srgb("#B9B9BE")
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 1.0
    out = {}
    # layout
    items = [((s, variant_at(s, i, j)), k, i * 67.0, j * 67.0) for (i, j), (s, k) in placed.items()]
    lay = combined_object("Layout", items, builders); lay.data.materials.append(mat)
    fl = bpy.data.meshes.new("Floor"); fl.from_pydata([(-3000, -3000, 0), (4000, -3000, 0), (4000, 4000, 0), (-3000, 4000, 0)], [], [(0, 1, 2, 3)])
    flo = bpy.data.objects.new("Floor", fl); bpy.context.scene.collection.objects.link(flo); fl.materials.append(floor_m)
    sc = setup_render(1920, 1080)
    cen = (268.0, 268.0, 0.0)
    game = {"hfov": 90.0, "pitch": 60.0, "azim": 0.0, "target": cen, "dist": 640.0}
    p = os.path.join(PREV_DIR, "rockslab_layout_game_cam.png"); render_one(sc, [lay, flo], p, game); out["layout_game_cam"] = p
    tq = {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": cen, "dist": 1050.0}
    p = os.path.join(PREV_DIR, "rockslab_layout_three_quarter.png"); render_one(sc, [lay, flo], p, tq); out["layout_three_quarter"] = p
    low = {"hfov": 50.0, "pitch": 12.0, "azim": 28.0, "target": (268.0, 268.0, 50.0), "dist": 760.0}
    p = os.path.join(PREV_DIR, "rockslab_layout_low_side.png"); render_one(sc, [lay, flo], p, low); out["layout_low_side"] = p
    bpy.data.objects.remove(lay)
    # per piece and variant: straight from above (outline) + low side view, each piece in canonical orientation; azim looks at its open side
    sc.render.resolution_x, sc.render.resolution_y = 800, 800
    LOW_AZIM = {"full": 0.0, "edge": 90.0, "outer": 45.0, "inner": 45.0, "diagonal": 135.0}
    for shape, var in [(sh, v) for sh in MASKS for v in PIECE_VARIANTS[sh]]:
        tag = "%s_%02d" % (shape, var)
        o = combined_object("P_" + tag, [((shape, var), 0, 0.0, 0.0)], builders); o.data.materials.append(mat)
        p = os.path.join(PREV_DIR, "rockslab_%s_top.png" % tag)
        render_one(sc, [o, flo], p, {"hfov": 40.0, "pitch": 89.9, "azim": 0.0, "target": (0, 0, 40.0), "dist": 330.0}); out[tag + "_top"] = p
        p = os.path.join(PREV_DIR, "rockslab_%s_low_side.png" % tag)
        render_one(sc, [o, flo], p, {"hfov": 40.0, "pitch": 14.0, "azim": LOW_AZIM[shape], "target": (0, 0, 45.0), "dist": 330.0}); out[tag + "_low_side"] = p
        bpy.data.objects.remove(o)
    out["camera"] = {"layout_game_cam": game, "layout_three_quarter": tq, "layout_low_side": low, "pieces": "hfov 40, pitch 89.9 (top) or pitch 14 at the piece open side (low side), target (0,0,40 or 45), distance 330",
                     "pitch_meaning": "degrees below the horizon, azim 0 = looking toward +Y, perspective, horizontal FOV, sensor fit horizontal",
                     "layout_resolution": [1920, 1080], "piece_resolution": [1000, 1000]}
    out["light"] = {"type": "sun, energy 1.0, tilted 50 deg, azimuth = camera + 55 deg", "world": "colour #C8C8CC strength 0.35 (also the background colour of the sky)", "floor": "plane z = 0, #B9B9BE, roughness 1",
                    "view_transform": "Standard", "engine": "EEVEE", "colours": "top #8E89A8, chamfer #F2A03A (R), sides #2E2442 / #4A3A68 / #6A5690 by facet R; preview tones only"}
    return out


main()
