"""Replace the simple collision of pickup static meshes by one primitive fitted to the body.

Unreal Editor Python, run in the headless editor, editor closed (task-229-01):

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/pickup_simple_collision.py [--analyse] /Game/Path/SM_A:aabb /Game/Path/SM_B:best"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

Each argument is a package path with an optional fit mode after a colon:
- aabb (default): axis-aligned box over the body (the XP nut "rectangle", the big XP cube).
- best: the ore chunk rule. Body AABB, oriented box (Precise fit) and convex hull volumes are
  compared: the oriented box is kept when it is at least OBB_GAIN smaller than the AABB, else
  the AABB; a convex hull only when the chosen box is more than HULL_RATIO times the hull
  volume (a box is then clearly wrong).

The body is the LOD 0 source model without the triangles of the slot named GLOW_DISC_SLOT
(the flat glow disc under the XP pickups, which extends the bounds). The primitive replaces
every existing simple shape (Geometry Script SetStaticMeshCollisionFromMesh run on a box mesh
built at the fitted size, or on the body convex hull, so exactly one shape comes out), the collision trace flag is left
as it is (never "simple as complex"), the asset is saved, then the body setup is read back.

--analyse: print the fits and the current collision only, change and save nothing. A path
outside /Game (engine content, e.g. /Engine/BasicShapes/Cube) is always read only.

One line per mesh: "PICKUPCOLLISION <path> ok|error ..." with the body bounds, the volumes,
the choice and the read-back primitives (type, centre, rotation, full size; a convex is read
through its triangulation: bounds and volume). Raises at the end when a mesh failed; in
-ExecutePythonScript mode the log then shows "Python script executed with errors" but the
process still exits 0, so check the lines.
"""

import sys

import unreal

LINE_TAG = "PICKUPCOLLISION"
GLOW_DISC_SLOT = "glow_disc"
OBB_GAIN = 0.85       # keep the oriented box when its volume <= OBB_GAIN x AABB volume
HULL_RATIO = 2.0      # convex hull only when box volume > HULL_RATIO x hull volume
SIZE_TOLERANCE = 0.05  # cm, read-back size against the fitted size
HULL_FACE_COUNT = 48  # convex hull simplification target (25, the engine default, cut the ore tips by 0.9 cm)


def _fmt(v):
    return "({:.2f}, {:.2f}, {:.2f})".format(v.x, v.y, v.z)


def _fmt_rot(r):
    return "(p {:.2f}, y {:.2f}, r {:.2f})".format(r.pitch, r.yaw, r.roll)


def _body_mesh(mesh):
    """Dynamic mesh of LOD 0 source without the glow disc triangles; (dyn, removed, total)."""
    dyn = unreal.DynamicMesh()
    options = unreal.GeometryScriptCopyMeshFromAssetOptions()
    lod = unreal.GeometryScriptMeshReadLOD()
    lod.set_editor_property("lod_type", unreal.GeometryScriptLODType.SOURCE_MODEL)
    lod.set_editor_property("lod_index", 0)
    result = unreal.GeometryScript_AssetUtils.copy_mesh_from_static_mesh(mesh, dyn, options, lod)
    outcome = result[1] if isinstance(result, tuple) else None
    if outcome is not None and outcome != unreal.GeometryScriptOutcomePins.SUCCESS:
        raise RuntimeError("copy_mesh_from_static_mesh outcome {}".format(outcome))
    total = unreal.GeometryScript_MeshQueries.get_num_triangle_i_ds(dyn)
    removed = 0
    for index, slot in enumerate(mesh.static_materials):
        if str(slot.material_slot_name) == GLOW_DISC_SLOT:
            result = unreal.GeometryScript_Materials.delete_triangles_by_material_id(dyn, index)
            removed += result[1] if isinstance(result, tuple) and len(result) > 1 else 0
    return dyn, removed, total


def _volume(dyn):
    result = unreal.GeometryScript_MeshQueries.get_mesh_volume_area(dyn)
    return float(result[1]) if isinstance(result, tuple) else float("nan")


def _fits(dyn):
    """Body AABB, oriented box and convex hull volume."""
    box = unreal.GeometryScript_MeshQueries.get_mesh_bounding_box(dyn)
    size = box.max - box.min
    aabb = {"center": (box.min + box.max) * 0.5, "rotation": unreal.Rotator(0, 0, 0),
            "size": size, "volume": size.x * size.y * size.z, "min": box.min, "max": box.max}

    selection = unreal.GeometryScriptMeshSelection()
    obb_options = unreal.GeometryScriptFitOrientedBoxOptions()
    fit_enum = getattr(unreal, "GeometryScriptFitOrientedBoxMethod", None) or getattr(unreal, "FGeometryScriptFitOrientedBoxMethod")
    obb_options.set_editor_property("fit_method", fit_enum.PRECISE)
    result = unreal.GeometryScript_Containment.compute_mesh_oriented_box(dyn, selection, obb_options)
    obb_struct = result[1] if isinstance(result, tuple) else result
    ax = obb_struct.get_editor_property("axis_x")
    ay = obb_struct.get_editor_property("axis_y")
    az = ax.cross(ay)  # right-handed frame; the box is symmetric so the axis signs do not matter
    obb_size = unreal.Vector(2.0 * obb_struct.get_editor_property("extent_x"),
                             2.0 * obb_struct.get_editor_property("extent_y"),
                             2.0 * obb_struct.get_editor_property("extent_z"))
    obb = {"center": obb_struct.get_editor_property("center"),
           "rotation": unreal.MathLibrary.make_rotation_from_axes(ax, ay, az),
           "size": obb_size, "volume": obb_size.x * obb_size.y * obb_size.z}

    hull = unreal.DynamicMesh()
    hull_options = unreal.GeometryScriptConvexHullOptions()
    unreal.GeometryScript_Containment.compute_mesh_convex_hull(dyn, hull, selection, hull_options)
    hull_volume = _volume(hull)
    return aabb, obb, hull, hull_volume


def _read_back(mesh):
    """Every simple shape of the body setup, and the trace flag."""
    body = mesh.get_editor_property("body_setup")
    if body is None:
        return "body_setup=None", []
    agg = body.get_editor_property("agg_geom")
    shapes = []
    for b in agg.get_editor_property("box_elems"):
        shapes.append(("box", b.get_editor_property("center"), b.get_editor_property("rotation"),
                       unreal.Vector(b.get_editor_property("x"), b.get_editor_property("y"),
                                     b.get_editor_property("z"))))
    for s in agg.get_editor_property("sphere_elems"):
        r = s.get_editor_property("radius")
        shapes.append(("sphere", s.get_editor_property("center"), unreal.Rotator(0, 0, 0),
                       unreal.Vector(2 * r, 2 * r, 2 * r)))
    for c in agg.get_editor_property("sphyl_elems"):
        shapes.append(("capsule", c.get_editor_property("center"), c.get_editor_property("rotation"),
                       unreal.Vector(c.get_editor_property("radius"), c.get_editor_property("length"), 0)))
    convex = list(agg.get_editor_property("convex_elems"))
    if convex:
        # KConvexElem exposes no property to Python: triangulate the simple collision instead
        # (one convex expected; the bounds and volume cover every convex element)
        simple = unreal.GeometryScript_Collision.get_simple_collision_from_static_mesh(mesh)
        only = unreal.DynamicMesh()
        unreal.GeometryScript_Primitives.append_simple_collision_shapes(
            only, unreal.GeometryScriptPrimitiveOptions(), unreal.Transform(), simple,
            unreal.GeometryScriptSimpleCollisionTriangulationOptions())
        if len(convex) != 1 or agg.get_editor_property("box_elems") or agg.get_editor_property("sphere_elems"):
            only = None  # several or mixed shapes: the bounds would not be one hull's
        if only is not None:
            bb = unreal.GeometryScript_MeshQueries.get_mesh_bounding_box(only)
            tris = unreal.GeometryScript_MeshQueries.get_num_triangle_i_ds(only)
            shapes.append(("convex(tris {}, volume {:.0f})".format(tris, _volume(only)),
                           (bb.min + bb.max) * 0.5, unreal.Rotator(0, 0, 0), bb.max - bb.min))
        else:
            for _ in convex:
                shapes.append(("convex", unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0), unreal.Vector(0, 0, 0)))
    flag = body.get_editor_property("collision_trace_flag")
    text = "trace_flag={} shapes=[{}]".format(flag, "; ".join(
        "{} centre={} rotation={} size={}".format(t, _fmt(c), _fmt_rot(r), _fmt(s)) for t, c, r, s in shapes))
    return text, shapes


def _apply_box(mesh, fit, oriented):
    box_mesh = unreal.DynamicMesh()
    transform = unreal.Transform(fit["center"], fit["rotation"], unreal.Vector(1, 1, 1))
    unreal.GeometryScript_Primitives.append_box(
        box_mesh, unreal.GeometryScriptPrimitiveOptions(), transform,
        fit["size"].x, fit["size"].y, fit["size"].z, 0, 0, 0,
        unreal.GeometryScriptPrimitiveOriginMode.CENTER)
    _apply_from(mesh, box_mesh, unreal.GeometryScriptCollisionGenerationMethod.ORIENTED_BOXES if oriented
                else unreal.GeometryScriptCollisionGenerationMethod.ALIGNED_BOXES)


def _apply_from(mesh, source, method):
    options = unreal.GeometryScriptCollisionFromMeshOptions()
    options.set_editor_property("emit_transaction", False)
    options.set_editor_property("method", method)
    options.set_editor_property("auto_detect_spheres", False)
    options.set_editor_property("auto_detect_boxes", False)
    options.set_editor_property("auto_detect_capsules", False)
    options.set_editor_property("max_convex_hulls_per_mesh", 1)
    options.set_editor_property("convex_hull_target_face_count", HULL_FACE_COUNT)
    unreal.GeometryScript_Collision.set_static_mesh_collision_from_mesh(
        source, mesh, options, unreal.GeometryScriptSetStaticMeshCollisionOptions())


def process_one(path, mode, analyse):
    if not unreal.EditorAssetLibrary.does_asset_exist(path):
        return False, "asset not found"
    mesh = unreal.EditorAssetLibrary.load_asset(path)
    if not isinstance(mesh, unreal.StaticMesh):
        return False, "not a StaticMesh ({})".format(type(mesh).__name__)
    read_only = analyse or not path.startswith("/Game/")
    before, _ = _read_back(mesh)

    dyn, removed, total = _body_mesh(mesh)
    aabb, obb, hull, hull_volume = _fits(dyn)
    body_volume = _volume(dyn)
    info = ("tris={} glow_disc_removed={} body_min={} body_max={} body_size={} body_volume={:.0f} "
            "aabb_volume={:.0f} obb_size={} obb_rotation={} obb_volume={:.0f} hull_volume={:.0f}").format(
        total, removed, _fmt(aabb["min"]), _fmt(aabb["max"]), _fmt(aabb["size"]), body_volume,
        aabb["volume"], _fmt(obb["size"]), _fmt_rot(obb["rotation"]), obb["volume"], hull_volume)

    if mode == "best":
        fit, kind = (obb, "obb") if obb["volume"] <= OBB_GAIN * aabb["volume"] else (aabb, "aabb")
        if hull_volume > 0 and fit["volume"] > HULL_RATIO * hull_volume:
            kind = "convex"
    else:
        fit, kind = aabb, "aabb"
    info += " choice={} box_over_hull={:.2f}".format(
        kind, fit["volume"] / hull_volume if hull_volume > 0 else float("nan"))

    if read_only:
        return True, "{} read_only current: {}".format(info, before)

    if kind == "convex":
        _apply_from(mesh, hull, unreal.GeometryScriptCollisionGenerationMethod.CONVEX_HULLS)
    else:
        _apply_box(mesh, fit, kind == "obb")
    if not unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False):
        return False, "save failed"

    after, shapes = _read_back(mesh)
    if len(shapes) != 1:
        return False, "{} expected one shape, got: {}".format(info, after)
    if kind == "convex":
        if not shapes[0][0].startswith("convex("):
            return False, "{} expected one convex, got: {}".format(info, after)
    else:
        t, centre, _rot, size = shapes[0]
        dims_fit = sorted([fit["size"].x, fit["size"].y, fit["size"].z])
        dims_got = sorted([size.x, size.y, size.z])
        if t != "box" or any(abs(a - b) > SIZE_TOLERANCE for a, b in zip(dims_fit, dims_got)) \
                or (centre - fit["center"]).length() > SIZE_TOLERANCE:
            return False, "{} read-back differs from the fit: {}".format(info, after)
    return True, "{} after: {} (before: {})".format(info, after, before)


def main(argv):
    analyse = "--analyse" in argv
    items = [a for a in argv if a.startswith("/")]
    if not items:
        raise RuntimeError("{}: no asset path given".format(LINE_TAG))
    failures = 0
    for item in items:
        path, _, mode = item.partition(":")
        mode = mode or "aabb"
        try:
            ok, message = process_one(path, mode, analyse)
        except Exception as exc:
            ok, message = False, "exception: {}".format(exc)
        line = "{} {} {} {}".format(LINE_TAG, path, "ok" if ok else "error", message)
        if ok:
            unreal.log(line)
        else:
            unreal.log_error(line)
            failures += 1
    if failures:
        raise RuntimeError("{}: {} of {} mesh(es) failed".format(LINE_TAG, failures, len(items)))
    unreal.log("{}: {} mesh(es) done".format(LINE_TAG, len(items)))


main(sys.argv[1:])
