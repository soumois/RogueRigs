"""XP cube preview (task-217-01). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_cube_big_preview.py -- <in.fbx> <out.png> <azimuth deg> <elevation deg> [ortho_scale]
Orthographic, 1200x800 like the inspect renders, light-grey background, one sun 1.0 that follows the camera (same direction as the view, tilted), Standard view,
emissive shown as flat colour (strength 0, base colour only), glow_disc hidden. Azimuth 0 = looking along +Y from -Y; elevation 90 = straight down."""
import bpy, math, os, sys
from mathutils import Vector

args = sys.argv[sys.argv.index("--") + 1:]
FBX, OUT = os.path.abspath(args[0]), os.path.abspath(args[1])
AZ, EL = math.radians(float(args[2])), math.radians(float(args[3]))
ORTHO = float(args[4]) if len(args) > 4 else 50.0

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=FBX)
obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
for m in obj.data.materials:
    if m.name.startswith("glow_disc"):
        continue
    m.use_nodes = True
    b = m.node_tree.nodes.get("Principled BSDF")
    b.inputs["Roughness"].default_value = 0.8
    b.inputs["Emission Strength"].default_value = 0.0
disc = next((i for i, m in enumerate(obj.data.materials) if m.name.startswith("glow_disc")), None)
for p in obj.data.polygons:                       # hide the disc by hiding its faces
    if p.material_index == disc:
        p.hide = True
import bmesh
bm = bmesh.new(); bm.from_mesh(obj.data)
bmesh.ops.delete(bm, geom=[f for f in bm.faces if f.material_index == disc], context="FACES")
bm.to_mesh(obj.data); bm.free()
bpy.context.view_layer.update()

sc = bpy.context.scene
for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        sc.render.engine = name
        break
    except TypeError:
        continue
sc.render.resolution_x, sc.render.resolution_y = 1200, 800
sc.render.image_settings.file_format = "PNG"
sc.render.filepath = OUT
sc.view_settings.view_transform = "Standard"
sc.view_settings.look = "None"
w = bpy.data.worlds.new("W")
w.use_nodes = True
sc.world = w
bg = w.node_tree.nodes["Background"]
bg.inputs["Color"].default_value = (0.6, 0.6, 0.6, 1)
bg.inputs["Strength"].default_value = 1.0
target = Vector((0, 0, 0))
dist = 400.0
d = Vector((math.sin(AZ) * math.cos(EL), -math.cos(AZ) * math.cos(EL), math.sin(EL)))
cd = bpy.data.cameras.new("Cam")
cd.type = "ORTHO"
cd.ortho_scale = ORTHO
cd.clip_start, cd.clip_end = 1, 2000
cam = bpy.data.objects.new("Cam", cd)
sc.collection.objects.link(cam)
cam.location = target + d * dist
up = "Y" if EL < math.radians(89) else "Y"
cam.rotation_euler = (target - cam.location).to_track_quat("-Z", up).to_euler()
sc.camera = cam
sd = bpy.data.lights.new("Sun", "SUN")
sd.energy = 1.0
sun = bpy.data.objects.new("Sun", sd)
sc.collection.objects.link(sun)
sun.parent = cam
sun.rotation_euler = (math.radians(30), math.radians(20), 0)   # camera-following sun: light comes from over the viewer's shoulder
bpy.ops.render.render(write_still=True)
print("rendered", OUT)
