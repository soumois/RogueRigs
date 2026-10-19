"""Duskite deposit crystal formations (spire, ridge, crown), Blender route, task-234-02.
Faceted hexagonal prisms with pointed tips, crystals only (no stone pad), base sunk 5 cm under z 0,
pivot at the footprint centre, one material slot `crystal`, flat shading. 1 Blender unit = 1 cm.
Run: blender -b --factory-startup -P ore_deposit.py -- --outdir Art/Meshes/ore-deposit"""
import bpy, bmesh, math, json, os, sys, random
from mathutils import Matrix, Vector

SINK = 5.0
FOOT = 134.0
# (base_x, base_y, height_above_z0, radius, lean_deg, lean_dir_deg(0=+x, 90=+y))
SPIRE = [
    (0, 5, 110, 18, 2, 90),
    (-38, 8, 72, 13, 15, 180),
    (36, 22, 74, 12, 14, 20),
    (24, -16, 64, 14, 24, -30),
    (-12, -32, 45, 11, 10, -100),
    (6, -46, 32, 9, 8, -80),
    (-44, -20, 26, 6, 30, -150),
    (48, -8, 24, 6, 30, 0),
]
RIDGE = [
    (-10, 2, 116, 16, 24, 0),
    (-38, -4, 70, 13, 8, 180),
    (8, -14, 78, 13, 28, 0),
    (28, -20, 42, 8, 38, 0),
    (-24, -30, 40, 11, 12, -20),
    (4, 20, 60, 11, 25, 0),
]
def crown():
    c = [(0, 8, 110, 16, 4, 90), (0, -20, 58, 13, 6, -90)]
    for i, h in enumerate([78, 64, 92, 70, 84, 60]):
        a = math.radians(30 + 60 * i)
        c.append((26 * math.cos(a), 26 * math.sin(a), h, 12, 20, math.degrees(a)))
    return c
VARIANTS = {"01": ("spire", SPIRE), "02": ("ridge", RIDGE), "03": ("crown", crown())}

def crystal_geo(rng, r, L):
    rot = rng.uniform(0, math.pi / 3)
    def ring(rad, z):
        pts = []
        for k in range(6):
            a = rot + k * math.pi / 3
            j = rad * rng.uniform(0.9, 1.1)
            pts.append((j * math.cos(a), j * math.sin(a), z))
        return pts
    v = ring(r, 0) + ring(r * 0.86, L * 0.78)
    v.append((rng.uniform(-.15, .15) * r, rng.uniform(-.15, .15) * r, L))
    v.append((0, 0, 0))
    f = []
    for k in range(6):
        n = (k + 1) % 6
        f.append((k, n, 6 + n, 6 + k))
        f.append((6 + k, 6 + n, 12))
        f.append((n, k, 13))
    return v, f

def build(name, spec, seed):
    rng = random.Random(seed)
    verts, faces = [], []
    for (bx, by, h, r, lean, ld) in spec:
        v, f = crystal_geo(rng, r, h + SINK)
        m = Matrix.Rotation(math.radians(ld), 4, "Z") @ Matrix.Rotation(math.radians(lean), 4, "Y")
        o = len(verts)
        pts = [m @ Vector(p) for p in v]
        lift = -SINK - min(pts[i].z for i in (0, 1, 2, 3, 4, 5, 13))  # lowest base point sits at -SINK
        verts += [tuple(Vector((bx, by, lift)) + q) for q in pts]
        faces += [tuple(i + o for i in fc) for fc in f]
    me = bpy.data.meshes.new(name)
    me.from_pydata(verts, [], faces)
    me.update()
    bm = bmesh.new(); bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    bm.to_mesh(me); bm.free()
    for p in me.polygons: p.use_smooth = False
    uv = me.uv_layers.new(name="UVMap")
    for l in me.loops:
        x, y, _ = me.vertices[l.vertex_index].co
        uv.data[l.index].uv = (x / FOOT + 0.5, y / FOOT + 0.5)
    mat = bpy.data.materials.get("crystal") or bpy.data.materials.new("crystal")
    mat.diffuse_color = (0.42, 0.12, 0.85, 1)
    me.materials.append(mat)
    ob = bpy.data.objects.new(name, me)
    bpy.context.collection.objects.link(ob)
    return ob

def main():
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    outdir = argv[argv.index("--outdir") + 1] if "--outdir" in argv else "Art/Meshes/ore-deposit"
    os.makedirs(outdir, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    for key, (label, spec) in VARIANTS.items():
        name = "SM_DuskiteDeposit_" + key
        ob = build(name, spec, int(key) * 7919)
        bpy.context.view_layer.objects.active = ob
        ob.select_set(True)
        bb = [ob.matrix_world @ Vector(c) for c in ob.bound_box]
        mn = [min(c[i] for c in bb) for i in range(3)]; mx = [max(c[i] for c in bb) for i in range(3)]
        dims = [mx[i] - mn[i] for i in range(3)]
        rep = {"mesh": name, "variant": label, "crystals": len(spec), "tris": len(ob.data.polygons),
               "bounds_min_cm": [round(x, 2) for x in mn], "bounds_max_cm": [round(x, 2) for x in mx],
               "dimensions_cm": [round(x, 2) for x in dims], "materials": [m.name for m in ob.data.materials],
               "uv_layers": [u.name for u in ob.data.uv_layers], "warnings": []}
        if max(abs(mn[0]), abs(mx[0]), abs(mn[1]), abs(mx[1])) > FOOT / 2:
            rep["warnings"].append("outside 134 footprint")
        rep["inside_footprint"] = not rep["warnings"]
        bpy.ops.export_scene.fbx(filepath=os.path.join(outdir, name + ".fbx"), use_selection=True, object_types={"MESH"},
            apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE", add_leaf_bones=False,
            path_mode="STRIP", embed_textures=False)
        with open(os.path.join(outdir, name + ".report.json"), "w") as fh: json.dump(rep, fh, indent=2)
        print(json.dumps(rep))
        ob.select_set(False)
main()
