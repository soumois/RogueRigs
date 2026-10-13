"""XP nut preview (task-128-01, fidelity camera task-130-01). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_nut_preview.py -- <in.fbx> <out.png> [azimuth_deg elevation_deg]
Flat colours, hole emissive. 1000x1000 EEVEE, orthographic scale 50 (barrel: 200), azimuth -35, elevation 28, target z 8.5."""
import bpy, math, os, sys
from mathutils import Vector, Euler

args = sys.argv[sys.argv.index("--") + 1:]
FBX, OUT = os.path.abspath(args[0]), os.path.abspath(args[1])

def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=FBX)
obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
h = max(v.co.z for v in obj.data.vertices) * obj.scale.z
if False:
    obj.scale = obj.scale * (1.0)
bpy.context.view_layer.update()
for m in obj.data.materials:
    m.use_nodes = True
    b = m.node_tree.nodes.get("Principled BSDF") or m.node_tree.nodes.new("ShaderNodeBsdfPrincipled")
    b.inputs["Roughness"].default_value = 0.8
    if "hole" in m.name:   # preview only: low emission so the shading of the walls shows (mesh materials keep full emission)
        hc = srgb("#8CF2FF") if "floor" in m.name else srgb("#00E5FF")
        b.inputs["Base Color"].default_value = [x * 0.6 for x in hc[:3]] + [1.0]
        b.inputs["Emission Color"].default_value = hc; b.inputs["Emission Strength"].default_value = 0.3
    else:
        b.inputs["Base Color"].default_value = srgb("#70757B")

sc = bpy.context.scene
for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        sc.render.engine = name; break
    except TypeError:
        continue
sc.render.resolution_x = sc.render.resolution_y = 1000; sc.render.resolution_percentage = 100
sc.render.image_settings.file_format = "PNG"; sc.render.filepath = OUT
sc.view_settings.view_transform = "Standard"; sc.view_settings.look = "None"
w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (0.78, 0.79, 0.81, 1); bg.inputs["Strength"].default_value = 1.0   # light-grey background (A16)

AZ = float(args[2]) if len(args) > 2 else 0.0; EL = float(args[3]) if len(args) > 3 else 45.0
target = Vector((0, 0, 5.0)); az, el, dist = math.radians(AZ), math.radians(EL), 400.0
cam_d = bpy.data.cameras.new("Cam"); cam_d.type = "ORTHO"; cam_d.ortho_scale = 42.0; cam_d.clip_start = 1; cam_d.clip_end = 2000
cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam)
cam.location = target + Vector((math.sin(az) * math.cos(el), -math.cos(az) * math.cos(el), math.sin(el))) * dist
cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
sc.camera = cam
sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 4.0
sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
sun.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)], "XYZ")
bpy.ops.render.render(write_still=True)
print("rendered", OUT)
