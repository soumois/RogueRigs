"""task-284-01: preview frames of a baked walk on the rigid parts (parts FBX + keyed poses stored in the anim report, flat base colour texture, Workbench flat light, fixed orthographic cameras).
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/robot_walk_preview.py -- --in-fbx <parts.fbx> --in-tex <basecolor.png> --anim-report <report.json> --out-dir <dir> [--frames 0,4,8,12] [--prefix heavy_walk_f] [--frame-cm 300] [--height-cm 248] [--work <dir>]
One PNG per frame: <prefix><i>.png (i = index in --frames), side view (camera at -Y, robot faces right) on the left and three-quarter view (camera front-left at azimuth 40 degrees, elevation 14) on the right, the same two cameras for every frame, a dark floor slab with its top at z = 0.
The poses are the keyed poses of the report (walk_poses); meshy_cut_check.py compares the saved texture with the same poses (differences of about 0.01 cm). Blender only. Nothing is imported in Unreal."""
import bpy, sys, os, json, math
import numpy as np
from mathutils import Vector, Matrix, Euler

argv = sys.argv[sys.argv.index("--") + 1:]
opt = {"--frames": "0,4,8,12", "--prefix": "heavy_walk_f", "--frame-cm": "300", "--height-cm": "248", "--px": "900"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
FR = [int(x) for x in opt["--frames"].split(",")]
REP = json.load(open(opt["--anim-report"], encoding="utf-8"))
POSES = REP["walk_poses"]
opt["--out-dir"] = os.path.abspath(opt["--out-dir"])
os.makedirs(opt["--out-dir"], exist_ok=True)
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=opt["--in-fbx"], colors_type="LINEAR")
OB = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
img = bpy.data.images.load(os.path.abspath(opt["--in-tex"]), check_existing=False)
img.colorspace_settings.name = "sRGB"
mats = {m for o in OB.values() for m in o.data.materials if m}
for m in mats:
    m.use_nodes = True
    nt = m.node_tree
    nt.nodes.clear()
    tn = nt.nodes.new("ShaderNodeTexImage")
    tn.image = img
    pb = nt.nodes.new("ShaderNodeBsdfPrincipled")
    out = nt.nodes.new("ShaderNodeOutputMaterial")
    nt.links.new(tn.outputs[0], pb.inputs["Base Color"])
    nt.links.new(pb.outputs[0], out.inputs[0])
REST_LOC = {n: o.location.copy() for n, o in OB.items()}
for o in OB.values():
    o.rotation_mode = "XYZ"
sc = bpy.context.scene
sc.render.engine = "BLENDER_WORKBENCH"
sc.display.shading.light = "FLAT"
sc.display.shading.color_type = "TEXTURE"
sc.display.render_aa = "8"
sc.view_settings.view_transform = "Standard"
w = bpy.data.worlds.new("W")
w.color = (0.62, 0.62, 0.64)
sc.world = w
sc.render.film_transparent = False
gme = bpy.data.meshes.new("floor")
gv = [(-300, -200, -1.0), (300, -200, -1.0), (300, 200, -1.0), (-300, 200, -1.0), (-300, -200, 0), (300, -200, 0), (300, 200, 0), (-300, 200, 0)]
gme.from_pydata(gv, [], [(0, 1, 2, 3), (4, 7, 6, 5), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)])
gm = bpy.data.materials.new("floor")
gm.diffuse_color = (0.2, 0.2, 0.24, 1)
gme.materials.append(gm)
go = bpy.data.objects.new("floor", gme)
sc.collection.objects.link(go)
cam = bpy.data.cameras.new("c")
cam.type = "ORTHO"
cam.clip_start, cam.clip_end = 1, 5000
co = bpy.data.objects.new("c", cam)
sc.collection.objects.link(co)
sc.camera = co
opt["--work"] = os.path.abspath(opt.get("--work", opt["--out-dir"]))
PX = int(opt["--px"])
sc.render.resolution_x = sc.render.resolution_y = PX
CENTRE = Vector((0, 0, float(opt["--height-cm"]) / 2))
SIZE = float(opt["--frame-cm"])


def aim(d):
    d = Vector(d).normalized()
    fwd = -d
    r = fwd.cross(Vector((0, 0, 1))).normalized()
    u = r.cross(fwd).normalized()
    co.location = CENTRE + d * 1500
    co.rotation_euler = Matrix((r, u, -fwd)).transposed().to_euler()
    cam.ortho_scale = SIZE
    bpy.context.view_layer.update()


az, el = math.radians(40), math.radians(14)
VIEWS = [(0, -1, 0), (math.cos(el) * math.cos(az), -math.cos(el) * math.sin(az), math.sin(el))]      # side, three-quarter (front, robot's right side)


def apply_pose(pose):
    for n, o in OB.items():
        p = pose.get(n, {})
        o.rotation_euler = Euler([math.radians(a) for a in p.get("rot", (0, 0, 0))], "XYZ")
        o.location = REST_LOC[n] + Vector(p.get("loc", (0, 0, 0)))
    bpy.context.view_layer.update()


tmp = os.path.join(opt.get("--work", opt["--out-dir"]), "_tile.png")
for i, f in enumerate(FR):
    apply_pose(POSES[f])
    tiles = []
    for d in VIEWS:
        aim(d)
        sc.render.filepath = tmp
        sc.render.image_settings.file_format = "PNG"
        bpy.ops.render.render(write_still=True)
        im = bpy.data.images.load(tmp, check_existing=False)
        tiles.append(np.array(im.pixels[:], np.float32).reshape(PX, PX, 4))
        bpy.data.images.remove(im)
    a = np.concatenate(tiles, axis=1)
    o = bpy.data.images.new("sheet", a.shape[1], a.shape[0], alpha=False)
    o.colorspace_settings.name = "sRGB"
    o.pixels.foreach_set(a.ravel())
    o.filepath_raw = os.path.join(opt["--out-dir"], "%s%d.png" % (opt["--prefix"], i))
    o.file_format = "PNG"
    o.save()
    bpy.data.images.remove(o)
if os.path.exists(tmp):
    os.remove(tmp)
print("DONE", FR)
