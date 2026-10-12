"""Barrel preview (task-101-07). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/barrel_preview.py -- <in.fbx> <out.png>
Loads the FBX; materials are flat colours (edge bands and bolts have their own lighter materials); faint scratches on lid and cap only:
colour = lerp(surface colour, lighter tone, (1 - T_WearMask) * 0.6) on UV channel 0, 100 cm per tile; decals unmasked, then renders ONE 1000x1000 EEVEE image. Units: 1 Blender unit = 1 cm."""
import bpy, math, os, sys
from mathutils import Vector, Euler

args = sys.argv[sys.argv.index("--") + 1:]
FBX, OUT = os.path.abspath(args[0]), os.path.abspath(args[1])
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
MASK = os.path.join(ROOT, "Art", "Textures", "T_WearMask.png")
TILE_CM = 100.0

def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
PAINT = {"body": "#FF6A00", "ring": "#D9480F", "lid": "#3C4048", "cap": "#2B2E34",
         "lid_edge": "#5C626D", "cap_edge": "#4A4F58", "ring_edge": "#EE6A2E"}
LIGHT = {"lid": "#5C626D", "cap": "#4A4F58"}   # lighter tones for the faint scratch lerp

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=FBX)
obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
print("UV layers:", [u.name for u in obj.data.uv_layers])
# normalise to 1 unit = 1 cm whatever the importer scale
h = max(v.co.z for v in obj.data.vertices) * obj.scale.z
if h > 0 and abs(h - 134.0) > 1.0:
    obj.scale = obj.scale * (134.0 / h)
bpy.context.view_layer.update()

mask = bpy.data.images.load(MASK); mask.colorspace_settings.name = "Non-Color"
for m in obj.data.materials:
    key = m.name.replace("M_Barrel_", "").split(".")[0]
    m.use_nodes = True
    nt = m.node_tree; bsdf = nt.nodes.get("Principled BSDF") or nt.nodes.new("ShaderNodeBsdfPrincipled")
    bsdf.inputs["Roughness"].default_value = 0.8
    if key in ("lid", "cap"):
        # faint scratches: colour = lerp(surface colour, lighter tone, (1 - mask) * 0.6), UV channel 0, 100 cm per tile
        uv = nt.nodes.new("ShaderNodeUVMap"); uv.uv_map = "UVMap"
        tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = mask; tx.extension = "REPEAT"; tx.interpolation = "Linear"
        inv = nt.nodes.new("ShaderNodeMath"); inv.operation = "SUBTRACT"; inv.inputs[0].default_value = 1.0
        mul = nt.nodes.new("ShaderNodeMath"); mul.operation = "MULTIPLY"; mul.inputs[1].default_value = 0.6
        mix = nt.nodes.new("ShaderNodeMix"); mix.data_type = "RGBA"; mix.blend_type = "MIX"
        mix.inputs[6].default_value = srgb(PAINT[key]); mix.inputs[7].default_value = srgb(LIGHT[key])
        nt.links.new(uv.outputs["UV"], tx.inputs["Vector"]); nt.links.new(tx.outputs["Color"], inv.inputs[1])
        nt.links.new(inv.outputs["Value"], mul.inputs[0]); nt.links.new(mul.outputs["Value"], mix.inputs[0])
        nt.links.new(mix.outputs[2], bsdf.inputs["Base Color"])
    elif key in PAINT:
        bsdf.inputs["Base Color"].default_value = srgb(PAINT[key])   # flat colour, no mask
    elif key in ("sticker", "pictogram"):
        img = bpy.data.images.load(os.path.join(os.path.dirname(FBX), "T_Barrel_%s.png" % key.capitalize()))
        tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = img
        nt.links.new(tx.outputs["Color"], bsdf.inputs["Base Color"])
        if key == "pictogram":
            nt.links.new(tx.outputs["Alpha"], bsdf.inputs["Alpha"])

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

target = Vector((0, 0, 67)); az, el, dist = math.radians(-35), math.radians(28), 400.0
cam_d = bpy.data.cameras.new("Cam"); cam_d.type = "ORTHO"; cam_d.ortho_scale = 200.0; cam_d.clip_start = 1; cam_d.clip_end = 2000
cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam)
cam.location = target + Vector((math.sin(az) * math.cos(el), -math.cos(az) * math.cos(el), math.sin(el))) * dist
cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
sc.camera = cam

sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 4.0
sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
sun.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)], "XYZ")

bpy.ops.render.render(write_still=True)
print("rendered", OUT)
