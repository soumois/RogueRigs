"""Measure the burst rifle Meshy FBX and its five maps (headless Blender).

Usage: blender -b --factory-startup --python burst_rifle_identify.py -- <fbx> <maps_dir> <orig_dir> <report.json>
fbx: the staged FBX (short path). maps_dir: renamed maps (burst_rifle_meshy_*.png). orig_dir: the extracted zip
folder (original names and SHA-256 are recorded). Reports triangles, vertices, objects, bounds in cm, material
slots, UV layers, embedded textures, map sizes and channels, file sizes and SHA-256.
"""
import bpy
import bmesh
import hashlib
import json
import os
import sys
from mathutils import Vector

fbx, maps_dir, orig_dir, out = sys.argv[sys.argv.index("--") + 1:][:4]
SUF = {"basecolor": ".png", "metallic": "_metallic.png", "metallic_roughness": "_metallic_roughness.png",
       "normal": "_normal.png", "roughness": "_roughness.png"}
ORIG = "Meshy_AI_Aqua_Wasteland_Carbine_20261030_texture_remesh_tri"


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()


bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
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
    mats += [s.material.name for s in o.material_slots if s.material]
scale = bpy.context.scene.unit_settings.scale_length
dims = [round((mx[i] - mn[i]) * scale * 100, 2) for i in range(3)]
embedded = {i.name: {"size": list(i.size), "channels": i.channels, "packed": i.packed_file is not None}
            for i in bpy.data.images}

maps = {}
for key, suf in SUF.items():
    p = os.path.join(maps_dir, "burst_rifle_meshy_%s.png" % key)
    op = os.path.join(orig_dir, ORIG + suf)
    im = bpy.data.images.load(p)
    maps[key] = {"file": os.path.basename(p), "original_name": os.path.basename(op), "size_bytes": os.path.getsize(p),
                 "sha256": sha(p), "original_sha256": sha(op), "size": list(im.size), "channels": im.channels,
                 "is_float": im.is_float}
res = {"fbx": os.path.basename(fbx), "original_name": ORIG + ".fbx", "size_bytes": os.path.getsize(fbx),
       "sha256": sha(fbx), "original_sha256": sha(os.path.join(orig_dir, ORIG + ".fbx")),
       "triangles": tris, "vertices": verts, "objects": objs, "bbox_cm_blender_axes": dims, "unit_scale": scale,
       "materials": mats, "uv_layers": uvs, "embedded_textures": embedded, "maps": maps}
with open(out, "w") as f:
    json.dump(res, f, indent=2)
print("RESULT", tris, verts, dims, mats, uvs)
