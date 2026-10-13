"""XP chip preview (task-129-01), copy of barrel_preview.py with the same camera angles and light, scaled to the object. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_chip_big_preview.py -- <in.fbx> <out.png>
1000x1000 EEVEE, orthographic scale 60, azimuth -35, elevation 28, distance 400 cm, target z 4, one sun 4.0 at (50, 10, -60), white world 0.35, Standard."""
import bpy, math, os, sys
from mathutils import Vector, Euler

args = sys.argv[sys.argv.index("--") + 1:]
FBX, OUT = os.path.abspath(args[0]), os.path.abspath(args[1])
ORTHO = float(args[2]) if len(args) > 2 else 50.0
TZ = float(args[3]) if len(args) > 3 else 4.5
CX = float(args[4]) if len(args) > 4 else 0.0
CY = float(args[5]) if len(args) > 5 else 0.0


def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


PAINT = {"board": "#2B2E34", "casing": "#3C4048", "casing_chamfer": "#585E69", "edge_band": "#8F959F", "legs": "#8A9099", "core_bright": "#FF26BF", "core_dark": "#B0107F"}
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=FBX)
obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
h = max(v.co.z for v in obj.data.vertices) * obj.scale.z
if False:
    obj.scale = obj.scale * (10.3 / h)
bpy.context.view_layer.update()
for m in obj.data.materials:
    key = m.name.replace("M_XPChipBig_", "").split(".")[0]
    m.use_nodes = True
    bsdf = m.node_tree.nodes.get("Principled BSDF") or m.node_tree.nodes.new("ShaderNodeBsdfPrincipled")
    bsdf.inputs["Roughness"].default_value = 0.8
    if key in PAINT:
        bsdf.inputs["Base Color"].default_value = srgb(PAINT[key])
    if key.startswith("core"):
        bsdf.inputs["Emission Color"].default_value = srgb(PAINT[key])
        bsdf.inputs["Emission Strength"].default_value = 0.5

sc = bpy.context.scene
for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        sc.render.engine = name
        break
    except TypeError:
        continue
sc.render.resolution_x = sc.render.resolution_y = 1000
sc.render.resolution_percentage = 100
sc.render.image_settings.file_format = "PNG"
sc.render.filepath = OUT
sc.view_settings.view_transform = "Standard"
sc.view_settings.look = "None"
sc.view_settings.exposure = 0.0
sc.view_settings.gamma = 1.0
w = bpy.data.worlds.new("W")
w.use_nodes = True
sc.world = w
bg = w.node_tree.nodes["Background"]
bg.inputs["Color"].default_value = (0.65, 0.65, 0.65, 1)
bg.inputs["Strength"].default_value = 1.0
target = Vector((CX, CY, TZ))
az, el, dist = math.radians(45), math.radians(35), 400.0
cam_d = bpy.data.cameras.new("Cam")
cam_d.type = "ORTHO"
cam_d.ortho_scale = ORTHO
cam_d.clip_start = 1
cam_d.clip_end = 2000
cam = bpy.data.objects.new("Cam", cam_d)
sc.collection.objects.link(cam)
cam.location = target + Vector((math.sin(az) * math.cos(el), -math.cos(az) * math.cos(el), math.sin(el))) * dist
cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
sc.camera = cam
sun_d = bpy.data.lights.new("Sun", "SUN")
sun_d.energy = 3.0
sun = bpy.data.objects.new("Sun", sun_d)
sc.collection.objects.link(sun)
sun.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)], "XYZ")
bpy.ops.render.render(write_still=True)
print("rendered", OUT)
