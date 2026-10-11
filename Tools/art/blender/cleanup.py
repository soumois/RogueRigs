"""Rogue Rigs 3D agent: deterministic mesh cleanup in headless Blender.

Spec: vault nodes `agent-3d` and `asset-pipeline`. Runs without Blender MCP and without UI:

  blender -b --factory-startup -P Tools/art/blender/cleanup.py -- \
      --in Art/Meshes/<asset>/raw/<run>.glb --out Art/Meshes/<asset>/<Name>.fbx \
      [--max-tris N] [--height-cm H] [--pivot bottom|center] [--merge-dist 0.0001] \
      [--min-island 0.002] [--name SM_Name]

Steps: import (glb, gltf, fbx, obj) -> keep meshes only, apply transforms, join ->
merge vertices closer than --merge-dist (metres) -> delete islands smaller than
--min-island x total faces -> recalculate normals outward -> decimate to --max-tris ->
flat shading (visible facets) -> uniform scale to --height-cm -> pivot -> FBX export
in centimetres for Unreal. Writes <out>.report.json next to the FBX.
Exit 0 = exported; 1 = processing error; 2 = usage error. Never imports into Unreal.
"""

import json
import os
import sys

import bpy  # first: bmesh is only importable once bpy is loaded
import bmesh


def fail(code, message):
    print(f"error: {message}", file=sys.stderr)
    sys.exit(code)


def parse_args():
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    opts = {"pivot": "bottom", "merge-dist": "0.0001", "min-island": "0.002"}
    i = 0
    while i < len(argv):
        if not argv[i].startswith("--") or i + 1 >= len(argv):
            fail(2, f"bad argument near {argv[i]}")
        opts[argv[i][2:]] = argv[i + 1]
        i += 2
    for required in ("in", "out"):
        if required not in opts:
            fail(2, f"--{required} is required")
    if opts["pivot"] not in ("bottom", "center"):
        fail(2, "--pivot must be bottom or center")
    if not os.path.isfile(opts["in"]):
        fail(2, f"input not found: {opts['in']}")
    return opts


def import_file(path):
    ext = os.path.splitext(path)[1].lower()
    if ext in (".glb", ".gltf"):
        bpy.ops.import_scene.gltf(filepath=path)
    elif ext == ".fbx":
        bpy.ops.import_scene.fbx(filepath=path)
    elif ext == ".obj":
        bpy.ops.wm.obj_import(filepath=path)
    else:
        fail(2, f"unsupported input format: {ext}")


def island_groups(bm):
    """Faces grouped by connectivity (one list per loose part)."""
    seen, groups = set(), []
    for face in bm.faces:
        if face.index in seen:
            continue
        group, stack = [], [face]
        seen.add(face.index)
        while stack:
            f = stack.pop()
            group.append(f)
            for edge in f.edges:
                for other in edge.link_faces:
                    if other.index not in seen:
                        seen.add(other.index)
                        stack.append(other)
        groups.append(group)
    return groups


def tri_count(mesh):
    return sum(len(p.vertices) - 2 for p in mesh.polygons)


def main():
    o = parse_args()
    report = {"input": o["in"], "output": o["out"], "warnings": []}

    bpy.ops.wm.read_factory_settings(use_empty=True)
    import_file(o["in"])

    meshes = [ob for ob in bpy.context.scene.objects if ob.type == "MESH"]
    if not meshes:
        fail(1, "no mesh in the input file")
    dropped = [ob.type for ob in bpy.context.scene.objects if ob.type != "MESH"]
    if dropped:
        report["warnings"].append(f"dropped non-mesh objects: {sorted(set(dropped))}")

    bpy.ops.object.select_all(action="DESELECT")
    for ob in meshes:
        ob.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    bpy.ops.object.parent_clear(type="CLEAR_KEEP_TRANSFORM")
    for ob in [ob for ob in bpy.context.scene.objects if ob.type != "MESH"]:
        bpy.data.objects.remove(ob, do_unlink=True)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    if len(meshes) > 1:
        bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    obj.name = o.get("name") or os.path.splitext(os.path.basename(o["out"]))[0]
    mesh = obj.data
    report["tris_in"] = tri_count(mesh)

    bm = bmesh.new()
    bm.from_mesh(mesh)
    before = len(bm.verts)
    bmesh.ops.remove_doubles(bm, verts=bm.verts, dist=float(o["merge-dist"]))
    report["verts_merged"] = before - len(bm.verts)
    bm.faces.ensure_lookup_table()
    groups = island_groups(bm)
    report["islands_in"] = len(groups)
    limit = float(o["min-island"]) * len(bm.faces)
    small = [f for g in groups if len(g) < limit for f in g]
    if small:
        bmesh.ops.delete(bm, geom=small, context="FACES")
    report["faces_removed_small_islands"] = len(small)
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    bm.to_mesh(mesh)
    bm.free()

    max_tris = int(o["max-tris"]) if "max-tris" in o else None
    tris = tri_count(mesh)
    if max_tris and tris > max_tris:
        mod = obj.modifiers.new("Decimate", "DECIMATE")
        mod.decimate_type = "COLLAPSE"
        mod.ratio = max_tris / tris
        bpy.ops.object.modifier_apply(modifier=mod.name)
    elif max_tris is None:
        report["warnings"].append("no --max-tris given: polygon budget not enforced")

    for poly in mesh.polygons:
        poly.use_smooth = False
    if hasattr(mesh, "has_custom_normals") and mesh.has_custom_normals:
        bpy.ops.mesh.customdata_custom_splitnormals_clear()

    def bounds():
        xs, ys, zs = zip(*[v.co[:] for v in mesh.vertices])
        return (min(xs), min(ys), min(zs)), (max(xs), max(ys), max(zs))

    lo, hi = bounds()
    if "height-cm" in o:
        height_m = hi[2] - lo[2]
        if height_m <= 0:
            fail(1, "mesh has zero height")
        s = (float(o["height-cm"]) / 100.0) / height_m
        for v in mesh.vertices:
            v.co *= s
        lo, hi = bounds()
    else:
        report["warnings"].append("no --height-cm given: scale left as generated")

    cx, cy = (lo[0] + hi[0]) / 2, (lo[1] + hi[1]) / 2
    cz = lo[2] if o["pivot"] == "bottom" else (lo[2] + hi[2]) / 2
    for v in mesh.vertices:
        v.co.x -= cx
        v.co.y -= cy
        v.co.z -= cz
    obj.location = (0, 0, 0)
    mesh.update()
    lo, hi = bounds()

    report["tris_out"] = tri_count(mesh)
    report["verts_out"] = len(mesh.vertices)
    report["dimensions_cm"] = [round((hi[i] - lo[i]) * 100, 1) for i in range(3)]
    report["pivot"] = o["pivot"]
    report["materials"] = [m.name for m in mesh.materials if m]
    report["uv_layers"] = [uv.name for uv in mesh.uv_layers]
    report["object_name"] = obj.name

    os.makedirs(os.path.dirname(os.path.abspath(o["out"])), exist_ok=True)
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.ops.export_scene.fbx(
        filepath=o["out"], use_selection=True, object_types={"MESH"},
        apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
        add_leaf_bones=False, path_mode="COPY", embed_textures=False,
    )
    with open(os.path.splitext(o["out"])[0] + ".report.json", "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2)
    print(json.dumps({k: report[k] for k in ("output", "tris_in", "tris_out", "dimensions_cm", "warnings")}))


if __name__ == "__main__":
    try:
        main()
    except SystemExit:
        raise
    except Exception as exc:  # report the failure, never leave a half-written FBX unannounced
        fail(1, f"{type(exc).__name__}: {exc}")
