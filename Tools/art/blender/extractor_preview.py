"""Extractor preview renders (task-366-02), headless Blender. Same cameras and light for every mesh.

Usage: blender -b --factory-startup -P extractor_preview.py -- <fbx> <out_prefix> [--grazing] [--scale-cm 260] [--rot-z 0] [--basecolor <png>]
Renders <out_prefix>_{front,side,three_quarter,rear_quarter,top}.png (orthographic, 900 px, EEVEE, grey background, one sun, Standard).
The mesh is centred on x/y and set on z = 0; --rot-z turns it (degrees) so a raw-frame source lines up with the game frame;
--scale-cm is the orthographic window (use the same value for source and result after scaling the source by --mesh-scale).
--basecolor re-applies the base colour map when the FBX carries no texture (raw Meshy FBX keeps it embedded).
"""
import math
import os
import sys

import bpy
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:]
fbx, prefix = os.path.abspath(argv[0]), os.path.abspath(argv[1])


def val(name, default):
    return float(argv[argv.index(name) + 1]) if name in argv else default


rotz, win, mscale = val("--rot-z", 0.0), val("--scale-cm", 260.0), val("--mesh-scale", 1.0)
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
sc = bpy.context.scene
meshes = [o for o in sc.objects if o.type == "MESH"]
for o in meshes:
    o.rotation_euler[2] += math.radians(rotz)
    o.scale = (o.scale[0] * mscale,) * 3
bpy.context.view_layer.update()
mn = Vector((1e9,) * 3); mx = Vector((-1e9,) * 3)
for o in meshes:
    for c in o.bound_box:
        w = o.matrix_world @ Vector(c)
        mn = Vector(map(min, mn, w)); mx = Vector(map(max, mx, w))
shift = Vector(((mn.x + mx.x) / 2, (mn.y + mx.y) / 2, mn.z))
for o in meshes:
    o.location -= shift
if "--basecolor" in argv:
    img = bpy.data.images.load(os.path.abspath(argv[argv.index("--basecolor") + 1]))
    for o in meshes:
        for m in o.data.materials:
            if m and m.use_nodes:
                b = m.node_tree.nodes.get("Principled BSDF")
                if b and not b.inputs["Base Color"].is_linked:
                    t = m.node_tree.nodes.new("ShaderNodeTexImage"); t.image = img
                    m.node_tree.links.new(t.outputs["Color"], b.inputs["Base Color"])

world = bpy.data.worlds.new("w"); world.use_nodes = True
world.node_tree.nodes["Background"].inputs[0].default_value = (0.78, 0.78, 0.78, 1)
world.node_tree.nodes["Background"].inputs[1].default_value = 0.6
sc.world = world
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = 4.0
sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(-60))
if "--grazing" in argv:
    sun.rotation_euler = (math.radians(80), math.radians(0), math.radians(-70))
    sun.data.energy = 5.0
sc.collection.objects.link(sun)
cam = bpy.data.objects.new("cam", bpy.data.cameras.new("cam"))
cam.data.type = "ORTHO"
sc.collection.objects.link(cam)
sc.camera = cam
try:
    sc.render.engine = "BLENDER_EEVEE_NEXT"
except TypeError:
    sc.render.engine = "BLENDER_EEVEE"
sc.render.resolution_x = sc.render.resolution_y = 900
sc.view_settings.view_transform = "Standard"


def shoot(name, az, el):
    a, e = math.radians(az), math.radians(el)
    d = Vector((-math.sin(a) * math.cos(e), -math.cos(a) * math.cos(e), math.sin(e)))
    ctr = Vector((0, 0, (mx.z - mn.z) / 2))
    cam.location = ctr + d * 12.0
    cam.rotation_euler = (-d).to_track_quat("-Z", "Y").to_euler()
    cam.data.ortho_scale = win * 0.01
    cam.data.clip_end = 60
    sc.render.filepath = f"{prefix}_{name}.png"
    bpy.ops.render.render(write_still=True)
    print("RENDER", sc.render.filepath)


for name, az, el in [("front", 0, 0), ("side", 90, 0), ("three_quarter", -35, 35), ("rear_quarter", 145, 35), ("top", 0, 89.5)]:
    shoot(name, az, el)
