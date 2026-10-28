"""Turret preview renders (headless Blender). Same camera, light and scale for every mesh.

Usage: blender -b --factory-startup -P turret_preview.py -- <fbx> <out_prefix> [--target x,y,z --closeup-scale s] [--grazing]
Renders <out_prefix>_front.png, _side.png, _three_quarter.png, _canon.png (orthographic, 900 px,
EEVEE, light-grey background, one sun, Standard view transform). Canon close-up looks at --target (cm, normalised space) with --closeup-scale (cm). Scene is in metres (FBX import).
Meshes are normalised to bbox centre x/y = 0 and min z = 0 so views are comparable.
"""
import bpy
import math
import sys
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:]
import os
fbx, prefix = os.path.abspath(argv[0]), os.path.abspath(argv[1])
target = Vector((0, 0, 0))
cscale = 0.6
if "--target" in argv:
    target = Vector(map(float, argv[argv.index("--target") + 1].split(","))) * 0.01
if "--closeup-scale" in argv:
    cscale = float(argv[argv.index("--closeup-scale") + 1]) * 0.01
views = ["front", "side", "three_quarter", "canon"]
if "--views" in argv:
    views = argv[argv.index("--views") + 1].split(",")

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
sc = bpy.context.scene
meshes = [o for o in sc.objects if o.type == "MESH"]
mn = Vector((1e9,) * 3)
mx = Vector((-1e9,) * 3)
for o in meshes:
    for c in o.bound_box:
        w = o.matrix_world @ Vector(c)
        mn = Vector(map(min, mn, w))
        mx = Vector(map(max, mx, w))

# Camera/target are in turret-normalised space: x,y centred, z from 0.
shift = Vector(((mn.x + mx.x) / 2, (mn.y + mx.y) / 2, mn.z))
for o in meshes:
    o.location -= shift

world = bpy.data.worlds.new("w")
world.use_nodes = True
world.node_tree.nodes["Background"].inputs[0].default_value = (0.78, 0.78, 0.78, 1)
world.node_tree.nodes["Background"].inputs[1].default_value = 0.6
sc.world = world
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = 4.0
sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(-60))
if "--grazing" in argv:
    # task-347: low sun skimming the surfaces so bulges and facets show as gradients (default light unchanged)
    sun.rotation_euler = (math.radians(80), math.radians(0), math.radians(-70))
    sun.data.energy = 5.0
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


def shoot(name, az, el, scale, tgt, dist=6.0):
    a, e = math.radians(az), math.radians(el)
    d = Vector((math.sin(a) * math.cos(e) * -1, math.cos(a) * math.cos(e) * -1, math.sin(e)))
    cam.location = tgt + d * dist
    cam.rotation_euler = (-d).to_track_quat("-Z", "Y").to_euler()
    cam.data.ortho_scale = scale
    cam.data.clip_end = 50
    sc.render.filepath = f"{prefix}_{name}.png"
    bpy.ops.render.render(write_still=True)
    print("RENDER", sc.render.filepath)


centre = Vector((0, 0, (mx.z - mn.z) / 2))
spec = {
    "front": (0, 0, 2.6, centre),
    "side": (90, 0, 2.6, centre),
    "three_quarter": (-35, 35, 2.6, centre),
    "canon": (-35, 25, cscale, target),
    "canon_top": (0, 89.5, cscale, target),
    "canon_side": (90, 0, cscale, target),
    "canon_under": (0, -89.5, cscale, target),
}
for v in views:
    az, el, s, t = spec[v]
    shoot(v, az, el, s, t)
