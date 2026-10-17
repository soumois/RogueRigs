"""Rock shard-cluster overlay (task-230-06). Blender route, deterministic, no Unreal import.
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/rock_plates.py -- [--out-dir Art/Meshes/rock-slab] [--no-preview] [--seed N]
Writes SM_RockPlates_01.fbx, SM_RockPlates.report.json and previews.
Units: 1 Blender unit = 1 cm (= 1 uu). Footprint 67 x 67 (one cell), pivot at the cell centre, z = 0 floor top.
Cluster fills z 40 .. 110 (slab top is 100): a chunky tilted base block, tilted broken plates at varied heights, a rim of
upright shards. Vertex colour as the slab pieces: R=1 on plate tops, G=1 on broken sides and shards."""
import bpy, bmesh, math, os, sys, json, random
from mathutils import Vector, Matrix

H = 33.5
MAX_OUT = 5.0          # allowed overshoot beyond the footprint
TOPSLAB = 100.0
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
OUTDIR = os.path.join(ROOT, "Art", "Meshes", "rock-slab")
if "--out-dir" in argv:
    OUTDIR = os.path.abspath(argv[argv.index("--out-dir") + 1])
SEED = int(argv[argv.index("--seed") + 1]) if "--seed" in argv else 230
PREVIEW = "--no-preview" not in argv
PREV_DIR = os.path.join(OUTDIR, "preview")
NAME = "SM_RockPlates_01"
TRI_MIN, TRI_MAX = 150, 250


class Builder:
    def __init__(self):
        self.bm = bmesh.new()
        self.uv0 = self.bm.loops.layers.uv.new("UVMap")
        self.uv1 = self.bm.loops.layers.uv.new("LightMap")
        self.col = self.bm.loops.layers.float_color.new("Color")

    def tri(self, a, b, c, centre, top):
        """triangle wound so the normal points away from `centre`; top plates get R=1, everything else G=1"""
        a, b, c = Vector(a), Vector(b), Vector(c)
        n = (b - a).cross(c - a)
        if n.dot((a + b + c) / 3 - centre) < 0:
            b, c = c, b
            n = -n
        rgb = (1.0, 0.0, 0.0) if top else (0.0, 1.0, 0.0)
        vs = [self.bm.verts.new(v) for v in (a, b, c)]
        f = self.bm.faces.new(vs)
        f.smooth = False
        for l in f.loops:
            l[self.col] = (rgb[0], rgb[1], rgb[2], 1.0)
            l[self.uv0].uv = (l.vert.co.x / 100.0, l.vert.co.y / 100.0)
        return f


def tilt_matrix(az, deg):
    ax = Vector((-math.sin(az), math.cos(az), 0.0))
    return Matrix.Rotation(math.radians(deg), 3, ax)


def plate(b, rng, c, r, t, n, az, deg, bottom_shrink=0.8, spin=None):
    """flat broken plate: n-gon top (jittered), smaller jittered bottom, no bottom face. 3n-2 tris."""
    M = tilt_matrix(az, deg)
    spin = rng.uniform(0, 2 * math.pi) if spin is None else spin
    ang = [spin + 2 * math.pi * (i + rng.uniform(-0.18, 0.18)) / n for i in range(n)]
    rad = [r * rng.uniform(0.72, 1.12) for _ in range(n)]
    top = [M @ Vector((rad[i] * math.cos(ang[i]), rad[i] * math.sin(ang[i]), t / 2)) + Vector(c) for i in range(n)]
    bot = [M @ Vector((bottom_shrink * rad[i] * math.cos(ang[i]), bottom_shrink * rad[i] * math.sin(ang[i]), -t / 2)) + Vector(c) for i in range(n)]
    ctr = Vector(c)
    up = M @ Vector((0, 0, 1))
    for i in range(1, n - 1):
        f = b.tri(top[0], top[i], top[i + 1], ctr - up * 50, True)
    for i in range(n):
        j = (i + 1) % n
        b.tri(top[i], top[j], bot[j], ctr, False)
        b.tri(top[i], bot[j], bot[i], ctr, False)


def shard(b, rng, c, r, h, n, lean_az, lean_deg):
    """upright broken shard: n-gon base (z = c.z), apex offset by lean; no base face. n tris."""
    ang = [2 * math.pi * (i + rng.uniform(-0.2, 0.2)) / n + rng.uniform(0, 0.3) for i in range(n)]
    base = [Vector((c[0] + r * rng.uniform(0.7, 1.15) * math.cos(a), c[1] + r * rng.uniform(0.7, 1.15) * math.sin(a), c[2])) for a in ang]
    lean = h * math.tan(math.radians(lean_deg))
    apex = Vector((c[0] + lean * math.cos(lean_az), c[1] + lean * math.sin(lean_az), c[2] + h))
    ctr = Vector((c[0], c[1], c[2] - 5.0))
    for i in range(n):
        b.tri(base[i], base[(i + 1) % n], apex, ctr, False)


def build():
    rng = random.Random(SEED)
    b = Builder()
    # chunky base block: fills the cell volume, tops out ~z 82
    plate(b, rng, (0, 0, 62), 31, 40, 8, 0.0, 3, bottom_shrink=0.92)
    # tilted broken plates, varied heights and tilts
    P = [  # centre, radius, thickness, sides, tilt azimuth deg, tilt deg
        ((-11, 9, 90), 21, 14, 6, 40, 17),
        ((13, -12, 94), 19, 13, 5, 210, 22),
        ((12, 15, 88), 16, 12, 5, 100, 27),
        ((-18, -14, 87), 16, 12, 5, 300, 15),
        ((1, -1, 98), 14, 10, 5, 160, 11),
        ((-14, -27, 84), 10, 9, 4, 240, 22),
        ((26, -14, 85), 10, 9, 4, 330, 25),
        ((15, 26, 86), 10, 9, 4, 50, 20),
        ((-24, 6, 79), 12, 11, 4, 180, 24),
        ((22, 2, 80), 12, 11, 4, 20, 30),
        ((-2, 24, 82), 12, 10, 4, 270, 20),
        ((2, -25, 81), 12, 10, 4, 90, 26),
    ]
    for c, r, t, n, az, d in P:
        plate(b, rng, c, r, t, n, math.radians(az), d, bottom_shrink=0.78)
    # rim of upright shards
    SH = [  # centre xy, base z, radius, height, sides, lean azimuth deg, lean deg
        (-29, -27, 74, 8, 24, 4, 60, 12), (-3, -31, 72, 8, 22, 4, 250, 10), (27, -28, 75, 8, 24, 4, 120, 14),
        (31, 3, 76, 7, 20, 4, 10, 10), (28, 28, 74, 8, 24, 4, 200, 12), (3, 31, 75, 7, 20, 4, 300, 10),
        (-28, 29, 73, 8, 22, 4, 330, 12), (-31, -4, 74, 7, 20, 4, 80, 10),
        (-9, -12, 100, 7, 11, 4, 20, 8), (15, 6, 96, 6, 12, 3, 200, 8),
    ]
    global lean_az
    for x, y, z, r, h, n, laz, ld in SH:
        lean_az = math.radians(laz)
        shard(b, rng, (x, y, z), r, h, n, lean_az, ld)
    lim = H + MAX_OUT - 0.5   # safety clamp: no vertex beyond 4.5 uu outside the footprint
    for v in b.bm.verts:
        v.co.x = max(-lim, min(lim, v.co.x)); v.co.y = max(-lim, min(lim, v.co.y))
    for l in (l for f in b.bm.faces for l in f.loops):
        l[b.uv0].uv = (l.vert.co.x / 100.0, l.vert.co.y / 100.0)
    return b


def finish(b):
    bm = b.bm
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
    return N


def to_object(b, name):
    me = bpy.data.meshes.new(name)
    b.bm.to_mesh(me)
    me.update()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    me.materials.append(bpy.data.materials.new("M_RockSlab"))
    return ob


def main():
    os.makedirs(OUTDIR, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    b = build()
    finish(b)
    ob = to_object(b, NAME)
    bpy.ops.object.select_all(action="DESELECT"); ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    path = os.path.join(OUTDIR, NAME + ".fbx")
    bpy.ops.export_scene.fbx(filepath=path, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                             mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
    me = ob.data
    tris = len(me.polygons)
    xs = [v.co.x for v in me.vertices]; ys = [v.co.y for v in me.vertices]; zs = [v.co.z for v in me.vertices]
    over = max(max(xs) - H, -H - min(xs), max(ys) - H, -H - min(ys), 0.0)
    ca = me.color_attributes["Color"].data
    bad = r1 = g1 = 0
    for p in me.polygons:
        cs = {tuple(round(c, 3) for c in ca[li].color[:3]) for li in p.loop_indices}
        if len(cs) != 1: bad += 1; continue
        r, g, _ = next(iter(cs))
        if r == 1 and g == 0:
            r1 += 1
            if p.normal.z < 0.3: bad += 1
        elif g == 1 and r == 0: g1 += 1
        else: bad += 1
    uv1 = me.uv_layers["LightMap"].data
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
    uv_ok = overlaps == 0 and bmin * 256 >= 2 - 1e-6 and (1 - bmax) * 256 >= 2 - 1e-6 and mingap * 256 >= 2 - 1e-6
    uv0 = me.uv_layers["UVMap"].data
    uv0_ok = all(abs(uv0[l.index].uv[0] - me.vertices[l.vertex_index].co.x / 100) < 1e-5 and abs(uv0[l.index].uv[1] - me.vertices[l.vertex_index].co.y / 100) < 1e-5 for l in me.loops)
    warnings = []
    if not TRI_MIN <= tris <= TRI_MAX: warnings.append("tris outside %d-%d" % (TRI_MIN, TRI_MAX))
    if over > MAX_OUT: warnings.append("overshoot %.2f > %.1f" % (over, MAX_OUT))
    if not uv_ok: warnings.append("LightMap check failed")
    if bad: warnings.append("vertex colour inconsistency")
    report = {"task": "task-230-06-rock-plate-overlay", "script": "Tools/art/blender/rock_plates.py", "seed": SEED, "units": "cm (1 uu = 1 cm)",
              "file": "Art/Meshes/rock-slab/%s.fbx" % NAME, "tris": tris, "budget": [TRI_MIN, TRI_MAX], "within_budget": TRI_MIN <= tris <= TRI_MAX,
              "pivot": "cell centre, z = 0 floor top", "footprint_uu": [67, 67], "max_overshoot_allowed_uu": MAX_OUT, "overshoot_uu": round(over, 2),
              "bounds_min": [round(min(xs), 2), round(min(ys), 2), round(min(zs), 2)], "bounds_max": [round(max(xs), 2), round(max(ys), 2), round(max(zs), 2)],
              "dimensions_cm": [round(max(xs) - min(xs), 2), round(max(ys) - min(ys), 2), round(max(zs) - min(zs), 2)],
              "materials": ["M_RockSlab"], "uv_layers": [u.name for u in me.uv_layers],
              "uv_checks": {"UVMap_planar_top_down_xy_over_100": bool(uv0_ok),
                            "LightMap": {"triangles": tris, "overlapping_pairs": overlaps, "bounds": [round(bmin, 4), round(bmax, 4)], "min_gap_px_at_256": round(mingap * 256, 3), "pass": bool(uv_ok)}},
              "vertex_colour": {"attribute": "Color (corner, float)", "export": "colors_type=LINEAR", "top_tris_R1": r1, "side_tris_G1": g1, "inconsistent_faces": bad, "pass": bad == 0,
                                "rule": "R=1 on plate top faces (normal.z >= 0.3), G=1 on broken sides and upright shards"},
              "all_flat_shaded": all(not p.use_smooth for p in me.polygons),
              "fbx_export": {"apply_scale_options": "FBX_SCALE_UNITS", "global_scale": 1.0, "axes": "Blender default", "colors_type": "LINEAR", "mesh_smooth_type": "FACE"},
              "warnings": warnings}
    if PREVIEW:
        report["preview"] = previews(b, ob)
    with open(os.path.join(OUTDIR, "SM_RockPlates.report.json"), "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print(json.dumps({k: report[k] for k in ("tris", "bounds_min", "bounds_max", "overshoot_uu", "warnings")}))


# ---------------------------------------------------------------- previews (same camera and light as rock_slab_pieces.py)
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


def append_geo(bm, col, src_bm_or_mesh, xf, skip=None):
    """copy faces of a mesh object (with 'Color' attribute) into bm through transform xf (callable on Vector)."""
    me = src_bm_or_mesh
    ca = me.color_attributes[0].data
    for p in me.polygons:
        pts = [xf(me.vertices[i].co.copy()) for i in p.vertices]
        if skip and skip(pts):
            continue
        try:
            nf = bm.faces.new([bm.verts.new(v) for v in pts])
        except ValueError:
            continue
        nf.smooth = False
        for nl, li in zip(nf.loops, p.loop_indices):
            nl[col] = ca[li].color


def previews(b, ob):
    os.makedirs(PREV_DIR, exist_ok=True)
    mat = make_material()
    floor_m = bpy.data.materials.new("FLOOR"); floor_m.use_nodes = True
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = srgb("#B9B9BE")
    floor_m.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 1.0
    fl = bpy.data.meshes.new("Floor"); fl.from_pydata([(-3000, -3000, 0), (4000, -3000, 0), (4000, 4000, 0), (-3000, 4000, 0)], [], [(0, 1, 2, 3)])
    flo = bpy.data.objects.new("Floor", fl); bpy.context.scene.collection.objects.link(flo); fl.materials.append(floor_m)
    ob.data.materials.clear(); ob.data.materials.append(mat)
    sc = setup_render(1000, 1000)
    out = {}
    p = os.path.join(PREV_DIR, "rockplates_three_quarter.png")
    render_one(sc, p, {"hfov": 40.0, "pitch": 35.0, "azim": 32.0, "target": (0, 0, 40.0), "dist": 330.0}); out["three_quarter"] = p
    p = os.path.join(PREV_DIR, "rockplates_game_cam.png")
    render_one(sc, p, {"hfov": 40.0, "pitch": 60.0, "azim": 0.0, "target": (0, 0, 40.0), "dist": 330.0}); out["game_cam"] = p
    ob.hide_render = True

    # layout: slab pieces of rock_slab_pieces.py (re-imported from their FBX), 3 cells replaced by the cluster
    rep = json.load(open(os.path.join(OUTDIR, "SM_RockSlab.report.json"), encoding="utf-8"))
    placed = rep["layout"]["placed"]
    cells = [((1, 1), 0), ((5, 1), 90), ((1, 5), 180)]   # (col,row), cluster rotation deg
    boxes = [(((c + 0.5) * 67.0), ((r + 0.5) * 67.0)) for (c, r), _ in cells]
    def inside(pts):
        cx = sum(p.x for p in pts) / len(pts); cy = sum(p.y for p in pts) / len(pts)
        return any(abs(cx - bx) < H and abs(cy - by) < H for bx, by in boxes)
    bm = bmesh.new(); col = bm.loops.layers.float_color.new("Color")
    imported = {}
    for shape in ("full", "edge", "outer", "inner", "diagonal"):
        before = set(bpy.data.objects)
        bpy.ops.import_scene.fbx(filepath=os.path.join(OUTDIR, "SM_RockSlab_%s_01.fbx" % shape.capitalize()))
        o = [x for x in bpy.data.objects if x not in before and x.type == "MESH"][0]
        imported[shape] = (o.data, o.matrix_world.copy())
    for key, (shape, deg) in placed.items():
        i, j = [int(x) for x in key.split(",")]
        me, mw = imported[shape]
        R = Matrix.Rotation(math.radians(deg), 3, "Z")
        append_geo(bm, col, me, lambda v: (R @ (mw @ v)) + Vector((i * 67.0, j * 67.0, 0)))
    for bx, by in boxes:   # cut the slab exactly at the cell square, then drop what lies inside
        for pt, no in (((bx - H, 0, 0), (1, 0, 0)), ((bx + H, 0, 0), (1, 0, 0)), ((0, by - H, 0), (0, 1, 0)), ((0, by + H, 0), (0, 1, 0))):
            g = list(bm.verts) + list(bm.edges) + list(bm.faces)
            bmesh.ops.bisect_plane(bm, geom=g, plane_co=pt, plane_no=no, dist=1e-4)
    gone = [f_ for f_ in bm.faces if any(abs(f_.calc_center_median().x - bx) < H and abs(f_.calc_center_median().y - by) < H for bx, by in boxes)]
    bmesh.ops.delete(bm, geom=gone, context="FACES")
    for (c, r), deg in cells:
        R = Matrix.Rotation(math.radians(deg), 3, "Z")
        append_geo(bm, col, ob.data, lambda v: (R @ v) + Vector(((c + 0.5) * 67.0, (r + 0.5) * 67.0, 0)))
    me = bpy.data.meshes.new("Layout"); bm.to_mesh(me); bm.free()
    lay = bpy.data.objects.new("Layout", me); bpy.context.scene.collection.objects.link(lay); me.materials.append(mat)
    for o in list(bpy.data.objects):
        if o.name not in (lay.name, flo.name, ob.name) and o.type == "MESH":
            o.hide_render = True
    sc = setup_render(1920, 1080)
    cen = (268.0, 268.0, 0.0)
    p = os.path.join(PREV_DIR, "rockplates_layout_game_cam.png")
    render_one(sc, p, {"hfov": 90.0, "pitch": 60.0, "azim": 0.0, "target": cen, "dist": 640.0}); out["layout_game_cam"] = p
    p = os.path.join(PREV_DIR, "rockplates_layout_three_quarter.png")
    render_one(sc, p, {"hfov": 50.0, "pitch": 38.0, "azim": 28.0, "target": cen, "dist": 1050.0}); out["layout_three_quarter"] = p
    out["layout_note"] = "cluster cells (col,row,rot deg) %s; slab triangles whose centroid lies in those cells are removed (the material sinks the quadrants in the engine)" % cells
    out["camera"] = "same as rockslab previews: layout hfov 90 pitch 60 azim 0 dist 640 / hfov 50 pitch 38 azim 28 dist 1050; cluster alone hfov 40, target (0,0,40), dist 330, pitch 35 azim 32 or pitch 60 azim 0"
    return out


main()
