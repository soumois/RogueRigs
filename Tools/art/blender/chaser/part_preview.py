"""Render one chaser part alone (task-151-05). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser/part_preview.py -- --only <headtorso|pelvis|upperarm|forearmhand|thigh|shinfoot> [--side L|R] [--out-dir Art/Meshes/chaser/parts] [--ppc 11.7]
Writes <out-dir>/<part>_front.png, _side.png, _back.png (orthographic, 11.7 px per cm, light grey background), <part>_closeup.png (window from PREVIEW in the part module,
same scale) and <out-dir>/<part>.report.json (tris, bounds, pivot, attachment section; an existing fidelity object is kept). Both sides of a sided part are built for the report; the render shows --side (default L)."""
import bpy, math, json, os, sys, importlib
from mathutils import Vector

HERE = os.path.dirname(os.path.abspath(__file__)) if "__file__" in globals() else os.path.abspath("Tools/art/blender/chaser")
if HERE not in sys.path:
    sys.path.insert(0, HERE)
import common as C

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--only": "", "--side": "L", "--out-dir": "Art/Meshes/chaser/parts", "--ppc": "11.7", "--margin": "3.0"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
G = opt["--only"]
if G not in C.GROUPS:
    raise SystemExit("--only must be one of " + ", ".join(C.GROUPS))
OUTD = os.path.abspath(opt["--out-dir"])
PPC = float(opt["--ppc"])
MARGIN = float(opt["--margin"])
os.makedirs(OUTD, exist_ok=True)
modn, base, sided = C.GROUPS[G]
mod = importlib.import_module(modn)

C.setup(os.path.dirname(OUTD))          # T_ChaserLogo.png next to the parts folder (Art/Meshes/chaser)
sides = ["L", "R"] if sided else [None]
objs = {s: mod.build(s) for s in sides}
rep = C.write_part_report(G, list(objs.values()), OUTD)
shown = objs[opt["--side"] if sided else None]
for s, o in objs.items():
    if o is not shown:
        o.hide_render = True
bpy.context.view_layer.update()

sc = bpy.context.scene
for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        sc.render.engine = name
        break
    except TypeError:
        pass
sc.view_settings.view_transform = "Standard"
sc.render.film_transparent = False
sc.render.resolution_percentage = 100
w = bpy.data.worlds.new("W")
sc.world = w
w.use_nodes = True
bgn = w.node_tree.nodes["Background"]
bgn.inputs["Color"].default_value = (0.62, 0.62, 0.62, 1)      # light grey (about 0.81 sRGB)
bgn.inputs["Strength"].default_value = 1.0
suns = []
for e in (3.0, 0.8):
    sun = bpy.data.lights.new("sun", "SUN")
    sun.energy = e
    so = bpy.data.objects.new("sun", sun)
    sc.collection.objects.link(so)
    suns.append(so)
cam = bpy.data.cameras.new("cam")
cam.type = "ORTHO"
co = bpy.data.objects.new("cam", cam)
sc.collection.objects.link(co)
sc.camera = co


def light_from(cam_obj):
    fwd = (cam_obj.rotation_euler.to_matrix() @ Vector((0, 0, -1)))
    k = (fwd + Vector((0, 0, -0.9))).normalized()
    suns[0].rotation_euler = k.to_track_quat("-Z", "Y").to_euler()
    f = (-fwd * 0.5 + Vector((0, 0, -1))).normalized()
    suns[1].rotation_euler = f.to_track_quat("-Z", "Y").to_euler()


ROT = {"front": (90, 0, 90), "side": (90, 0, 180), "back": (90, 0, -90)}


def shot(view, centre, wcm, hcm, path):
    """camera on the axis of the view, window wcm x hcm centred on `centre` (world cm), PPC pixels per cm"""
    cx, cy, cz = centre
    if view == "front":
        loc = (300.0, cy, cz)
    elif view == "back":
        loc = (-300.0, cy, cz)
    else:
        loc = (cx, 300.0, cz)
    co.location = loc
    co.rotation_euler = tuple(math.radians(a) for a in ROT[view])
    cam.ortho_scale = max(wcm, hcm)
    cam.clip_end = 1000.0
    bpy.context.view_layer.update()
    light_from(co)
    sc.render.resolution_x, sc.render.resolution_y = int(round(wcm * PPC)), int(round(hcm * PPC))
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)


o = shown
bb = [o.matrix_world @ Vector(c) for c in o.bound_box]
lo = Vector((min(v.x for v in bb), min(v.y for v in bb), min(v.z for v in bb)))
hi = Vector((max(v.x for v in bb), max(v.y for v in bb), max(v.z for v in bb)))
ctr = (lo + hi) / 2
ext = (hi - lo) + Vector((2 * MARGIN,) * 3)
for vn in ("front", "side", "back"):
    wcm = ext.x if vn == "side" else ext.y
    shot(vn, tuple(ctr), wcm, ext.z, os.path.join(OUTD, "%s_%s.png" % (G, vn)))
cl = getattr(mod, "PREVIEW", {}).get("close")
if cl:
    view, centre, size = cl
    cen = Vector(centre)
    if sided and opt["--side"] == "R":
        cen.y = -cen.y
    shot(view, tuple(cen), size, size, os.path.join(OUTD, "%s_closeup.png" % G))
print("PART", G, "TRIS", rep["tris"], {k: v["tris"] for k, v in rep["objects"].items()})
for ob in [co] + suns:
    bpy.data.objects.remove(ob)
