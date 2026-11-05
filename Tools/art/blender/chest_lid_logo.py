"""Yellow Kessler-Voss mark cut into the chest lid top (task-314-01). Blender route, no Meshy call. Headless:
  cut    : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_lid_logo.py -- cut [--w 22] [--d 12] [--in <fbx>] [--out <fbx>]
  render : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_lid_logo.py -- render [--lid <fbx>] [--prefix logo] [--rdir <dir>]
Input  : Art/Source/chest/clean/chest_lid.fbx as cut by chest_cut.py (task-306-02, task-309-07): origin on the rear seam line, the lid extends toward -Y,
         X long side, Z up, 1 Blender unit = 1 cm. The cut refuses a lid that already has a Logo slot (run chest_cut.py cut first to start over).
Cut    : 1 the middle band of the lid top (flat faces at z ~18.1 between the groove at y ~-22 and the step at y ~-35.5, lid space) is measured at x = 0;
         2 W = 22 cm by default: the flat band is only 11.3 cm deep, so D (asked 12, Q3) is capped to the band minus 0.8 cm margins (9.7 cm) and W 22 keeps
           the proportions of the sheet mark (Q3 said about 25 x 12);
           the mark (defaults Q1 to Q5 of task-314: the simplified mark of vault/assets/chest-turnaround.webp, no text) is centred on that band, W x D cm,
           tips along X: a wide flat diamond (two side lobes) over a taller centre diamond, with a diamond hole;
         3 the band faces under the mark are re-triangulated (constrained Delaunay) with the mark outline and hole as constraint edges, so the mark faces lie
           in the lid surface itself (no offset, no new volume); faces inside the mark take the new slot Logo, the others keep M_BonusChest and their UVs;
         4 boundary vertices of the band are reused, so the lid stays closed where it was; flat shading; export FBX_SCALE_UNITS, origin unchanged.
Output : the lid FBX (slots M_BonusChest, Logo) and Art/Source/chest/clean/chest_lid_logo.report.json (tris before/after, Logo faces and area, pivot and bbox
         before/after). Renders: closed chest top and three-quarter with the camera and light of the task-306 cut_* renders, a close top, and the same
         renders next to the sheet (Art/Source/chest/clean/renders/logo_*.png).
Logo colour: company yellow #F5BA20 (the chaser body yellow), roughness 0.45, no emission (Q5)."""
import bpy, bmesh, json, math, os, sys
import numpy as np
from mathutils import Vector, Matrix, Euler
from mathutils.geometry import delaunay_2d_cdt, intersect_point_tri_2d
from mathutils.bvhtree import BVHTree

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rr_blender as rr

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
CLEAN = os.path.join(ROOT, "Art", "Source", "chest", "clean")
LID = os.path.join(CLEAN, "chest_lid.fbx")
TEX = os.path.join(CLEAN, "chest_flat_basecolor.png")
SHEET = os.path.join(ROOT, "vault", "assets", "chest-turnaround.webp")
REPORT = os.path.join(CLEAN, "chest_lid_logo.report.json")
YELLOW = "#F5BA20"
MARGIN = 0.8              # cm between the mark and the band edges (groove at the back, step at the front)
argv = sys.argv[sys.argv.index("--") + 1:]
mode = argv[0]


def opt(name, default):
    return type(default)(argv[argv.index(name) + 1]) if name in argv else default


def mark_outline(w, d):
    """Outer outline (CCW) and hole (CW) of the mark, centred on (0, 0), in cm. Side lobes: half-depth 0.6 d, centred at +-(w/2 - lw) with half-width lw."""
    hw, hd = w / 2.0, d / 2.0
    cw = 0.52 * hw            # centre diamond half-width
    lw = 0.52 * hw            # side lobe half-width
    lc = hw - lw              # side lobe centre x
    lh = 0.6 * hd             # side lobe half-depth
    # notch: lobe inner descending edge y = lh (1 - (x - lc... mirrored)) meets the centre rising edge y = hd (1 - |x| / cw)
    # left side: lobe desc y = lh * (lw - (x + lc)) / lw for x > -lc ; centre rising y = hd * (x + cw) / cw
    x = (lh * (lw - lc) / lw - hd) / (hd / cw + lh / lw)
    y = hd * (x + cw) / cw
    up = [(-hw, 0.0), (-lc, lh), (x, y), (0.0, hd), (-x, y), (lc, lh), (hw, 0.0)]
    outer = up + [(px, -py) for px, py in reversed(up[1:-1])]
    outer = outer[::-1]  # CCW: tip left, bottom, tip right, top
    if signed_area(outer) < 0: outer = outer[::-1]
    hx, hy = 0.36 * hw, 0.37 * hd
    hole = [(-hx, 0.0), (0.0, hy), (hx, 0.0), (0.0, -hy)]
    if signed_area(hole) > 0: hole = hole[::-1]
    return outer, hole


def signed_area(poly):
    return 0.5 * sum(poly[i][0] * poly[(i + 1) % len(poly)][1] - poly[(i + 1) % len(poly)][0] * poly[i][1] for i in range(len(poly)))


def inside(poly, p):
    c = False; n = len(poly)
    for i in range(n):
        (x1, y1), (x2, y2) = poly[i], poly[(i + 1) % n]
        if (y1 > p[1]) != (y2 > p[1]) and p[0] < (x2 - x1) * (p[1] - y1) / (y2 - y1) + x1: c = not c
    return c


def bary(p, a, b, c):
    v0, v1, v2 = b - a, c - a, p - a
    d00, d01, d11, d20, d21 = v0.dot(v0), v0.dot(v1), v1.dot(v1), v2.dot(v0), v2.dot(v1)
    den = d00 * d11 - d01 * d01
    v = (d11 * d20 - d01 * d21) / den; w = (d00 * d21 - d01 * d20) / den
    return 1.0 - v - w, v, w


def lid_object(path):
    objs = rr.import_fbx(path)
    assert len(objs) == 1, "expected one mesh in %s" % path
    return objs[0]


def ident(o):
    return all(abs(o.matrix_world[i][j] - (1.0 if i == j else 0.0)) < 1e-5 for i in range(4) for j in range(4))


def bbox(o):
    V = np.array([v.co[:] for v in o.data.vertices])
    return V.min(0).round(2).tolist(), V.max(0).round(2).tolist()


if mode == "cut":
    W = opt("--w", 22.0); D = opt("--d", 12.0)
    src = opt("--in", LID); dst = opt("--out", LID)
    o = lid_object(src)
    me = o.data
    assert ident(o), "lid object transform is not identity: %s" % o.matrix_world
    assert "Logo" not in [m.name for m in me.materials if m], "the lid already has a Logo slot: re-cut it with chest_cut.py first"
    bm0 = bmesh.new(); bm0.from_mesh(me); open_before = sum(1 for e in bm0.edges if len(e.link_faces) == 1); bm0.free()
    tris_before = rr.tri_count(o); bb_before = bbox(o); slots_before = [m.name for m in me.materials]
    # 1 middle band at x = 0
    bvh = BVHTree.FromObject(o, bpy.context.evaluated_depsgraph_get())
    ys = []
    for i in range(0, 200):
        y = -20.0 - i * 0.1
        h = bvh.ray_cast(Vector((0, y, 100)), Vector((0, 0, -1)))
        if h[0] is not None and h[1].z > 0.99 and 17.8 < h[0].z < 18.3: ys.append(y)
    # longest contiguous run
    runs = []; cur = [ys[0]]
    for a, b in zip(ys, ys[1:]):
        if abs(a - b - 0.1) < 1e-6: cur.append(b)
        else: runs.append(cur); cur = [b]
    runs.append(cur)
    band = max(runs, key=len)
    band_y = (max(band), min(band)); cy = 0.5 * (band_y[0] + band_y[1])
    band_depth = band_y[0] - band_y[1]
    D_asked = D; D = min(D, round(band_depth - 2 * MARGIN, 2))   # keep a margin to the groove and the step (the band is about 12 cm deep)
    print("BAND", band_y, band_depth, "D", D)
    outer, hole = mark_outline(W, D)
    outer = [(x, y + cy) for x, y in outer]; hole = [(x, y + cy) for x, y in hole]
    xs = [p[0] for p in outer]; yy = [p[1] for p in outer]
    box = (min(xs) - 0.5, max(xs) + 0.5, min(yy) - 0.5, max(yy) + 0.5)
    # 2 faces of the band under the mark
    bm = bmesh.new(); bm.from_mesh(me)
    bm.verts.ensure_lookup_table(); bm.faces.ensure_lookup_table()
    uvl = bm.loops.layers.uv.active

    def overlaps(f):
        P = [v.co.xy for v in f.verts]
        if max(p.x for p in P) < box[0] or min(p.x for p in P) > box[1] or max(p.y for p in P) < box[2] or min(p.y for p in P) > box[3]: return False
        return True
    sel = [f for f in bm.faces if f.normal.z > 0.99 and 17.8 < f.calc_center_median().z < 18.3 and band_y[1] - 1 < f.calc_center_median().y < band_y[0] + 1 and overlaps(f)]
    assert all(len(f.verts) == 3 for f in sel), "band faces are not triangles"
    # every mark vertex must lie in a selected face
    for p in outer + hole:
        assert any(intersect_point_tri_2d(Vector(p), *[v.co.xy for v in f.verts]) for f in sel), "mark vertex %s outside the band faces" % (p,)
    # 3 constrained triangulation
    in_verts = []; vid = {}
    for f in sel:
        for v in f.verts:
            if v.index not in vid: vid[v.index] = len(in_verts); in_verts.append(v)
    coords = [v.co.xy.copy() for v in in_verts]
    faces_in = [[vid[v.index] for v in f.verts] for f in sel]
    n0 = len(coords)
    mk = [Vector(p) for p in outer] + [Vector(p) for p in hole]
    coords += mk
    edges = [(n0 + i, n0 + (i + 1) % len(outer)) for i in range(len(outer))]
    no = n0 + len(outer)
    edges += [(no + i, no + (i + 1) % len(hole)) for i in range(len(hole))]
    vc, ve, vf, ov, oe, of = delaunay_2d_cdt(coords, edges, faces_in, 1, 1e-4, True)
    src_tris = [([v.co.copy() for v in f.verts], [l[uvl].uv.copy() for l in f.loops]) for f in sel]
    seltri2d = [[c.xy for c in t[0]] for t in src_tris]

    def host(p):
        for k, T in enumerate(seltri2d):
            if intersect_point_tri_2d(p, *T): return k
        # nearest by barycentric error (points on an edge with rounding)
        return min(range(len(seltri2d)), key=lambda k: -min(bary(p, *seltri2d[k])))

    def z_at(p):
        k = host(p); a, b, c = src_tris[k][0]; u, v, w = bary(p, a.xy, b.xy, c.xy)
        return u * a.z + v * b.z + w * c.z
    logo_idx = len(me.materials)
    me.materials.append(rr.make_material("Logo", YELLOW, roughness=0.45))
    out_v = []
    for i, c in enumerate(vc):
        orig = [j for j in ov[i] if j < n0]
        out_v.append(in_verts[orig[0]] if orig else bm.verts.new((c.x, c.y, z_at(Vector(c)))))
    mat0 = sel[0].material_index
    for f in sel: assert f.material_index == mat0
    bmesh.ops.delete(bm, geom=sel, context="FACES_ONLY")
    logo_faces = 0; logo_area = 0.0; new_faces = []
    for fv in vf:
        P = [Vector(vc[i]) for i in fv]
        cen = sum(P, Vector((0, 0))) / len(P)
        f = bm.faces.new([out_v[i] for i in fv])
        f.normal_update()
        if f.normal.z < 0: f.normal_flip()
        is_logo = inside(outer, cen) and not inside(hole, cen)
        f.material_index = logo_idx if is_logo else mat0
        k = host(cen); (a, b, c), (ua, ub, uc) = src_tris[k]
        for l in f.loops:
            u, v, w = bary(l.vert.co.xy, a.xy, b.xy, c.xy)
            l[uvl].uv = ua * u + ub * v + uc * w
        f.smooth = False
        if is_logo: logo_faces += 1; logo_area += f.calc_area()
        new_faces.append(f)
    loose = [v for v in bm.verts if not v.link_faces]
    bmesh.ops.delete(bm, geom=loose, context="VERTS")
    up = sum(1 for f in new_faces if f.normal.z > 0.99)
    bm.to_mesh(me); bm.free()
    for p in me.polygons: p.use_smooth = False
    me.update()
    tris_after = rr.tri_count(o); bb_after = bbox(o)
    mark_area = abs(signed_area(outer)) - abs(signed_area(hole))
    # open edges check: same count as before on the band boundary (no crack)
    bm2 = bmesh.new(); bm2.from_mesh(me)
    open_after = sum(1 for e in bm2.edges if len(e.link_faces) == 1); bm2.free()
    os.makedirs(os.path.dirname(os.path.abspath(dst)), exist_ok=True)
    bpy.ops.object.select_all(action="DESELECT"); o.select_set(True); bpy.context.view_layer.objects.active = o
    bpy.ops.export_scene.fbx(filepath=dst, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                             add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
    # read back
    o2 = lid_object(dst)
    me2 = o2.data
    fps = {}
    for p in me2.polygons:
        n = me2.materials[p.material_index].name; fps[n] = fps.get(n, 0) + 1
    rep = rr.write_report(None, [o2], warnings=[])
    rep.update(script="Tools/art/blender/chest_lid_logo.py", source=os.path.relpath(src, ROOT).replace("\\", "/"), output=os.path.relpath(dst, ROOT).replace("\\", "/"),
               units="1 Blender unit = 1 cm = 1 uu; lid space: origin on the rear seam line, lid toward -Y, Z up; FBX_SCALE_UNITS",
               defaults="task-314 Q1 simplified mark, no text; Q2 cut into the lid top faces, own slot Logo, flat yellow; Q3 centred on the lid top band, W x D cm; Q4 <= 150 extra tris; Q5 company yellow, no emissive",
               tris_before=tris_before, tris_after=rr.tri_count(o2), tris_added=rr.tri_count(o2) - tris_before,
               slots_before=slots_before, slots_after=[m.name for m in me2.materials], faces_per_slot=fps,
               logo_faces=fps.get("Logo", 0), logo_area_cm2=round(logo_area, 2), mark_area_cm2=round(mark_area, 2),
               mark_size_cm=[W, D], mark_depth_asked_cm=D_asked, band_margin_cm=MARGIN, mark_centre_lid_cm=[0.0, round(cy, 2), round(z_at(Vector((0, cy))), 2)],
               band_y_lid_cm=[round(band_y[1], 2), round(band_y[0], 2)], band_faces_retriangulated=len(sel), new_faces=len(vf), new_faces_facing_up=up,
               mark_outline_lid_cm=[[round(x, 3), round(y, 3)] for x, y in outer], mark_hole_lid_cm=[[round(x, 3), round(y, 3)] for x, y in hole],
               pivot="object origin unchanged (identity transform, vertices local to the rear seam line, (0, 32, 41.25) from the body origin)",
               object_transform_identity=ident(o2),
               bbox_before_cm=bb_before, bbox_after_cm=bbox(o2), bbox_unchanged=bb_before == bbox(o2), open_edges_before=open_before, open_edges_after=open_after,
               colour=dict(hex=YELLOW, roughness=0.45, emission=0.0, note="company yellow = chaser body yellow (Tools/art/blender/chaser/common.py)"))
    json.dump(rep, open(REPORT, "w", encoding="utf-8"), indent=2)
    print("LOGO", json.dumps({k: rep[k] for k in ("tris_before", "tris_after", "tris_added", "faces_per_slot", "logo_area_cm2", "mark_area_cm2", "mark_centre_lid_cm", "band_y_lid_cm", "bbox_unchanged", "object_transform_identity", "new_faces_facing_up", "new_faces")}))

elif mode == "render":
    lid_path = opt("--lid", LID); prefix = opt("--prefix", "logo"); rdir = opt("--rdir", os.path.join(CLEAN, "renders"))
    bpy.ops.wm.read_factory_settings(use_empty=True)
    cut = json.load(open(os.path.join(CLEAN, "chest_cut.report.json"), encoding="utf-8"))
    pv = cut["pivots_relative_to_body_origin_cm"]
    P = dict(body=Vector((0, 0, 0)), lid=Vector(pv["lid_hinge_axis_X"]), latch=Vector(pv["latch_top_edge_axis_X"]))
    sc = bpy.context.scene; objs = {}
    for name, path in (("body", os.path.join(CLEAN, "chest_body.fbx")), ("lid", lid_path), ("latch", os.path.join(CLEAN, "chest_latch.fbx"))):
        before = set(sc.objects)
        bpy.ops.import_scene.fbx(filepath=path)
        ob = next(x for x in sc.objects if x not in before and x.type == "MESH")
        ob.data.transform(ob.matrix_world); ob.matrix_world = Matrix.Identity(4)
        for m in ob.data.materials:
            for n in m.node_tree.nodes:
                if n.type == "TEX_IMAGE": n.image = bpy.data.images.load(TEX, check_existing=True)
        objs[name] = ob
    objs["body"].matrix_world = Matrix.Translation(P["body"])
    objs["lid"].matrix_world = Matrix.Translation(P["lid"])
    objs["latch"].matrix_world = Matrix.Translation(P["latch"])
    # camera and light of the task-306 cut_* renders
    sc.render.engine = "BLENDER_EEVEE"; sc.render.resolution_x = sc.render.resolution_y = 800
    sc.render.image_settings.file_format = "PNG"; sc.view_settings.view_transform = "Standard"
    w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
    bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (0.85, 0.85, 0.85, 1); bg.inputs["Strength"].default_value = 0.6
    sd = bpy.data.lights.new("S", "SUN"); sd.energy = 4.0
    s = bpy.data.objects.new("S", sd); sc.collection.objects.link(s); s.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)])
    cd = bpy.data.cameras.new("C"); cd.type = "ORTHO"; cd.ortho_scale = 170.0; cd.clip_start = 1; cd.clip_end = 3000
    cam = bpy.data.objects.new("C", cd); sc.collection.objects.link(cam); sc.camera = cam
    os.makedirs(rdir, exist_ok=True)
    files = []

    def shoot(name, az, el, t, scale):
        cd.ortho_scale = scale
        a, e = math.radians(az), math.radians(el)
        cam.location = t + Vector((math.sin(a) * math.cos(e), -math.cos(a) * math.cos(e), math.sin(e))) * 500
        cam.rotation_euler = (t - cam.location).to_track_quat("-Z", "Y").to_euler()
        f = os.path.join(rdir, "%s_%s.png" % (prefix, name)); sc.render.filepath = f
        bpy.ops.render.render(write_still=True); files.append(f)
    t = Vector((0, 0, 33))
    shoot("closed_top", 0, 89.9, t, 170.0)
    shoot("closed_q", -35, 28, t, 170.0)
    shoot("closed_front", 0, 10, t, 170.0)
    shoot("close_top", 0, 89.9, Vector((0, 0, 60)), 70.0)
    # next to the sheet: render (800 x 800) | sheet front view crop scaled to 800 high
    sh = bpy.data.images.load(SHEET); SW, SH = sh.size
    A = np.array(sh.pixels[:], dtype=np.float32).reshape(SH, SW, 4)[::-1]       # top row first
    crop = A[110:600, 30:850]                                                     # front view of the sheet
    k = 800.0 / crop.shape[0]
    cw = int(crop.shape[1] * k)
    yi = np.clip((np.arange(800) / k).astype(int), 0, crop.shape[0] - 1); xi = np.clip((np.arange(cw) / k).astype(int), 0, crop.shape[1] - 1)
    crop = crop[yi][:, xi]
    for name in ("closed_q", "closed_front", "closed_top"):
        r = bpy.data.images.load(os.path.join(rdir, "%s_%s.png" % (prefix, name)))
        R = np.array(r.pixels[:], dtype=np.float32).reshape(800, 800, 4)[::-1]
        out = np.ones((800, 800 + 20 + cw, 4), dtype=np.float32)
        out[:, :800] = R; out[:, 820:] = crop
        img = bpy.data.images.new("cmp_" + name, out.shape[1], 800, alpha=True)
        img.pixels = out[::-1].ravel().tolist()
        f = os.path.join(rdir, "%s_vs_sheet_%s.png" % (prefix, name.replace("closed_", "")))
        img.filepath_raw = f; img.file_format = "PNG"; img.save(); files.append(f)
    print("RENDERS", json.dumps([os.path.relpath(f, ROOT).replace("\\", "/") for f in files]))
