"""Extractor close-ups of two large panels (task-368-01), headless Blender. Same camera and light for the source, the old and the new mesh.

Usage: blender -b --factory-startup -P extractor_rebake_closeup.py -- <fbx> <out_prefix> [--source <frame.json>]
Renders <out_prefix>_panelA.png (top deck, region 0 of extractor_flat.report.json) and <out_prefix>_panelB.png (rotor-ring panel, region 60):
orthographic, 1000 px, 70 cm window, camera 150 cm in front of the panel centre along its normal, EEVEE, the extractor_preview light.
--source: the FBX is the raw Meshy mesh; frame.json (written by extractor_rebake.py, bake phase) places it in the SM_Extractor frame.
"""
import json
import math
import os
import sys

import bpy
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:]
fbx, prefix = os.path.abspath(argv[0]), os.path.abspath(argv[1])
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
sc = bpy.context.scene
meshes = [o for o in sc.objects if o.type == "MESH"]
if "--source" in argv:
    fr = json.load(open(os.path.abspath(argv[argv.index("--source") + 1])))
    for o in meshes:
        o.rotation_euler = (0, 0, math.radians(90))
        o.scale = (fr["scale"],) * 3
        o.location = Vector(fr["offset_cm"]) * 0.01
bpy.context.view_layer.update()
world = bpy.data.worlds.new("w"); world.use_nodes = True
world.node_tree.nodes["Background"].inputs[0].default_value = (0.78, 0.78, 0.78, 1)
world.node_tree.nodes["Background"].inputs[1].default_value = 0.6
sc.world = world
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = 4.0
sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(-60))
sc.collection.objects.link(sun)
cam = bpy.data.objects.new("cam", bpy.data.cameras.new("cam"))
cam.data.type = "ORTHO"; cam.data.ortho_scale = 0.70; cam.data.clip_end = 60
sc.collection.objects.link(cam); sc.camera = cam
try:
    sc.render.engine = "BLENDER_EEVEE_NEXT"
except TypeError:
    sc.render.engine = "BLENDER_EEVEE"
sc.render.resolution_x = sc.render.resolution_y = 1000
sc.view_settings.view_transform = "Standard"
PANELS = {"panelA": ((-20.63, -0.16, 72.37), (0.022, 0.0005, 0.9998)), "panelB": ((-97.71, -63.63, 71.77), (-0.9993, 0.0016, 0.0376))}
for name, (c, n) in PANELS.items():
    ctr = Vector(c) * 0.01; nn = Vector(n).normalized()
    cam.location = ctr + nn * 1.5
    cam.rotation_euler = (-nn).to_track_quat("-Z", "Y").to_euler()
    sc.render.filepath = f"{prefix}_{name}.png"
    bpy.ops.render.render(write_still=True)
    print("RENDER", sc.render.filepath)
