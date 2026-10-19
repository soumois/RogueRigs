"""Read-only report of the UV channels and vertex colours of static meshes.

Unreal Editor Python, run in the headless editor, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/check_mesh_channels.py /Game/Path/SM_A /Game/Path/SM_B"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

For each static mesh and for both LOD 0 sources (SOURCE_MODEL, the imported mesh description, and
RENDER_DATA, what the renderer draws after the build), copy the mesh into a dynamic mesh with
Geometry Script and print lines starting with "MESHCHANNELS":
- one line per UV channel: U and V range over every triangle corner and the number of distinct
  values (rounded to 1e-4), so a channel overwritten by a lightmap generation shows up when two
  runs are compared;
- one vertex colour line: whether the colour attribute exists, the corner count per colour class
  (RGB rounded to 0.25), and the number of non-white corners.

Written for task-230-20 (rock slab reimport with vertex colours, see tool-unreal-reimport). The
asset is never modified or saved. A mesh that cannot be read raises at the end.
"""

import sys

import unreal

LINE_TAG = "MESHCHANNELS"
ROUND = 4  # decimals for the distinct UV values


def _fmt(v):
    return "({:.2f}, {:.2f}, {:.2f})".format(v.x, v.y, v.z)


def _copy(mesh, lod_type):
    dyn = unreal.DynamicMesh()
    options = unreal.GeometryScriptCopyMeshFromAssetOptions()
    lod = unreal.GeometryScriptMeshReadLOD()
    lod.set_editor_property("lod_type", lod_type)
    lod.set_editor_property("lod_index", 0)
    result = unreal.GeometryScript_AssetUtils.copy_mesh_from_static_mesh(mesh, dyn, options, lod)
    outcome = result[1] if isinstance(result, tuple) else None
    if outcome is not None and outcome != unreal.GeometryScriptOutcomePins.SUCCESS:
        raise RuntimeError("copy_mesh_from_static_mesh outcome {}".format(outcome))
    return dyn


def _triangle_ids(dyn):
    queries = unreal.GeometryScript_MeshQueries
    num_ids = getattr(queries, "get_num_triangle_i_ds", None) or getattr(queries, "get_num_triangle_ids")
    return [t for t in range(num_ids(dyn)) if queries.is_valid_triangle_id(dyn, t)]


def _uv_lines(dyn, tris):
    lines = []
    queries = unreal.GeometryScript_MeshQueries
    for channel in range(queries.get_num_uv_sets(dyn)):
        us, vs = [], []
        for tid in tris:
            res = queries.get_triangle_u_vs(dyn, channel, tid)
            for uv in [r for r in res if isinstance(r, unreal.Vector2D)][:3]:
                us.append(uv.x)
                vs.append(uv.y)
        if not us:
            lines.append("uv{} empty".format(channel))
            continue
        lines.append("uv{} u=[{:.4f}, {:.4f}] v=[{:.4f}, {:.4f}] distinct_u={} distinct_v={}".format(
            channel, min(us), max(us), min(vs), max(vs),
            len(set(round(u, ROUND) for u in us)), len(set(round(v, ROUND) for v in vs))))
    return lines


def _colour_line(dyn, tris):
    queries = unreal.GeometryScript_MeshQueries
    if not queries.get_has_vertex_colors(dyn):
        return "vertex_colours=absent"
    hist = {}
    non_white = 0
    for tid in tris:
        res = queries.get_triangle_vertex_colors(dyn, tid)
        for c in [r for r in res if isinstance(r, unreal.LinearColor)][:3]:
            key = tuple(round(x * 4) / 4 for x in (c.r, c.g, c.b))
            hist[key] = hist.get(key, 0) + 1
            if key != (1.0, 1.0, 1.0):
                non_white += 1
    classes = ", ".join("({:g},{:g},{:g}):{}".format(k[0], k[1], k[2], hist[k]) for k in sorted(hist))
    return "vertex_colours=present non_white_corners={} classes={{{}}}".format(non_white, classes)


def check_one(package_path):
    mesh = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(mesh, unreal.StaticMesh):
        raise RuntimeError("not a StaticMesh or not found")
    box = mesh.get_bounding_box()
    nanite = mesh.get_editor_property("nanite_settings").get_editor_property("enabled")
    slots = ["{}={}".format(m.material_slot_name,
                            m.material_interface.get_name() if m.material_interface else "None")
             for m in mesh.static_materials]
    ext = (mesh.get_editor_property("positive_bounds_extension"),
           mesh.get_editor_property("negative_bounds_extension"))
    lines = ["bounds_extension=+{} -{}".format(_fmt(ext[0]), _fmt(ext[1]))]
    lines += ["bounds_min=({:.2f}, {:.2f}, {:.2f}) bounds_max=({:.2f}, {:.2f}, {:.2f}) nanite={} slots=[{}]".format(
        box.min.x, box.min.y, box.min.z, box.max.x, box.max.y, box.max.z, nanite, ", ".join(slots))]
    for name, lod_type in (("source", unreal.GeometryScriptLODType.SOURCE_MODEL),
                           ("render", unreal.GeometryScriptLODType.RENDER_DATA)):
        dyn = _copy(mesh, lod_type)
        tris = _triangle_ids(dyn)
        gbox = unreal.GeometryScript_MeshQueries.get_mesh_bounding_box(dyn)
        lines.append("{} triangles={} geometry_min=({:.2f}, {:.2f}, {:.2f}) geometry_max=({:.2f}, {:.2f}, {:.2f})".format(
            name, len(tris), gbox.min.x, gbox.min.y, gbox.min.z, gbox.max.x, gbox.max.y, gbox.max.z))
        lines += ["{} {}".format(name, t) for t in _uv_lines(dyn, tris)]
        lines.append("{} {}".format(name, _colour_line(dyn, tris)))
    return lines


def main(argv):
    paths = [a for a in argv if a.startswith("/")]
    if not paths:
        raise RuntimeError("{}: no asset path given (expected /Game/... arguments)".format(LINE_TAG))
    failures = 0
    for path in paths:
        try:
            for text in check_one(path):
                unreal.log("{} {} {}".format(LINE_TAG, path, text))
        except Exception as exc:
            unreal.log_error("{} {} error {}".format(LINE_TAG, path, exc))
            failures += 1
    if failures:
        raise RuntimeError("{}: {} of {} mesh(es) failed".format(LINE_TAG, failures, len(paths)))
    unreal.log("{}: {} mesh(es) read".format(LINE_TAG, len(paths)))


main(sys.argv[1:])
