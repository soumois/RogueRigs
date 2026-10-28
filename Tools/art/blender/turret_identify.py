"""Measure turret Meshy FBX files (headless Blender).

Usage: blender -b --factory-startup --python turret_identify.py -- <fbxA> <fbxB> <report.json>
Reports triangles (evaluated, triangulated), vertices, objects, bbox in cm,
material slots, UV layers and image sizes for each FBX.
"""
import bpy
import bmesh
import json
import sys
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:]
fbxs, out = argv[:-1], argv[-1]


def measure(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    dg = bpy.context.evaluated_depsgraph_get()
    tris = verts = 0
    objs, mats, uvs = [], [], {}
    mn = Vector((1e9,) * 3)
    mx = Vector((-1e9,) * 3)
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
        objs.append(o.name)
        uvs[o.name] = [u.name for u in o.data.uv_layers]
        for s in o.material_slots:
            if s.material:
                mats.append(s.material.name)
    scale = bpy.context.scene.unit_settings.scale_length
    dims = [(mx[i] - mn[i]) * scale * 100 for i in range(3)]
    imgs = {i.name: list(i.size) for i in bpy.data.images}
    return {"file": path, "triangles": tris, "vertices": verts, "objects": objs,
            "bbox_cm": [round(d, 2) for d in dims], "unit_scale": scale,
            "materials": mats, "uv_layers": uvs, "images": imgs}


res = [measure(p) for p in fbxs]
with open(out, "w") as f:
    json.dump(res, f, indent=2)
for r in res:
    print("RESULT", r["file"], r["triangles"], r["vertices"], r["bbox_cm"], r["materials"])
