"""Fixed-camera renders of the console C2 meshes (reused by task-301-02 and 03 for before/after). Headless:
  "$BLENDER_EXE" -b --factory-startup -P console_c2_render.py -- --out-dir <dir> --prefix <name> [--lift-cm 40] [--tint] [--raw] <fbx> [<fbx> ...]
Cameras (centimetres, Z up, all orthographic, 1000x1000, light grey background):
  top   : looks down -Z at (0,0,95), ortho scale 280
  front : looks along +Y from -Y (image right = +X)
  side  : looks along -X from +X (image right = +Y) at (0,0,95), ortho scale 280
  iso   : azimuth -35, elevation 28, distance 600, target (0,0,95), ortho scale 280
Light: one sun energy 4.0 rotated (50, 10, -60), world 0.35 (as blender-asset-route). --lift-cm raises the objects whose name contains 'head'.
--tint paints the slots flat: AntennaOrange magenta, OtherOrange cyan, Screen lime, rest = grey (slot split check). --raw: file is the untouched Meshy FBX (metres)."""
import bpy, sys, os, math
from mathutils import Vector
argv = sys.argv[sys.argv.index("--") + 1:]
a = {"--out-dir": ".", "--prefix": "r", "--lift-cm": "0", "--set": "std"}
files, tint, raw, i = [], False, False, 0
while i < len(argv):
    t = argv[i]
    if t == "--tint": tint = True; i += 1
    elif t == "--raw": raw = True; i += 1
    elif t in a: a[t] = argv[i + 1]; i += 2
    else: files.append(os.path.abspath(t)); i += 1
bpy.ops.wm.read_factory_settings(use_empty=True)
sc = bpy.context.scene
objs = []
for f in files:
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=f)
    for o in set(bpy.data.objects) - before:
        if o.type == "MESH":
            objs.append(o)
            o.scale = (100, 100, 100)      # FBX import gives metres; the scene is read as centimetres
            if raw:
                o.location.z = 95.0
        else:
            bpy.data.objects.remove(o)
lift = float(a["--lift-cm"])
import json
rp = os.path.join(os.path.dirname(files[0]), "console_c2_cut.report.json")
OFF = json.load(open(rp))["pivots"]["head_offset_on_base_cm"] if os.path.exists(rp) and not raw else [0, 0, 0]
for o in objs:
    if "head" in o.name.lower():      # head origin = mast axis at the cut: put it at its offset on the base, plus the optional lift
        o.location = (OFF[0], OFF[1], OFF[2] + lift)
TINT = {"AntennaOrange": (1, 0, 1, 1), "OtherOrange": (0, 0.9, 1, 1), "Screen": (0.4, 1, 0, 1)}
if tint:
    for o in objs:
        for k, s in enumerate(o.material_slots):
            nm = s.material.name.split(".")[0]
            m = bpy.data.materials.new("tint_" + nm)
            m.use_nodes = True
            bsdf = m.node_tree.nodes["Principled BSDF"]
            bsdf.inputs["Base Color"].default_value = TINT.get(nm, (0.55, 0.55, 0.55, 1))
            bsdf.inputs["Roughness"].default_value = 0.8
            s.material = m
sc.render.engine = "BLENDER_EEVEE"
sc.render.resolution_x = sc.render.resolution_y = 1000
sc.view_settings.view_transform = "Standard"
w = bpy.data.worlds.new("w"); w.use_nodes = True
w.node_tree.nodes["Background"].inputs[0].default_value = (0.82, 0.82, 0.82, 1)
w.node_tree.nodes["Background"].inputs[1].default_value = 0.35
sc.world = w
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = 4.0
sun.rotation_euler = [math.radians(x) for x in (50, 10, -60)]
sc.collection.objects.link(sun)
cam = bpy.data.objects.new("cam", bpy.data.cameras.new("cam"))
cam.data.type = "ORTHO"; cam.data.ortho_scale = 280; cam.data.clip_end = 5000
sc.collection.objects.link(cam); sc.camera = cam
T = Vector((0, 0, 95))
def aim(loc, rot):
    cam.location = loc; cam.rotation_euler = [math.radians(x) for x in rot]
views = {"top": (T + Vector((0, 0, 600)), (0, 0, 0)),
         "front": (T + Vector((0, -600, 0)), (90, 0, 0)),
         "side": (T + Vector((600, 0, 0)), (90, 0, 90))}
az, el, d = math.radians(-35), math.radians(28), 600
views["iso"] = (T + Vector((d * math.cos(el) * math.sin(-az) * -1, -d * math.cos(el) * math.cos(az), d * math.sin(el))), (90 - 28, 0, -35))
os.makedirs(a["--out-dir"], exist_ok=True)
SET = a.get("--set", "std")
if SET == "close":
    views = {"back": (T + Vector((0, 600, 0)), (90, 0, 180), 280),
             "back_close": (Vector((0, 600, 125)), (90, 0, 180), 140),
             "back_iso": (Vector((0, 0, 130)) + 600 * Vector((0.5 * math.cos(math.radians(20)), 0.866 * math.cos(math.radians(20)), math.sin(math.radians(20)))), (70, 0, 150), 200),
             "feet_top": (Vector((0, -45, 600)), (0, 0, 0), 170),
             "foot_fl": (Vector((-57, -49, 600)), (0, 0, 0), 55),
             "foot_fr": (Vector((57, -49, 600)), (0, 0, 0), 55),
             "foot_bl": (Vector((-57, 49, 600)), (0, 0, 0), 55),
             "foot_br": (Vector((57, 49, 600)), (0, 0, 0), 55)}
views = {k: (v + (280,))[:3] if len(v) == 2 else v for k, v in views.items()}
for n, (loc, rot, osc) in views.items():
    cam.data.ortho_scale = osc
    aim(loc, rot)
    sc.render.filepath = os.path.join(os.path.abspath(a["--out-dir"]), "%s_%s.png" % (a["--prefix"], n))
    bpy.ops.render.render(write_still=True)
print("RENDERED", a["--prefix"])
