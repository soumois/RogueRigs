"""Rogue Rigs 3D agent: ore chunk meshes from owner-made Meshy web FBX (task-189-02).

  blender -b --factory-startup -P Tools/art/blender/ore_chunk.py -- \
      --in <meshy.fbx> --tex <meshy base colour png> --out Art/Meshes/OreChunk/SM_OreChunk_01.fbx [--size-cm 30]

Steps: import, apply transforms, join, merge vertices closer than 0.01 mm (positions only,
UV and look unchanged) -> flat shading -> uniform scale so the LARGEST dimension is --size-cm
-> pivot at the bounding-box centre -> two slots by base-colour luminance: the luminance of the
Meshy base colour texture is sampled at each face's UV centroid; faces above the median go to
`crystal_bright`, the others to `crystal_dark` (median = balanced split, clear facets in both)
-> UV0 kept from Meshy, UV1 `LightMap` (smart project, packed, no overlap) -> FBX in cm
(FBX_SCALE_UNITS), no texture exported. Writes <out>.report.json.
"""
import json, os, sys
import bpy, bmesh
import numpy as np


def args():
    a = sys.argv[sys.argv.index("--") + 1:]
    o = {"size-cm": "30"}
    for i in range(0, len(a), 2):
        o[a[i][2:]] = a[i + 1]
    return o


def tris(m):
    return sum(len(p.vertices) - 2 for p in m.polygons)


def main():
    o = args()
    rep = {"input": o["in"], "output": o["out"], "warnings": []}
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=o["in"])
    meshes = [ob for ob in bpy.context.scene.objects if ob.type == "MESH"]
    bpy.ops.object.select_all(action="DESELECT")
    for ob in meshes:
        ob.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    bpy.ops.object.parent_clear(type="CLEAR_KEEP_TRANSFORM")
    for ob in [x for x in bpy.context.scene.objects if x.type != "MESH"]:
        bpy.data.objects.remove(ob, do_unlink=True)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    if len(meshes) > 1:
        bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    obj.name = os.path.splitext(os.path.basename(o["out"]))[0]
    mesh = obj.data
    rep["tris_in"] = tris(mesh)
    rep["verts_in"] = len(mesh.vertices)

    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.remove_doubles(bm, verts=bm.verts, dist=0.00001)
    # Meshy geometry check (no repair: hole filling found no simple loop on c2)
    cnt = {}
    for e in bm.edges:
        cnt[len(e.link_faces)] = cnt.get(len(e.link_faces), 0) + 1
    rep["edge_face_counts"] = {str(k): v for k, v in sorted(cnt.items())}
    bad = sum(v for k, v in cnt.items() if k != 2)
    if bad:
        rep["warnings"].append(f"{bad} non-manifold edges (edge face counts {rep['edge_face_counts']}): interpenetrating prisms and small open boundary from Meshy, left as generated")
    bm.to_mesh(mesh)
    bm.free()
    for p in mesh.polygons:
        p.use_smooth = False
    if mesh.has_custom_normals:
        bpy.ops.mesh.customdata_custom_splitnormals_clear()

    # luminance per face from the base colour texture at the UV centroid
    img = bpy.data.images.load(os.path.abspath(o["tex"]))
    w, h = img.size
    px = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, img.channels)
    uv = mesh.uv_layers[0].data
    lum = []
    for p in mesh.polygons:
        u = np.mean([uv[l].uv[0] for l in p.loop_indices]) % 1.0
        v = np.mean([uv[l].uv[1] for l in p.loop_indices]) % 1.0
        c = px[min(h - 1, int(v * h)), min(w - 1, int(u * w))]
        lum.append(0.2126 * c[0] + 0.7152 * c[1] + 0.0722 * c[2])
    lum = np.array(lum)
    thr = float(np.median(lum))

    # scale + pivot (bounding-box centre)
    co = np.array([v.co[:] for v in mesh.vertices])
    lo, hi = co.min(0), co.max(0)
    s = (float(o["size-cm"]) / 100.0) / float((hi - lo).max())
    co = (co - (lo + hi) / 2) * s
    for v, c in zip(mesh.vertices, co):
        v.co = c
    mesh.update()

    # two slots; flat colours (placeholders: final tint is set in the Unreal material)
    mesh.materials.clear()
    for name, col in (("crystal_bright", (0.62, 0.35, 1.0, 1)), ("crystal_dark", (0.22, 0.08, 0.45, 1))):
        m = bpy.data.materials.new(name)
        m.diffuse_color = col
        mesh.materials.append(m)
    for p, l in zip(mesh.polygons, lum):
        p.material_index = 0 if l > thr else 1
    split = [0, 0]
    for p in mesh.polygons:
        split[p.material_index] += len(p.vertices) - 2
    if min(split) < 0.2 * sum(split):
        rep["warnings"].append(f"unbalanced slot split {split}")

    # LightMap UV1
    bpy.ops.object.mode_set(mode="OBJECT")
    mesh.uv_layers[0].name = "UVMap"
    lm = mesh.uv_layers.new(name="LightMap")
    mesh.uv_layers.active = lm
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=1.15, island_margin=0.01)
    bpy.ops.uv.select_all(action="SELECT")
    bpy.ops.uv.pack_islands(margin=0.01)
    bpy.ops.object.mode_set(mode="OBJECT")
    mesh.uv_layers.active_index = 0
    u1 = np.array([d.uv[:] for d in mesh.uv_layers["LightMap"].data])
    rep["lightmap_bounds"] = [round(float(u1.min()), 4), round(float(u1.max()), 4)]
    if u1.min() < 0 or u1.max() > 1:
        rep["warnings"].append("LightMap UV outside 0..1")

    co = np.array([v.co[:] for v in mesh.vertices])
    rep["tris_out"] = tris(mesh)
    rep["verts_out"] = len(mesh.vertices)
    rep["dimensions_cm"] = [round(float(x) * 100, 2) for x in (co.max(0) - co.min(0))]
    rep["bbox_centre_cm"] = [round(float(x) * 100, 3) for x in (co.max(0) + co.min(0)) / 2]
    rep["pivot"] = "bounding-box centre"
    rep["materials"] = {"crystal_bright": {"tris": split[0]}, "crystal_dark": {"tris": split[1]}}
    rep["slot_rule"] = f"base colour luminance at face UV centroid, median {thr:.4f}: above = crystal_bright"
    rep["uv_layers"] = [x.name for x in mesh.uv_layers]
    rep["shading"] = "flat"
    rep["texture"] = "not exported (Meshy base colour used only to choose slots)"
    if rep["tris_out"] > 500:
        rep["warnings"].append("tris above 500")

    os.makedirs(os.path.dirname(os.path.abspath(o["out"])), exist_ok=True)
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.ops.export_scene.fbx(filepath=o["out"], use_selection=True, object_types={"MESH"},
        apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE", add_leaf_bones=False,
        path_mode="STRIP", embed_textures=False)
    with open(os.path.splitext(o["out"])[0] + ".report.json", "w", encoding="utf-8") as f:
        json.dump(rep, f, indent=2)
    print(json.dumps(rep))


main()
