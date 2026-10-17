"""Set the LOD 0 build settings that keep an extra UV channel intact, rebuild and save.

Unreal Editor Python. Run in the headless editor, editor closed (the PythonScript commandlet
applies nothing, see art-3d-lessons):

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/static_mesh_build_settings.py /Game/Path/SM_A"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

For each static mesh path: Generate Lightmap UVs off (the engine would write a lightmap into
an extra UV channel of the render data, e.g. the PartId channel of the animated chasers),
full precision UVs on, LightMapCoordinateIndex 1 (the LightMap channel of the Blender export),
rebuild, save, then read the settings back and print one line starting with "BUILDSETTINGS"
with the settings, bounds and the material of every slot. The editor quits after the script.
"""

import sys

import unreal

LINE_TAG = "BUILDSETTINGS"
LIGHTMAP_COORDINATE_INDEX = 1


def _fmt_vec(v):
    return "({:.2f}, {:.2f}, {:.2f})".format(v.x, v.y, v.z)


def _read_back(subsystem, mesh):
    build = subsystem.get_lod_build_settings(mesh, 0)
    box = mesh.get_bounding_box()
    slots = ["{}={}".format(m.material_slot_name,
                            m.material_interface.get_name() if m.material_interface else "None")
             for m in mesh.static_materials]
    return ("generate_lightmap_u_vs={} use_full_precision_u_vs={} light_map_coordinate_index={} "
            "bounds_min={} bounds_max={} slots=[{}]").format(
        build.get_editor_property("generate_lightmap_u_vs"),
        build.get_editor_property("use_full_precision_u_vs"),
        mesh.get_editor_property("light_map_coordinate_index"),
        _fmt_vec(box.min), _fmt_vec(box.max), ", ".join(slots))


def apply_one(subsystem, package_path):
    """Apply the settings to one mesh; return (ok, message)."""
    if not unreal.EditorAssetLibrary.does_asset_exist(package_path):
        return False, "asset not found"
    mesh = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(mesh, unreal.StaticMesh):
        return False, "not a StaticMesh ({})".format(type(mesh).__name__)
    before = _read_back(subsystem, mesh)

    build = subsystem.get_lod_build_settings(mesh, 0)
    build.set_editor_property("generate_lightmap_u_vs", False)
    build.set_editor_property("use_full_precision_u_vs", True)
    mesh.modify()
    mesh.set_editor_property("light_map_coordinate_index", LIGHTMAP_COORDINATE_INDEX)
    subsystem.set_lod_build_settings(mesh, 0, build)  # rebuilds the render data
    if not unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False):
        return False, "save failed"

    after = _read_back(subsystem, mesh)
    build = subsystem.get_lod_build_settings(mesh, 0)
    if (build.get_editor_property("generate_lightmap_u_vs")
            or not build.get_editor_property("use_full_precision_u_vs")
            or mesh.get_editor_property("light_map_coordinate_index") != LIGHTMAP_COORDINATE_INDEX):
        return False, "settings not applied: {}".format(after)
    return True, "{} (before: {})".format(after, before)


def main(argv):
    paths = [a for a in argv if a.startswith("/")]
    if not paths:
        raise RuntimeError("{}: no asset path given (expected /Game/... arguments)".format(LINE_TAG))
    subsystem = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
    failures = 0
    for path in paths:
        try:
            ok, message = apply_one(subsystem, path)
        except Exception as exc:
            ok, message = False, "exception: {}".format(exc)
        line = "{} {} {} {}".format(LINE_TAG, path, "ok" if ok else "error", message)
        if ok:
            unreal.log(line)
        else:
            unreal.log_error(line)
            failures += 1
    if failures:
        raise RuntimeError("{}: {} of {} asset(s) failed".format(LINE_TAG, failures, len(paths)))
    unreal.log("{}: {} asset(s) done".format(LINE_TAG, len(paths)))


main(sys.argv[1:])
