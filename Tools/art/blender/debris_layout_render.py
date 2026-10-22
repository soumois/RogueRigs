"""task-288-01: lay the debris pieces side by side (front view, grid) and render one PNG. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/debris_layout_render.py -- --dir Art/Source/heavy-meshy/debris --tex <png> --out Saved/Renders/task-288/heavy_debris_layout.png"""
import bpy, sys, os, json
from mathutils import Vector
a = sys.argv[sys.argv.index("--") + 1:]
o = dict(zip(a[::2], a[1::2]))
D = os.path.abspath(o["--dir"])
rep = json.load(open(os.path.join(D, "debris_pieces.json")))
bpy.ops.wm.read_factory_settings(use_empty=True)
img = bpy.data.images.load(os.path.abspath(o["--tex"]))
mat = bpy.data.materials.new("m"); mat.use_nodes = True
nt = mat.node_tree; t = nt.nodes.new("ShaderNodeTexImage"); t.image = img
nt.links.new(t.outputs["Color"], nt.nodes["Principled BSDF"].inputs["Base Color"])
cols = 5; cell = 230.0; pos = []
for i, p in enumerate(rep["pieces"]):
    _before = {x.name for x in bpy.data.objects}
    bpy.ops.import_scene.fbx(filepath=os.path.join(D, p["file"]), axis_forward="-Z", axis_up="Y")
    ob = [x for x in bpy.data.objects if x.name not in _before and x.type == "MESH"][0]
    ob.data.materials.clear(); ob.data.materials.append(mat)
    for poly in ob.data.polygons: poly.use_smooth = False
    r, c = divmod(i, cols)
    ob.location = (c * cell, 0, -r * cell)  # front view: X right, Z up
bpy.ops.object.light_add(type="SUN", rotation=(0.9, 0.2, 0.5)); bpy.context.object.data.energy = 3
w = bpy.data.worlds.new("w"); w.use_nodes = True; w.node_tree.nodes["Background"].inputs[1].default_value = 1.2
w.node_tree.nodes["Background"].inputs[0].default_value = (0.5, 0.5, 0.55, 1); bpy.context.scene.world = w
cam = bpy.data.cameras.new("c"); cam.type = "ORTHO"; cam.ortho_scale = 1250
co = bpy.data.objects.new("c", cam); bpy.context.scene.collection.objects.link(co)
co.location = (2 * cell, -1000, -cell * 1.0); co.rotation_euler = (1.5708, 0, 0); bpy.context.scene.camera = co
s = bpy.context.scene; s.render.resolution_x = 1600; s.render.resolution_y = 900
s.render.engine = "BLENDER_EEVEE" if "BLENDER_EEVEE" in [e.identifier for e in s.render.bl_rna.properties["engine"].enum_items] else "BLENDER_EEVEE_NEXT"
s.view_settings.view_transform = "Standard"
s.render.filepath = os.path.abspath(o["--out"]); bpy.ops.render.render(write_still=True)
