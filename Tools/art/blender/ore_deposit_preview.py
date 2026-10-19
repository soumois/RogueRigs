"""Previews of the Duskite deposit meshes on a 134x134x100 lavender slab stand-in (top at z 100), EEVEE.
Run: blender -b --factory-startup -P ore_deposit_preview.py -- --dir Art/Meshes/ore-deposit"""
import bpy, math, os, sys
from mathutils import Vector
argv = sys.argv[sys.argv.index("--") + 1:]
d = argv[argv.index("--dir") + 1]
out = os.path.join(d, "preview"); os.makedirs(out, exist_ok=True)
sc = bpy.context.scene
for n in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try: sc.render.engine = n; break
    except TypeError: pass
sc.view_settings.view_transform = "Standard"
w = bpy.data.worlds.new("W"); w.use_nodes = True
w.node_tree.nodes["Background"].inputs[0].default_value = (0.5, 0.5, 0.52, 1); w.node_tree.nodes["Background"].inputs[1].default_value = 0.5
sc.world = w
sun = bpy.data.objects.new("Sun", bpy.data.lights.new("Sun", "SUN")); sun.data.energy = 3.0
sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(-40)); sc.collection.objects.link(sun)
cam_d = bpy.data.cameras.new("Cam"); cam_d.angle = math.radians(35); cam_d.clip_end = 5000
cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam); sc.camera = cam

# emission / violet preview material: bright magenta at the base fading to deep violet at the tips
m = bpy.data.materials.new("PreviewCrystal"); m.use_nodes = True
nt = m.node_tree; nt.nodes.clear()
pos = nt.nodes.new("ShaderNodeNewGeometry"); sep = nt.nodes.new("ShaderNodeSeparateXYZ")
mp = nt.nodes.new("ShaderNodeMapRange"); mp.inputs[1].default_value = 100; mp.inputs[2].default_value = 220
ramp = nt.nodes.new("ShaderNodeValToRGB")
ramp.color_ramp.elements[0].color = (0.9, 0.2, 1.0, 1); ramp.color_ramp.elements[1].color = (0.12, 0.02, 0.4, 1)
em = nt.nodes.new("ShaderNodeBsdfPrincipled"); em.inputs["Base Color"].default_value = (0.4, 0.1, 0.85, 1); em.inputs["Roughness"].default_value = 0.35; em.inputs["Emission Strength"].default_value = 0.6
o = nt.nodes.new("ShaderNodeOutputMaterial")
nt.links.new(pos.outputs["Position"], sep.inputs[0]); nt.links.new(sep.outputs["Z"], mp.inputs[0])
nt.links.new(mp.outputs[0], ramp.inputs[0]); nt.links.new(ramp.outputs[0], em.inputs["Emission Color"]); nt.links.new(em.outputs[0], o.inputs[0])

sm = bpy.data.materials.new("Slab"); sm.use_nodes = True
b = sm.node_tree.nodes["Principled BSDF"]; b.inputs["Base Color"].default_value = (0.5, 0.45, 0.68, 1); b.inputs["Roughness"].default_value = 0.9

def slab(x):
    bpy.ops.mesh.primitive_cube_add(size=1, location=(x, 0, 50)); s = bpy.context.object
    s.scale = (134, 134, 100); s.data.materials.append(sm)
def load(path, x):
    bpy.ops.import_scene.fbx(filepath=path)
    ob = bpy.context.selected_objects[0]
    ob.location = (x, 0, 100); ob.data.materials.clear(); ob.data.materials.append(m)
    return ob
def clear():
    for ob in list(bpy.data.objects):
        if ob.type == "MESH": bpy.data.objects.remove(ob)
def shot(name, target, pitch, yaw, dist, res):
    sc.render.resolution_x, sc.render.resolution_y = res
    yw, pt = math.radians(yaw), math.radians(pitch)
    fwd = Vector((math.sin(yw) * math.cos(pt), math.cos(yw) * math.cos(pt), math.sin(pt)))
    cam.location = Vector(target) - fwd * dist
    cam.rotation_euler = fwd.to_track_quat("-Z", "Y").to_euler()
    sc.render.filepath = os.path.join(out, name); bpy.ops.render.render(write_still=True)

names = ["SM_DuskiteDeposit_0%d" % i for i in (1, 2, 3)]
for n in names:
    clear(); slab(0); load(os.path.join(d, n + ".fbx"), 0)
    shot(n + "_game.png", (0, 0, 120), -60, 0, 700, (900, 900))
    shot(n + "_side.png", (0, 0, 100), -28, 35, 700, (900, 900))
clear()
for i, n in enumerate(names):
    slab((i - 1) * 190); load(os.path.join(d, n + ".fbx"), (i - 1) * 190)
shot("SM_DuskiteDeposit_all_game.png", (0, 0, 120), -60, 0, 1250, (1800, 800))
shot("SM_DuskiteDeposit_all_side.png", (0, 0, 100), -28, 35, 1250, (1800, 800))
