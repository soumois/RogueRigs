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
FLAGS = {f: (f in argv) for f in ("--lit", "--tq", "--backclose", "--top50", "--backtop50")}      # value-less flags
argv = [a for a in argv if a not in FLAGS]
LIT = FLAGS["--lit"]
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
bpy.context.view_layer.update()      # matrix_world current before the report
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
if LIT:
    try:
        sc.eevee.use_raytracing = True
        sc.eevee.fast_gi_method = "AMBIENT_OCCLUSION_ONLY"
        sc.eevee.fast_gi_distance = 6.0
    except Exception as ex_:
        print("lit: no raytracing/AO", ex_)
    for a_, v_ in (("use_gtao", True), ("gtao_distance", 6.0), ("gtao_factor", 1.4), ("use_soft_shadows", True)):      # older EEVEE
        if hasattr(sc.eevee, a_):
            setattr(sc.eevee, a_, v_)
w = bpy.data.worlds.new("W")
sc.world = w
w.use_nodes = True
bgn = w.node_tree.nodes["Background"]
bgn.inputs["Color"].default_value = (0.62, 0.62, 0.62, 1)      # light grey (about 0.81 sRGB)
bgn.inputs["Strength"].default_value = 0.55 if LIT else 1.0
suns = []
for e in ((4.5,) if LIT else (3.0, 0.8)):
    sun = bpy.data.lights.new("sun", "SUN")
    sun.energy = e
    if LIT:
        sun.angle = math.radians(14)      # soft shadows
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
    if LIT:      # key from the upper left front of the camera (travels to the right, away from the camera, and down)
        right = (cam_obj.rotation_euler.to_matrix() @ Vector((1, 0, 0)))
        k = (fwd + right * 0.75 + Vector((0, 0, -1.1))).normalized()
        suns[0].rotation_euler = k.to_track_quat("-Z", "Y").to_euler()
        return
    k = (fwd + Vector((0, 0, -0.9))).normalized()
    suns[0].rotation_euler = k.to_track_quat("-Z", "Y").to_euler()
    f = (-fwd * 0.5 + Vector((0, 0, -1))).normalized()
    suns[1].rotation_euler = f.to_track_quat("-Z", "Y").to_euler()


TAG = "_lit" if LIT else ""
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
    if view in ("tq", "top50", "backtop50"):      # tq: three-quarter front left, slightly above; top50: from the front, 50 degrees above the horizontal (screen up = robot back)
        d = Vector((0.8, 0.55, 0.28)).normalized() if view == "tq" else Vector((math.cos(math.radians(50)) * (-1 if view == "backtop50" else 1), 0.0, math.sin(math.radians(50))))
        co.location = Vector(centre) + d * 300.0
        co.rotation_euler = (-d).to_track_quat("-Z", "Y").to_euler()
    else:
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
    shot(vn, tuple(ctr), wcm, ext.z, os.path.join(OUTD, "%s%s_%s.png" % (G, TAG, vn)))
if FLAGS["--tq"]:
    sz = max(ext.x, ext.y, ext.z)
    shot("tq", tuple(ctr), sz, sz, os.path.join(OUTD, "%s%s_threequarter.png" % (G, TAG)))
if FLAGS["--top50"]:
    sz = max(ext.x, ext.y, ext.z)
    shot("top50", tuple(ctr), sz, sz, os.path.join(OUTD, "%s%s_top50.png" % (G, TAG)))
if FLAGS["--backtop50"]:
    sz = max(ext.x, ext.y, ext.z)
    shot("backtop50", tuple(ctr), sz, sz, os.path.join(OUTD, "%s%s_back_top50.png" % (G, TAG)))
if FLAGS["--backclose"]:
    shot("back", tuple(ctr), ext.y, ext.y, os.path.join(OUTD, "%s%s_backclose.png" % (G, TAG)))
cl = getattr(mod, "PREVIEW", {}).get("close")
if cl:
    view, centre, size = cl
    cen = Vector(centre)
    if sided and opt["--side"] == "R":
        cen.y = -cen.y
    shot(view, tuple(cen), size, size, os.path.join(OUTD, "%s%s_closeup.png" % (G, TAG)))
print("PART", G, "TRIS", rep["tris"], {k: v["tris"] for k, v in rep["objects"].items()})
for ob in [co] + suns:
    bpy.data.objects.remove(ob)
