"""Read-only check of a part index UV channel in the RENDER_DATA of static meshes.

Unreal Editor Python, run in the headless editor, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/check_uv_channel.py /Game/Path/SM_A /Game/Path/SM_B"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

For each static mesh: copy LOD 0 render data into a dynamic mesh with Geometry Script
(copy_mesh_from_static_mesh, RENDER_DATA), read UV channel CHANNEL on every triangle corner and
sort each corner into the bins (i + 0.5) / BINS (U and V both on the bin centre within
TOLERANCE). Prints one line starting with "UVCHECK" per mesh and per material ID with the
triangle count, the corner count per bin, corners off-bin and triangles whose corners fall in
different bins ("mixed"). This is the part index channel written by the chaser Blender bake
(chaser-meshy-pipeline): the engine lightmap generation must never overwrite it.

The asset is never modified or saved. A mesh whose render data cannot be read raises at the end.
"""

import sys

import unreal

LINE_TAG = "UVCHECK"
CHANNEL = 2
BINS = 10
TOLERANCE = 1.0e-3


def _bin_of(uv):
    """Bin index of a UV, or -1 when U or V is off every bin centre.

    The FBX import flips V (v = 1 - v_blender), so V is checked against 1 - centre.
    """
    i = int(round(uv.x * BINS - 0.5))
    centre = (i + 0.5) / BINS
    if 0 <= i < BINS and abs(uv.x - centre) <= TOLERANCE and abs(uv.y - (1.0 - centre)) <= TOLERANCE:
        return i
    return -1


def _fmt_hist(hist):
    return "{" + ", ".join("{:.2f}:{}".format((i + 0.5) / BINS, hist[i])
                           for i in sorted(k for k in hist if k >= 0)) + "}"


def check_one(package_path):
    """Return (ok, lines) for one mesh."""
    mesh = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(mesh, unreal.StaticMesh):
        return False, ["not a StaticMesh or not found"]
    dyn = unreal.DynamicMesh()
    options = unreal.GeometryScriptCopyMeshFromAssetOptions()
    lod = unreal.GeometryScriptMeshReadLOD()
    lod.set_editor_property("lod_type", unreal.GeometryScriptLODType.RENDER_DATA)
    lod.set_editor_property("lod_index", 0)
    result = unreal.GeometryScript_AssetUtils.copy_mesh_from_static_mesh(mesh, dyn, options, lod)
    outcome = result[1] if isinstance(result, tuple) else None
    if outcome is not None and outcome != unreal.GeometryScriptOutcomePins.SUCCESS:
        return False, ["copy_mesh_from_static_mesh outcome {}".format(outcome)]

    num_uv = unreal.GeometryScript_MeshQueries.get_num_uv_sets(dyn)
    if num_uv <= CHANNEL:
        return False, ["render data has {} UV channels, channel {} missing".format(num_uv, CHANNEL)]

    per_mat = {}  # material id -> [triangles, hist, off_bin, mixed]
    queries = unreal.GeometryScript_MeshQueries
    num_ids = getattr(queries, "get_num_triangle_i_ds", None) or getattr(queries, "get_num_triangle_ids")
    for tid in range(num_ids(dyn)):
        if not unreal.GeometryScript_MeshQueries.is_valid_triangle_id(dyn, tid):
            continue
        mat = unreal.GeometryScript_Materials.get_triangle_material_id(dyn, tid)
        mat = mat[0] if isinstance(mat, tuple) else mat
        uvs = unreal.GeometryScript_MeshQueries.get_triangle_u_vs(dyn, CHANNEL, tid)
        entry = per_mat.setdefault(mat, [0, {}, 0, 0, ""])
        entry[0] += 1
        bins = [_bin_of(uv) for uv in uvs[:3]]
        for b in bins:
            if b < 0:
                entry[2] += 1
                if not entry[4]:
                    bad = uvs[bins.index(b)]
                    entry[4] = "({:.4f}, {:.4f})".format(bad.x, bad.y)
            else:
                entry[1][b] = entry[1].get(b, 0) + 1
        if len(set(bins)) > 1:
            entry[3] += 1

    lines = ["uv_channels={} material_ids={}".format(num_uv, sorted(per_mat))]
    ok = True
    for mat in sorted(per_mat):
        tris, hist, off, mixed, first_off = per_mat[mat]
        lines.append("material_id={} triangles={} corners={} bins={} off_bin={}{} mixed={}".format(
            mat, tris, tris * 3, _fmt_hist(hist), off,
            " (first {})".format(first_off) if first_off else "", mixed))
        ok = ok and off == 0 and mixed == 0
    return ok, lines


def main(argv):
    paths = [a for a in argv if a.startswith("/")]
    if not paths:
        raise RuntimeError("{}: no asset path given (expected /Game/... arguments)".format(LINE_TAG))
    failures = 0
    for path in paths:
        try:
            ok, lines = check_one(path)
        except Exception as exc:
            ok, lines = False, ["exception: {}".format(exc)]
        for text in lines:
            line = "{} {} {} {}".format(LINE_TAG, path, "ok" if ok else "FAIL", text)
            if ok:
                unreal.log(line)
            else:
                unreal.log_error(line)
        failures += 0 if ok else 1
    if failures:
        raise RuntimeError("{}: {} of {} mesh(es) failed".format(LINE_TAG, failures, len(paths)))
    unreal.log("{}: {} mesh(es) checked".format(LINE_TAG, len(paths)))


main(sys.argv[1:])
