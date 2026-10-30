"""Measure the owner rigged miner Meshy FBX (headless Blender), task-382-01. No mesh edit.

Usage: blender -b --factory-startup --python miner_identify.py -- <fbx> <report.json> [<texture_out_dir>]
Reports triangles (evaluated, triangulated), vertices, objects and types, bbox in cm, armature,
bones, root bones, vertex groups and unweighted vertices, materials, UV layers, images and sizes,
animation actions. Optionally writes packed images byte-exact into texture_out_dir.
"""
import os
import sys

import bpy
import bmesh
import json

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rr_blender as rr  # noqa: E402
from mathutils import Vector  # noqa: E402

argv = sys.argv[sys.argv.index("--") + 1:]
fbx, out = argv[0], argv[1]
texdir = argv[2] if len(argv) > 2 else None

rr.import_fbx(fbx)
dg = bpy.context.evaluated_depsgraph_get()
tris = verts = 0
mn = Vector((1e9,) * 3)
mx = Vector((-1e9,) * 3)
meshes, mats, uvs, vg_info = [], [], {}, {}
for o in bpy.context.scene.objects:
    if o.type != "MESH":
        continue
    eo = o.evaluated_get(dg)
    me = eo.to_mesh()
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces[:])
    tris += len(bm.faces)
    verts += len(bm.verts)
    for v in bm.verts:
        w = eo.matrix_world @ v.co
        mn = Vector(map(min, mn, w))
        mx = Vector(map(max, mx, w))
    bm.free()
    eo.to_mesh_clear()
    meshes.append(o.name)
    uvs[o.name] = [u.name for u in o.data.uv_layers]
    for s in o.material_slots:
        if s.material:
            mats.append(s.material.name)
    unw = sum(1 for v in o.data.vertices if not any(g.weight > 0 for g in v.groups))
    maxinf = max((len(v.groups) for v in o.data.vertices), default=0)
    vg_info[o.name] = {"vertex_groups": len(o.vertex_groups), "vertices_without_weight": unw,
                       "max_influences": maxinf,
                       "armature_modifiers": [m.object.name for m in o.modifiers if m.type == "ARMATURE" and m.object]}

scale = bpy.context.scene.unit_settings.scale_length
dims = [(mx[i] - mn[i]) * scale * 100 for i in range(3)]
arms = []
for o in bpy.context.scene.objects:
    if o.type == "ARMATURE":
        bones = list(o.data.bones)
        arms.append({"name": o.name, "bone_count": len(bones),
                     "root_bones": [b.name for b in bones if b.parent is None],
                     "bone_names": [b.name for b in bones]})
imgs = {}
for i in bpy.data.images:
    imgs[i.name] = {"size": list(i.size), "packed": i.packed_file is not None, "filepath": i.filepath}
    if texdir and i.packed_file is not None:
        os.makedirs(texdir, exist_ok=True)
        with open(os.path.join(texdir, os.path.basename(i.filepath.replace("\\", "/")) or i.name), "wb") as f:
            f.write(i.packed_file.data)
actions = [{"name": a.name, "frame_range": list(a.frame_range)} for a in bpy.data.actions]
res = {"file": fbx, "triangles": tris, "vertices": verts,
       "objects": {o.name: o.type for o in bpy.context.scene.objects},
       "bbox_cm": [round(d, 2) for d in dims], "height_cm_z": round(dims[2], 2),
       "unit_scale": scale, "armatures": arms, "skin": vg_info, "materials": mats,
       "uv_layers": uvs, "images": imgs, "actions": actions}
with open(out, "w") as f:
    json.dump(res, f, indent=2)
print("RESULT", tris, verts, res["bbox_cm"], [(a["name"], a["bone_count"], a["root_bones"]) for a in arms],
      vg_info, mats, imgs, actions)
