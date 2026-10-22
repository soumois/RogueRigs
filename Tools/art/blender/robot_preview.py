"""Textured robot preview (task-281). Headless, same camera elevation, sun, world and EEVEE as barrel_preview.py, scaled to a 190 cm robot:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/robot_preview.py -- <in.fbx> <basecolor.png> <out.png> <front34|back34>
Base colour texture on UV0, roughness 0.8, no metal. The mesh is not rescaled (a Meshy robot is about 190 cm): orthographic scale 200 * 190/134,
target = bounding-box centre. The robot faces -Y in the FBX import (checked on the heavy raw mesh); front34 looks from the front-left,
back34 from the back-right (same elevation 28 degrees). One 1000x1000 render. Units: 1 Blender unit = 1 m (import), camera set in metres."""
import bpy, math, os, sys
from mathutils import Vector, Euler

args = sys.argv[sys.argv.index("--") + 1:]
FBX, TEX, OUT = (os.path.abspath(a) for a in args[:3])
VIEW = args[3]
CLOSE = None      # optional close-up view "closeup:<az_deg>:<el_deg>:<target_z_cm>:<ortho_cm>" (task-282): same sun, world and EEVEE, target on the robot axis (x, y of the bounding-box centre) at the given height above the bounding-box bottom
if VIEW.startswith("closeup:"):
    CLOSE = [float(v) for v in VIEW.split(":")[1:]]
    assert len(CLOSE) == 4, "closeup:<az>:<el>:<target_z_cm>:<ortho_cm>"
else:
    assert VIEW in ("front34", "back34"), "view must be front34, back34 or closeup:<az>:<el>:<target_z_cm>:<ortho_cm>"
FRONT_AXIS = (0.0, -1.0)   # direction the robot faces (x, y), FBX import

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=FBX)
obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
bpy.context.view_layer.update()
corners = [obj.matrix_world @ Vector(c) for c in obj.bound_box]
lo = Vector([min(c[i] for c in corners) for i in range(3)]); hi = Vector([max(c[i] for c in corners) for i in range(3)])
size = max(hi - lo)
unit = 1.0 if size < 20 else 100.0          # importer gives metres (size ~1.9) or centimetres (~190)
k = unit / 100.0 if size < 20 else 1.0       # factor to reach a 1 unit = 1 cm like the barrel camera
S = (190.0 / 134.0)

mat = bpy.data.materials.new("RobotPreview"); mat.use_nodes = True
bsdf = mat.node_tree.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 0.8
tx = mat.node_tree.nodes.new("ShaderNodeTexImage"); tx.image = bpy.data.images.load(TEX); tx.interpolation = "Linear"
uvn = mat.node_tree.nodes.new("ShaderNodeUVMap"); uvn.uv_map = obj.data.uv_layers[0].name
mat.node_tree.links.new(uvn.outputs["UV"], tx.inputs["Vector"])
mat.node_tree.links.new(tx.outputs["Color"], bsdf.inputs["Base Color"])
obj.data.materials.clear(); obj.data.materials.append(mat)

sc = bpy.context.scene
for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        sc.render.engine = name; break
    except TypeError:
        continue
sc.render.resolution_x = sc.render.resolution_y = 1000; sc.render.resolution_percentage = 100
sc.render.image_settings.file_format = "PNG"; sc.render.filepath = OUT
sc.view_settings.view_transform = "Standard"; sc.view_settings.look = "None"
sc.view_settings.exposure = 0.0; sc.view_settings.gamma = 1.0

w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (1, 1, 1, 1); bg.inputs["Strength"].default_value = 0.35

# metres: camera numbers of barrel_preview.py (cm) times 0.01 and the 190/134 scale
unit_m = 0.01 if size < 20 else 1.0
target = (lo + hi) / 2
az = math.radians(-35 if VIEW == "front34" else 145); el = math.radians(28); dist = 400.0 * S * unit_m
ortho = 200.0 * S * unit_m
if CLOSE:
    az, el = math.radians(CLOSE[0]), math.radians(CLOSE[1]); ortho = CLOSE[3] * unit_m
    target = Vector((target.x, target.y, lo.z + CLOSE[2] * unit_m))
cam_d = bpy.data.cameras.new("Cam"); cam_d.type = "ORTHO"; cam_d.ortho_scale = ortho
cam_d.clip_start = 1 * unit_m; cam_d.clip_end = 2000 * unit_m
cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam)
cam.location = target + Vector((math.sin(az) * math.cos(el), -math.cos(az) * math.cos(el), math.sin(el))) * dist
cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
sc.camera = cam

sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 4.0
sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
sun.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)], "XYZ")

bpy.ops.render.render(write_still=True)
print("rendered", OUT, "bbox", tuple(lo), tuple(hi))
