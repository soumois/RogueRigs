"""Background asteroids (task-334-01), Blender route: 4 faceted low-poly rock variants.

Run: "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/asteroid.py -- --outdir Art/Meshes/Asteroid
Outputs SM_Asteroid_01..04.fbx (+ .report.json each) and preview.png (4 variants side by side).
Deterministic: seeds SEEDS, no other randomness. 1 Blender unit = 1 m, FBX_SCALE_UNITS -> centimetres.
"""
import bpy, bmesh, sys, os, json, math, random
from mathutils import Vector, Euler

SEEDS = [3401, 3402, 3403, 3404]
MAX_TRIS = 400
SIZE_M = 1.0              # largest bbox dimension, 100 cm before scaling
SUBDIV = 3                # icosphere: 320 triangles
COLOUR = (0.16, 0.135, 0.21, 1.0)   # flat violet-grey (linear), darker than the arena rocks

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUTDIR = os.path.abspath(argv[argv.index("--outdir") + 1] if "--outdir" in argv else "Art/Meshes/Asteroid")
os.makedirs(OUTDIR, exist_ok=True)


def make_material():
    m = bpy.data.materials.new("M_Asteroid")
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = COLOUR
    b.inputs["Roughness"].default_value = 0.9
    m.diffuse_color = COLOUR
    return m


def build(seed, idx, mat):
    rnd = random.Random(seed)
    bm = bmesh.new()
    bmesh.ops.create_icosphere(bm, subdivisions=SUBDIV, radius=1.0)
    # low-frequency lobes (random directions) + per-vertex jitter -> irregular facets
    lobes = [(Vector((rnd.uniform(-1, 1), rnd.uniform(-1, 1), rnd.uniform(-1, 1))).normalized(),
              rnd.uniform(0.10, 0.28), rnd.uniform(-1, 1)) for _ in range(5)]
    stretch = Vector((rnd.uniform(0.8, 1.15), rnd.uniform(0.7, 1.0), rnd.uniform(0.55, 0.9)))
    for v in bm.verts:
        d = v.co.normalized()
        r = 1.0 + sum(a * math.copysign(1, s) * max(0.0, d.dot(ax)) ** 2 for ax, a, s in lobes)
        r += rnd.uniform(-0.12, 0.12)
        v.co = Vector((d.x * r * stretch.x, d.y * r * stretch.y, d.z * r * stretch.z))
    me = bpy.data.meshes.new("SM_Asteroid_%02d" % idx)
    bm.to_mesh(me); bm.free()
    obj = bpy.data.objects.new("SM_Asteroid_%02d" % idx, me)
    bpy.context.scene.collection.objects.link(obj)
    # bbox centre to origin, largest dimension = SIZE_M
    cs = [v.co for v in me.vertices]
    lo = Vector((min(c.x for c in cs), min(c.y for c in cs), min(c.z for c in cs)))
    hi = Vector((max(c.x for c in cs), max(c.y for c in cs), max(c.z for c in cs)))
    ctr = (lo + hi) / 2
    k = SIZE_M / max(hi.x - lo.x, hi.y - lo.y, hi.z - lo.z)
    for v in me.vertices:
        v.co = (v.co - ctr) * k
    me.update()
    for p in me.polygons:
        p.use_smooth = False
    me.materials.append(mat)
    # UV channel 0 only
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True)
    bpy.ops.object.mode_set(mode="EDIT"); bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=0.02)
    bpy.ops.object.mode_set(mode="OBJECT")
    return obj


def islands(me):
    adj = {v.index: set() for v in me.vertices}
    for e in me.edges:
        a, b = e.vertices; adj[a].add(b); adj[b].add(a)
    seen, n = set(), 0
    for s in adj:
        if s in seen: continue
        n += 1; st = [s]
        while st:
            x = st.pop()
            if x in seen: continue
            seen.add(x); st.extend(adj[x] - seen)
    return n


for _o in list(bpy.data.objects):
    bpy.data.objects.remove(_o, do_unlink=True)   # factory-startup cube, light, camera
mat = make_material()
objs, reports = [], []
for i, seed in enumerate(SEEDS, 1):
    o = build(seed, i, mat)
    me = o.data
    tris = sum(len(p.vertices) - 2 for p in me.polygons)
    bb = [o.matrix_world @ Vector(c) for c in o.bound_box]
    dims = [round((max(b[a] for b in bb) - min(b[a] for b in bb)) * 100, 2) for a in range(3)]
    ctr = [round(((max(b[a] for b in bb) + min(b[a] for b in bb)) / 2) * 100, 3) for a in range(3)]
    uvs = [l.name for l in me.uv_layers]
    warns = []
    if tris > MAX_TRIS: warns.append("tris %d above budget %d" % (tris, MAX_TRIS))
    isl = islands(me)
    if isl != 1: warns.append("islands = %d" % isl)
    if len(uvs) != 1: warns.append("UV layers = %s" % uvs)
    if not (90 <= max(dims) <= 110): warns.append("largest dimension %.1f cm off 100" % max(dims))
    fbx = os.path.join(OUTDIR, "SM_Asteroid_%02d.fbx" % i)
    bpy.ops.object.select_all(action="DESELECT"); o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.export_scene.fbx(filepath=fbx, use_selection=True, object_types={"MESH"},
                             apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                             add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
    rep = {"asset": "SM_Asteroid_%02d" % i, "seed": seed, "tris_out": tris, "max_tris": MAX_TRIS,
           "dimensions_cm": dims, "bbox_centre_cm": ctr, "pivot": "bbox centre", "islands": isl,
           "materials": [m.name for m in me.materials], "uv_layers": uvs, "shading": "flat",
           "units": "cm (FBX_SCALE_UNITS)", "warnings": warns}
    with open(os.path.splitext(fbx)[0] + ".report.json", "w", encoding="utf-8") as f:
        json.dump(rep, f, indent=2)
    print("REPORT", json.dumps(rep))
    objs.append(o); reports.append(rep)

# ---- preview: 4 variants side by side, top-down camera at -50 deg pitch, one sun, grey background
sc = bpy.context.scene
for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        sc.render.engine = name; break
    except TypeError:
        continue
sc.render.resolution_x, sc.render.resolution_y = 1600, 600
sc.render.image_settings.file_format = "PNG"
sc.render.filepath = os.path.join(OUTDIR, "preview.png")
sc.view_settings.view_transform = "Standard"; sc.view_settings.look = "None"
w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (0.2, 0.2, 0.2, 1); bg.inputs["Strength"].default_value = 1.0
gap = 1.35
for i, o in enumerate(objs):
    o.location = Vector(((i - 1.5) * gap, 0, 0))
pitch = math.radians(50)   # looking down 50 deg from horizontal -> camera rot X = 90 - 50
cam_d = bpy.data.cameras.new("Cam"); cam_d.type = "ORTHO"; cam_d.ortho_scale = 5.6
cam_d.clip_start = 0.1; cam_d.clip_end = 50
cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam)
cam.rotation_euler = Euler((math.radians(90) - pitch, 0, 0), "XYZ")
fwd = cam.rotation_euler.to_matrix() @ Vector((0, 0, -1))
cam.location = -fwd * 10
sc.camera = cam
sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 3.0
sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
sun.rotation_euler = Euler((math.radians(50), math.radians(10), math.radians(-60)), "XYZ")
bpy.ops.render.render(write_still=True)
print("PREVIEW", sc.render.filepath)
