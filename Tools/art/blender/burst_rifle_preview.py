"""Burst rifle preview renders (task-394-02), headless Blender. Same cameras and light for source and result.

Usage: blender -b --factory-startup -P burst_rifle_preview.py -- <fbx> <out_prefix> [--source] [--grazing]
Renders <out_prefix>_{left,right,top,three_quarter}.png (orthographic 70 cm window, 900 px, EEVEE, grey background, one sun, Standard view).
--source: the raw Meshy FBX is turned 180 deg about Z and scaled by 60/190.26 (the Q4 transform) so it lines up with the game frame.
Both meshes get the same material rebuilt from the four Meshy maps (Art/Meshes/burst-rifle/T_BurstRifle_*.png) and are centred on the bbox x/y, z min = 0.
Game frame: muzzle +X; 'left' camera on +Y (shooter left side), 'right' camera on -Y.
"""
import math
import os
import sys

import bpy
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:]
fbx, prefix = os.path.abspath(argv[0]), os.path.abspath(argv[1])
ROOT = os.getcwd()
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
sc = bpy.context.scene
meshes = [o for o in sc.objects if o.type == "MESH"]
if "--source" in argv:
    for o in meshes:
        o.rotation_euler[2] += math.pi
        o.scale = (60.0 / 190.26,) * 3
bpy.context.view_layer.update()
mn = Vector((1e9,) * 3); mx = Vector((-1e9,) * 3)
for o in meshes:
    for c in o.bound_box:
        w = o.matrix_world @ Vector(c)
        mn = Vector(map(min, mn, w)); mx = Vector(map(max, mx, w))
shift = Vector(((mn.x + mx.x) / 2, (mn.y + mx.y) / 2, mn.z))
for o in meshes:
    o.location -= shift

mat = bpy.data.materials.new("m"); mat.use_nodes = True
nt = mat.node_tree; bsdf = nt.nodes["Principled BSDF"]


def img(k, cs):
    n = nt.nodes.new("ShaderNodeTexImage"); n.image = bpy.data.images.load(os.path.join(ROOT, "Art/Meshes/burst-rifle/T_BurstRifle_%s.png" % k))
    n.image.colorspace_settings.name = cs; return n


nt.links.new(img("BaseColor", "sRGB").outputs["Color"], bsdf.inputs["Base Color"])
nt.links.new(img("Metallic", "Non-Color").outputs["Color"], bsdf.inputs["Metallic"])
nt.links.new(img("Roughness", "Non-Color").outputs["Color"], bsdf.inputs["Roughness"])
nm = nt.nodes.new("ShaderNodeNormalMap"); nt.links.new(img("Normal", "Non-Color").outputs["Color"], nm.inputs["Color"])
nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
for o in meshes:
    o.data.materials.clear(); o.data.materials.append(mat)

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
WIN = 70.0


def shoot(name, az, el):
    a, e = math.radians(az), math.radians(el)
    d = Vector((-math.sin(a) * math.cos(e), -math.cos(a) * math.cos(e), math.sin(e)))   # camera direction from target
    ctr = Vector((0, 0, (mx.z - mn.z) / 2))
    cam.location = ctr + d * 12.0
    cam.rotation_euler = (-d).to_track_quat("-Z", "Y").to_euler()
    cam.data.ortho_scale = WIN * 0.01
    cam.data.clip_end = 60
    sc.render.filepath = f"{prefix}_{name}.png"
    bpy.ops.render.render(write_still=True)
    print("RENDER", sc.render.filepath)


# az 0: camera on -Y (right side of the shooter), az 180: camera on +Y (left side)
for name, az, el in [("right", 0, 0), ("left", 180, 0), ("three_quarter", -35, 35), ("top", 0, 89.5)]:
    shoot(name, az, el)
