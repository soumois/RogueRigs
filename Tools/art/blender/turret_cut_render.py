"""task-349-01 renders of the cut turret. Same world, sun, orthographic camera, 900 px EEVEE and Standard view as turret_preview.py.
Usage: blender -b --factory-startup -P turret_cut_render.py -- <out_dir>
Head offset in the base frame is read from Art/Meshes/turret/turret_cut.report.json.
Outputs: assembled_{0,90,180,270}_{three_quarter,top}.png, lifted.png, caps_base.png, caps_head.png, parts_sides.png, parts_head_components.png"""
import bpy, math, sys, os, json
from mathutils import Vector, Euler

out = os.path.abspath(sys.argv[sys.argv.index("--") + 1])
os.makedirs(out, exist_ok=True)
rep = json.load(open("Art/Meshes/turret/turret_cut.report.json"))
OFF = Vector(rep["pivots"]["head_offset_in_base_frame_cm"]) * 0.01


def setup():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=os.path.abspath("Art/Meshes/turret/SM_TurretBase.fbx"))
    base = [o for o in bpy.context.scene.objects if o.type == "MESH"][0]
    bpy.ops.import_scene.fbx(filepath=os.path.abspath("Art/Meshes/turret/SM_TurretHead.fbx"))
    head = [o for o in bpy.context.scene.objects if o.type == "MESH" and o != base][0]
    base.name, head.name = "base", "head"
    sc = bpy.context.scene
    world = bpy.data.worlds.new("w")
    world.use_nodes = True
    world.node_tree.nodes["Background"].inputs[0].default_value = (0.78, 0.78, 0.78, 1)
    world.node_tree.nodes["Background"].inputs[1].default_value = 0.6
    sc.world = world
    sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
    sun.data.energy = 4.0
    sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(-60))
    sc.collection.objects.link(sun)
    cam = bpy.data.objects.new("cam", bpy.data.cameras.new("cam"))
    cam.data.type = "ORTHO"
    sc.collection.objects.link(cam)
    sc.camera = cam
    sc.render.engine = "BLENDER_EEVEE"
    try:
        sc.render.engine = "BLENDER_EEVEE_NEXT"
    except TypeError:
        pass
    sc.render.resolution_x = sc.render.resolution_y = 900
    sc.view_settings.view_transform = "Standard"
    sc.render.film_transparent = False
    return sc, cam, base, head


sc, cam, base, head = setup()
# same normalisation as the single-mesh preview of the assembled turret: x/y centred on the SM_Turret bbox, z from 0
SHIFT = Vector((0, 0, 0))
head.rotation_mode = "XYZ"


def place(yaw_deg=0.0, lift_cm=0.0):
    head.location = OFF + Vector((0, 0, lift_cm * 0.01)) - SHIFT
    head.rotation_euler = Euler((0, 0, math.radians(yaw_deg)), "XYZ")
    base.location = -SHIFT


def shoot(name, az, el, scale, tgt, dist=6.0):
    a, e = math.radians(az), math.radians(el)
    d = Vector((-math.sin(a) * math.cos(e), -math.cos(a) * math.cos(e), math.sin(e)))
    cam.location = tgt + d * dist
    cam.rotation_euler = (-d).to_track_quat("-Z", "Y").to_euler()
    cam.data.ortho_scale = scale
    cam.data.clip_end = 50
    sc.render.filepath = os.path.join(out, name + ".png")
    bpy.ops.render.render(write_still=True)
    print("RENDER", sc.render.filepath)


def tint(ob, colours):
    for i, c in enumerate(colours):
        m = bpy.data.materials.new("t%d" % i)
        m.use_nodes = True
        m.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = c
        m.node_tree.nodes["Principled BSDF"].inputs["Roughness"].default_value = 0.8
        if i < len(ob.data.materials):
            ob.data.materials[i] = m
        else:
            ob.data.materials.append(m)


centre = Vector((0, 0, 0.78))
for ang in (0, 90, 180, 270):
    place(ang)
    shoot("assembled_%d_three_quarter" % ang, -35, 35, 2.6, centre)
    shoot("assembled_%d_top" % ang, 0, 89.5, 2.6, centre)
# head lifted 45 cm, three-quarter
place(45, 45)
shoot("lifted", -35, 25, 2.6, Vector((0, 0, 0.9)))
# caps: base top with the head removed, head underside from below, close-ups
head.hide_render = True
place(0, 0)
shoot("caps_base", -35, 50, 0.7, Vector((OFF.x, OFF.y, OFF.z)))
head.hide_render = False
base.hide_render = True
place(0, 0)
shoot("caps_head", -35, -50, 0.7, Vector((OFF.x, OFF.y, OFF.z)))
base.hide_render = False
# lifted close-up of both caps facing each other
place(30, 25)
shoot("caps_gap", 0, 5, 0.7, Vector((OFF.x, OFF.y, OFF.z + 0.12)))
# parts tinted
tint(base, [(0.1, 0.35, 0.9, 1)])
tint(head, [(1.0, 0.45, 0.05, 1), (0.9, 0.1, 0.1, 1)])
place(0, 0)
shoot("parts_sides", -35, 35, 2.6, centre)
shoot("parts_sides_back", 145, 35, 2.6, centre)
# head components in distinct colours: split loose parts
bpy.ops.object.select_all(action="DESELECT")
head.select_set(True)
bpy.context.view_layer.objects.active = head
bpy.ops.object.mode_set(mode="EDIT")
bpy.ops.mesh.select_all(action="SELECT")
bpy.ops.mesh.separate(type="LOOSE")
bpy.ops.object.mode_set(mode="OBJECT")
pal = [(1, .45, .05), (.9, .1, .1), (.1, .8, .2), (.9, .8, .1), (.7, .2, .9), (.1, .8, .8), (1, .4, .7), (.5, .3, .1), (1, 1, 1)]
parts = sorted([o for o in sc.objects if o.type == "MESH" and o != base], key=lambda o: -len(o.data.polygons))
for i, o in enumerate(parts):
    o.data.materials.clear()
    m = bpy.data.materials.new("p%d" % i)
    m.use_nodes = True
    m.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (*pal[i % len(pal)], 1)
    o.data.materials.append(m)
print("HEAD_COMPONENTS", len(parts), [len(o.data.polygons) for o in parts])
shoot("parts_head_components", -35, 35, 2.6, centre)
shoot("parts_head_components_back", 145, 35, 2.6, centre)
