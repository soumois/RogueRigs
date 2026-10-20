"""task-252-01: orthographic previews of a drone FBX (Workbench, flat light, base colour texture). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/drone_preview.py -- <in.fbx> <out-prefix> [--views front,side,top,back,iso] [--px 900] [--scale 1.0] [--centre x,y,z] [--frame 120]
Writes <out-prefix>_<view>.png. Front = +X, left = +Y (Blender), Z up. --scale multiplies imported objects (use 100 on a raw Meshy FBX in metres)."""
import bpy, sys, math
from mathutils import Vector, Matrix
a = sys.argv[sys.argv.index("--") + 1:]
fbx, prefix = a[0], a[1]
opt = {"--views": "front,side,top", "--px": "900", "--scale": "1.0", "--centre": "0,0,0", "--frame": "120"}
for i in range(2, len(a) - 1, 2):
    opt[a[i]] = a[i + 1]
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
S = float(opt["--scale"])
for o in bpy.data.objects:
    if o.parent is None:
        o.scale = (o.scale[0] * S, o.scale[1] * S, o.scale[2] * S)
sc = bpy.context.scene
sc.render.engine = "BLENDER_WORKBENCH"
sc.display.shading.light = "FLAT"
sc.display.shading.color_type = "TEXTURE"
sc.display.render_aa = "FXAA"
sc.view_settings.view_transform = "Standard"
w = bpy.data.worlds.new("W")
w.color = (1, 1, 1)
sc.world = w
cam = bpy.data.cameras.new("cam")
cam.type = "ORTHO"
cam.clip_start, cam.clip_end = 1, 5000
co = bpy.data.objects.new("cam", cam)
sc.collection.objects.link(co)
sc.camera = co
sc.render.resolution_x = sc.render.resolution_y = int(opt["--px"])
C = Vector([float(x) for x in opt["--centre"].split(",")])
D = {"front": ((1, 0, 0), (0, 0, 1)), "side": ((0, -1, 0), (0, 0, 1)), "back": ((-1, 0, 0), (0, 0, 1)), "top": ((0, 0, 1), (1, 0, 0)), "iso": ((1, -1, 0.8), (0, 0, 1)),
     "bottom": ((0, 0, -1), (1, 0, 0))}
for vn in opt["--views"].split(","):
    d, up = D[vn]
    d = Vector(d).normalized()
    fwd = -d
    r = fwd.cross(Vector(up)).normalized()
    u = r.cross(fwd).normalized()
    co.location = C + d * 1000
    co.rotation_euler = Matrix((r, u, -fwd)).transposed().to_euler()
    cam.ortho_scale = float(opt["--frame"])
    sc.render.filepath = "%s_%s.png" % (prefix, vn)
    bpy.ops.render.render(write_still=True)
print("PREVIEW DONE")
