"""Rogue Rigs 3D agent: heal pickup chunk meshes, Blender route (task-339-01).

  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/heal_chunk.py -- --outdir Art/Meshes/HealChunk

Builds SM_HealChunk_01, _02, _03 from the sheet vault/assets/heal-pickup-sheet.png (left, middle, right piece):
two whole six-sided prisms with pointed tips side by side on a wide fracture body (core + ring of angular chips,
flat resting face), plus a small third tip on 01 (front centre) and 03 (front left), none on 02.
Units: 1 Blender unit = 1 cm. Largest side scaled to 30 cm, pivot at the bounding-box centre.
Slots (same order on the three): crystal_bright (prisms and small tip), crystal_dark (fracture body).
Flat shading, no texture, UV0 `UVMap`, UV1 `LightMap` (smart project, packed). FBX_SCALE_UNITS.
Writes <outdir>/SM_HealChunk_0N.fbx + .report.json and the preview renders (3-up, side, underside).
Optional: --fidelity <json> merges a `fidelity` object into each report (written by the agent after the loop).
"""
import json, math, os, random, sys
import bpy, bmesh
import numpy as np
from mathutils import Vector, Matrix, Euler

SIZE_CM = 30.0
BRIGHT, DARK = 0, 1
COLS = {"crystal_bright": (1.0, 0.0, 0.0, 1.0), "crystal_dark": (0.5, 0.0, 0.0, 1.0)}   # pure red placeholders; Unreal MIs set the final values


def args():
    a = sys.argv[sys.argv.index("--") + 1:]
    o = {}
    for i in range(0, len(a), 2):
        o[a[i][2:]] = a[i + 1]
    return o


def tris(m):
    return sum(len(p.vertices) - 2 for p in m.polygons)


def add_hull(bm, pts, mat):
    """Convex hull of pts added to bm, coplanar faces dissolved, material index set."""
    vs = [bm.verts.new(Vector(p)) for p in pts]
    res = bmesh.ops.convex_hull(bm, input=vs)
    bmesh.ops.delete(bm, geom=res["geom_interior"] + res["geom_unused"], context="VERTS")
    geom = [g for g in res["geom"] if g.is_valid]
    for f in [g for g in geom if isinstance(g, bmesh.types.BMFace)]:
        f.material_index = mat
    verts = [g for g in geom if isinstance(g, bmesh.types.BMVert)]
    edges = [g for g in geom if isinstance(g, bmesh.types.BMEdge)]
    bmesh.ops.dissolve_limit(bm, angle_limit=math.radians(1.5), verts=verts, edges=edges)


def prism(cx, cy, base_z, top_z, r, lean_x, lean_y, twist):
    """Six-sided prism with a pointed tip: base ring, shoulder ring, apex; leaned about its base centre."""
    pts = []
    h = top_z - base_z
    for k in range(6):
        a = math.radians(twist + 60 * k)
        pts.append((r * math.cos(a), r * math.sin(a), 0.0))
    for k in range(6):
        a = math.radians(twist + 8 + 60 * k)
        rr = r * (0.97 if k % 2 else 1.03)
        pts.append((rr * math.cos(a), rr * math.sin(a), 0.60 * h))
    pts.append((0.10 * r, -0.08 * r, h))
    rot = Matrix.Rotation(math.radians(lean_x), 3, "Y") @ Matrix.Rotation(math.radians(lean_y), 3, "X")
    out = []
    for p in pts:
        q = rot @ Vector(p)
        out.append((cx + q.x, cy + q.y, base_z + q.z))
    return out


def body_points(mirror):
    """Core + ring of chips; z clamped to 0 so the whole underside is one flat resting face."""
    rnd = random.Random(339)
    core = []
    for k in range(9):
        a = math.radians(40 * k + rnd.uniform(-8, 8))
        s = rnd.uniform(0.9, 1.05)
        core.append((9.5 * s * math.cos(a), 8.8 * s * math.sin(a), 0.0))
        core.append((7.4 * s * math.cos(a + 0.2), 6.8 * s * math.sin(a + 0.2), rnd.uniform(15.0, 17.0)))
    hulls = [core]
    n = 8
    for i in range(n):
        a = math.radians(360.0 / n * i + 15 + rnd.uniform(-10, 10))
        cx, cy = 9.6 * math.cos(a), 8.8 * math.sin(a)
        rx, ry = rnd.uniform(3.4, 4.4), rnd.uniform(3.4, 4.4)
        top = rnd.uniform(11.0, 15.0)
        if math.sin(a) < -0.5:
            top = rnd.uniform(10.0, 12.5)         # front chips lower: the small tip stands on the core
        pts = []
        rot = rnd.uniform(0, math.pi)
        for sx in (-1, 1):
            for sy in (-1, 1):
                for up in (0, 1):
                    k = 0.72 if up else 1.0          # tapered top: sloped fracture faces, not boxes
                    px, py = sx * rx * k * rnd.uniform(0.6, 1.25), sy * ry * k * rnd.uniform(0.6, 1.25)
                    x = cx + px * math.cos(rot) - py * math.sin(rot)
                    y = cy + px * math.sin(rot) + py * math.cos(rot)
                    z = 0.0 if not up else top * rnd.uniform(0.55, 1.0)
                    pts.append((x, y, z))
        hulls.append(pts)
    if mirror:
        hulls = [[(-x, y, z) for x, y, z in h] for h in hulls]
    return hulls


VARIANTS = [
    # prism A (left) and B (right): top z before scaling, lean x / lean y degrees, twist; tip (x, y, lean) or None
    {"name": "SM_HealChunk_01", "mirror": False,
     "A": (-4.4, 30.0, -6, 3, 0), "B": (4.4, 25.5, 8, -4, 12), "tip": (0.0, -7.6, 14, 14.5)},
    {"name": "SM_HealChunk_02", "mirror": True,
     "A": (-4.4, 30.0, -10, 4, 6), "B": (4.6, 24.0, 12, -6, 18), "tip": None},
    {"name": "SM_HealChunk_03", "mirror": False,
     "A": (-4.4, 30.0, -4, 2, 20), "B": (4.4, 28.5, 14, -3, 0), "tip": (-7.4, -6.4, 16, 11.5)},
]


def build(v):
    bm = bmesh.new()
    for pts in body_points(v["mirror"]):
        add_hull(bm, pts, DARK)
    meta = {}
    for key in ("A", "B"):
        x, top, lx, ly, tw = v[key]
        add_hull(bm, prism(x, 0.0, 12.0, top, 4.9, lx, ly, tw), BRIGHT)
        meta[key + "_top_z"] = top
    if v["tip"]:
        tx, ty, lean, tz = v["tip"]
        pts = []
        for k in range(5):
            a = math.radians(72 * k + 10)
            pts.append((2.4 * math.cos(a), 2.4 * math.sin(a), 0.0))
        pts.append((0.0, 0.0, 7.5))
        rot = Matrix.Rotation(math.radians(lean), 3, "X") @ Matrix.Rotation(math.radians(-lean * 0.3), 3, "Y")
        pts = [tuple(Vector((tx, ty, tz)) + rot @ Vector(p)) for p in pts]
        add_hull(bm, pts, BRIGHT)
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    # scale: largest dimension = SIZE_CM, pivot at the bounding-box centre
    co = np.array([vt.co[:] for vt in bm.verts])
    lo, hi = co.min(0), co.max(0)
    s = SIZE_CM / float((hi - lo).max())
    ctr = (lo + hi) / 2
    for vt in bm.verts:
        vt.co = (Vector(vt.co) - Vector(ctr)) * s
    meta["scale"] = s
    meta["centre_pre"] = [float(c) for c in ctr]
    mesh = bpy.data.meshes.new(v["name"])
    bm.to_mesh(mesh)
    bm.free()
    for p in mesh.polygons:
        p.use_smooth = False
    for name in ("crystal_bright", "crystal_dark"):
        if name in bpy.data.materials:           # one shared material per slot: names stay exact on the three FBX
            mesh.materials.append(bpy.data.materials[name])
            continue
        m = bpy.data.materials.new(name)
        m.diffuse_color = COLS[name]
        m.use_nodes = True
        b = m.node_tree.nodes["Principled BSDF"]
        b.inputs["Base Color"].default_value = COLS[name]
        b.inputs["Roughness"].default_value = 0.8
        for k in ("Specular IOR Level", "Specular"):
            if k in b.inputs:
                b.inputs[k].default_value = 0.0
        mesh.materials.append(m)
    obj = bpy.data.objects.new(v["name"], mesh)
    bpy.context.scene.collection.objects.link(obj)
    return obj, meta


def uvs(obj):
    mesh = obj.data
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    for name in ("UVMap", "LightMap"):
        layer = mesh.uv_layers.new(name=name)
        mesh.uv_layers.active = layer
        bpy.ops.object.mode_set(mode="EDIT")
        bpy.ops.mesh.select_all(action="SELECT")
        bpy.ops.uv.smart_project(angle_limit=1.15, island_margin=0.01)
        bpy.ops.uv.select_all(action="SELECT")
        bpy.ops.uv.pack_islands(margin=0.01)
        bpy.ops.object.mode_set(mode="OBJECT")
    mesh.uv_layers.active_index = 0


def hull2d(pts):
    pts = sorted(set(pts))
    def cross(o, a, b):
        return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])
    lo, up = [], []
    for p in pts:
        while len(lo) >= 2 and cross(lo[-2], lo[-1], p) <= 0:
            lo.pop()
        lo.append(p)
    for p in reversed(pts):
        while len(up) >= 2 and cross(up[-2], up[-1], p) <= 0:
            up.pop()
        up.append(p)
    return lo[:-1] + up[:-1]


def poly_area(h):
    return abs(sum(h[i][0] * h[(i + 1) % len(h)][1] - h[(i + 1) % len(h)][0] * h[i][1] for i in range(len(h)))) / 2


def metrics(obj, meta):
    mesh = obj.data
    co = np.array([v.co[:] for v in mesh.vertices])
    lo, hi = co.min(0), co.max(0)
    dims = hi - lo
    rep = {}
    rep["dimensions_cm"] = [round(float(x), 2) for x in dims]
    rep["bbox_centre_cm"] = [round(float(x), 3) for x in (hi + lo) / 2]
    rep["height_over_width"] = round(float(dims[2] / max(dims[0], dims[1])), 3)
    # flat resting face: union of downward faces at the minimum z, sampled on a 0.2 cm grid
    zmin = lo[2]
    polys = []
    for p in mesh.polygons:
        if p.normal.z < -0.99 and all(abs(mesh.vertices[i].co.z - zmin) < 0.01 for i in p.vertices):
            q = np.array([mesh.vertices[i].co[:2] for i in p.vertices]); a2 = sum(q[i][0]*q[(i+1)%len(q)][1]-q[(i+1)%len(q)][0]*q[i][1] for i in range(len(q)))
            polys.append(q if a2 > 0 else q[::-1])
    gx, gy = np.meshgrid(np.arange(lo[0], hi[0], 0.2), np.arange(lo[1], hi[1], 0.2))
    P = np.stack([gx.ravel(), gy.ravel()], 1)
    inside = np.zeros(len(P), bool)
    for poly in polys:
        sgn = None
        ok = np.ones(len(P), bool)
        for i in range(len(poly)):
            a, b = poly[i], poly[(i + 1) % len(poly)]
            c = (b[0] - a[0]) * (P[:, 1] - a[1]) - (b[1] - a[1]) * (P[:, 0] - a[0])
            ok &= c >= -1e-9
        inside |= ok
    flat_area = float(inside.sum() * 0.04)
    h2 = hull2d([(float(x), float(y)) for x, y, _ in co])
    rep["resting_face"] = {"faces": len(polys), "area_cm2": round(flat_area, 1),
                           "footprint_hull_cm2": round(poly_area(h2), 1),
                           "share_of_footprint": round(flat_area / poly_area(h2), 3)}
    s = meta["scale"]
    ctr_z = meta["centre_pre"][2]
    ha = (meta["A_top_z"] - ctr_z) * s - lo[2]
    hb = (meta["B_top_z"] - ctr_z) * s - lo[2]
    rep["prism_top_height_cm"] = {"A": round(ha, 1), "B": round(hb, 1)}
    rep["prism_B_over_A"] = round(min(ha, hb) / max(ha, hb), 3)
    return rep


def uv_checks(mesh):
    out = {}
    for name in ("UVMap", "LightMap"):
        u = np.array([d.uv[:] for d in mesh.uv_layers[name].data])
        out[name] = {"min": round(float(u.min()), 4), "max": round(float(u.max()), 4)}
    tri_area = 0.0
    lay = mesh.uv_layers["LightMap"].data
    for p in mesh.polygons:
        pts = [lay[i].uv for i in p.loop_indices]
        for k in range(1, len(pts) - 1):
            a, b, c = pts[0], pts[k], pts[k + 1]
            tri_area += abs((b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1])) / 2
    out["LightMap"]["uv_area_sum"] = round(tri_area, 4)
    out["LightMap"]["inside_0_1"] = out["LightMap"]["min"] >= 0 and out["LightMap"]["max"] <= 1
    out["LightMap"]["no_overlap"] = tri_area <= 1.0
    return out


def render(objs, out_png, az, el, res=(1800, 800), ortho=135.0):
    """Orthographic render, sun and world of blender-asset-route, light-grey background composited after."""
    sc = bpy.context.scene
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            continue
    sc.render.resolution_x, sc.render.resolution_y = res
    sc.render.resolution_percentage = 100
    sc.render.film_transparent = True
    sc.render.image_settings.file_format = "PNG"
    sc.render.image_settings.color_mode = "RGBA"
    sc.render.filepath = out_png
    sc.view_settings.view_transform = "Standard"
    sc.view_settings.look = "None"
    if sc.world is None:
        w = bpy.data.worlds.new("W")
        w.use_nodes = True
        sc.world = w
    bg = sc.world.node_tree.nodes["Background"]
    bg.inputs["Color"].default_value = (1, 1, 1, 1)
    bg.inputs["Strength"].default_value = 0.35
    for o in [o for o in sc.objects if o.type in ("CAMERA", "LIGHT")]:
        bpy.data.objects.remove(o, do_unlink=True)
    target = Vector((0, 0, 0))
    az, el = math.radians(az), math.radians(el)
    cd = bpy.data.cameras.new("Cam")
    cd.type = "ORTHO"
    cd.ortho_scale = ortho
    cd.clip_start, cd.clip_end = 1, 2000
    cam = bpy.data.objects.new("Cam", cd)
    sc.collection.objects.link(cam)
    cam.location = target + Vector((math.sin(az) * math.cos(el), -math.cos(az) * math.cos(el), math.sin(el))) * 400
    cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
    sc.camera = cam
    sd = bpy.data.lights.new("Sun", "SUN")
    sd.energy = 4.0
    sun = bpy.data.objects.new("Sun", sd)
    sc.collection.objects.link(sun)
    sun.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)], "XYZ")
    bpy.ops.render.render(write_still=True)
    img = bpy.data.images.load(out_png)
    w, h = img.size
    px = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, 4)
    grey = np.array([0.6, 0.6, 0.6], np.float32)
    # background A16 light grey (sRGB 0.8 -> linear 0.604 stored through Standard view: use the display value)
    rgb = px[..., :3] * px[..., 3:4] + grey * (1 - px[..., 3:4])
    px[..., :3] = rgb
    px[..., 3] = 1.0
    img.pixels[:] = px.ravel()
    img.file_format = "PNG"
    img.filepath_raw = out_png
    img.save()
    bpy.data.images.remove(img)


def main():
    o = args()
    outdir = os.path.abspath(o["outdir"])
    os.makedirs(outdir, exist_ok=True)
    fid = None
    if "fidelity" in o and os.path.exists(o["fidelity"]):
        fid = json.load(open(o["fidelity"], encoding="utf-8"))
    bpy.ops.wm.read_factory_settings(use_empty=True)
    built = []
    for v in VARIANTS:
        obj, meta = build(v)
        uvs(obj)
        rep = {"output": os.path.join(outdir, v["name"] + ".fbx"), "warnings": []}
        rep["tris"] = tris(obj.data)
        rep["verts"] = len(obj.data.vertices)
        rep.update(metrics(obj, meta))
        rep["pivot"] = "bounding-box centre"
        rep["materials"] = {}
        for i, m in enumerate(obj.data.materials):
            rep["materials"][m.name] = {"slot": i, "tris": sum(len(p.vertices) - 2 for p in obj.data.polygons if p.material_index == i)}
        rep["uv_layers"] = [u.name for u in obj.data.uv_layers]
        rep["uv_checks"] = uv_checks(obj.data)
        rep["shading"] = "flat"
        rep["texture"] = "none"
        if rep["tris"] > 500:
            rep["warnings"].append("tris above 500")
        if abs(max(rep["dimensions_cm"]) - SIZE_CM) > 1:
            rep["warnings"].append("longest side off 30 cm")
        if not rep["uv_checks"]["LightMap"]["inside_0_1"]:
            rep["warnings"].append("LightMap UV outside 0..1")
        if rep["resting_face"]["share_of_footprint"] < 0.4:
            rep["warnings"].append("resting face below 40 percent of the footprint")
        bpy.ops.object.select_all(action="DESELECT")
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj
        bpy.ops.export_scene.fbx(filepath=rep["output"], use_selection=True, object_types={"MESH"},
                                 apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                                 add_leaf_bones=False, path_mode="STRIP", embed_textures=False)
        built.append((obj, rep, v["name"]))
    # renders: sheet order, same scale, 48 cm spacing
    for i, (obj, rep, name) in enumerate(built):
        obj.location = (math.cos(math.radians(-35)) * (i - 1) * 48.0, math.sin(math.radians(-35)) * (i - 1) * 48.0, 0)
    bpy.context.view_layer.update()
    render([b[0] for b in built], os.path.join(outdir, "heal_chunk_preview.png"), -35, 35)
    render([b[0] for b in built], os.path.join(outdir, "heal_chunk_side.png"), -35, 4)
    render([b[0] for b in built], os.path.join(outdir, "heal_chunk_under.png"), -35, -55)
    render([b[0] for b in built], os.path.join(outdir, "heal_chunk_front.png"), -35, 20, ortho=135.0)
    for obj, rep, name in built:
        if fid:
            rep["fidelity"] = fid
        with open(os.path.join(outdir, name + ".report.json"), "w", encoding="utf-8") as f:
            json.dump(rep, f, indent=2)
        print(json.dumps({k: rep[k] for k in ("tris", "dimensions_cm", "height_over_width", "resting_face", "prism_top_height_cm", "prism_B_over_A", "warnings")}))


main()
