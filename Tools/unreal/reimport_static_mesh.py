"""Reimport static meshes in place from the source file stored in their import data.

Unreal Editor Python, run headless with the PythonScript commandlet, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject> -run=pythonscript
        -script="<abs path>/Tools/unreal/reimport_static_mesh.py /Game/Path/SM_A /Game/Path/SM_B"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

For each asset path: load it, reimport it through the Interchange manager (the engine path
behind the Content Browser "Reimport": same asset, same slots, references kept), fall back to an
AssetImportTask over the same asset (replace_existing, no material import, combined meshes),
save it, and print one line starting with "REIMPORT" with bounds, slot names and whether they
changed. The asset folder is listed before and after: any new asset counts as a failure.

unreal.EditorReimportHandler does not exist in the UE 5.8 Python API; InterchangeManager
.reimport_asset is used instead. When the asset stores no translator settings, the FBX
translator is pinned to convert_scene_unit False (see _ensure_translator_settings), otherwise
Blender FBX come back 100 times larger.

Option --vertex-colors=replace (task-230-20): skip Interchange and use the AssetImportTask
directly with VertexColorImportOption Replace (the FBX vertex colours are written into the mesh),
Generate Lightmap UVs off and Nanite kept as the asset had it. Without it, the legacy FbxImportUI
path keeps the importer default (Ignore for the project rock meshes, so the colours read white).
Rerun static_mesh_build_settings.py after any import_task reimport.

Any failure raises at the end, so the commandlet logs "Python script executed with errors" and
the process exits non-zero.
"""

import os
import sys

import unreal

LINE_TAG = "REIMPORT"
VERTEX_COLOR_OPTIONS = {"replace": "REPLACE", "ignore": "IGNORE", "override": "OVERRIDE"}


def _fmt_vec(v):
    return "({:.2f}, {:.2f}, {:.2f})".format(v.x, v.y, v.z)


def _describe(mesh):
    """Return 'bounds=... slots=[...]' for a static mesh."""
    box = mesh.get_bounding_box()
    slots = [str(m.material_slot_name) for m in mesh.static_materials]
    return "bounds_min={} bounds_max={} slots=[{}]".format(
        _fmt_vec(box.min), _fmt_vec(box.max), ", ".join(slots))


def _source_file(mesh):
    """First source file recorded in the asset import data, or empty string."""
    aid = mesh.get_editor_property("asset_import_data")
    if aid is None:
        return ""
    return aid.get_first_filename() or ""


def _folder_assets(package_path):
    folder = package_path.rsplit("/", 1)[0]
    return set(unreal.EditorAssetLibrary.list_assets(folder, recursive=False, include_folder=False))


def _ensure_translator_settings(mesh):
    """Pin the FBX translator settings used by the first import when the asset stores none.

    Project meshes were first imported through FbxImportUI (MCP StaticMeshTools.import_file),
    whose legacy defaults do not convert the scene unit. Without stored settings, a headless
    Interchange reimport falls back to the Interchange defaults (convert_scene_unit true) and
    scales Blender FBX (UnitScaleFactor 100) by 100. Returns 'stored' or 'legacy'.
    """
    aid = mesh.get_editor_property("asset_import_data")
    if aid.get_translator_settings() is not None:
        return "stored"
    settings = unreal.InterchangeFbxTranslatorSettings()
    settings.set_editor_property("convert_scene", True)
    settings.set_editor_property("force_front_x_axis", False)
    settings.set_editor_property("convert_scene_unit", False)
    aid.set_translator_settings(settings)
    return "legacy"


def _reimport_interchange(mesh):
    """Content Browser equivalent: Interchange reimport from the recorded source and pipeline."""
    manager = unreal.InterchangeManager.get_interchange_manager_scripted()
    params = unreal.ImportAssetParameters()
    params.set_editor_property("is_automated", True)
    params.set_editor_property("reimport_asset", mesh)
    result = manager.reimport_asset(mesh, params)
    manager.wait_until_all_tasks_done(False)
    # Out-param binding: (bool, objects) or bool depending on the generated signature.
    ok = result[0] if isinstance(result, tuple) else bool(result)
    return ok


def _reimport_task(mesh, package_path, source, vertex_colors=None):
    """Fallback: import the source file over the existing asset.

    vertex_colors: None keeps the importer default, otherwise a VERTEX_COLOR_OPTIONS key; it also
    turns Generate Lightmap UVs off and keeps the Nanite flag of the existing asset.
    """
    folder, name = package_path.rsplit("/", 1)
    options = unreal.FbxImportUI()
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_materials", False)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("import_as_skeletal", False)
    options.static_mesh_import_data.set_editor_property("combine_meshes", True)
    if vertex_colors:
        smid = options.static_mesh_import_data
        smid.set_editor_property("vertex_color_import_option",
                                 getattr(unreal.VertexColorImportOption, VERTEX_COLOR_OPTIONS[vertex_colors]))
        smid.set_editor_property("generate_lightmap_u_vs", False)
        nanite = mesh.get_editor_property("nanite_settings").get_editor_property("enabled")
        smid.set_editor_property("build_nanite", bool(nanite))

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", folder)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    return len(task.get_editor_property("imported_object_paths")) > 0


def reimport_one(package_path, vertex_colors=None):
    """Reimport one asset; return (ok, message)."""
    if not unreal.EditorAssetLibrary.does_asset_exist(package_path):
        return False, "asset not found"
    mesh = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(mesh, unreal.StaticMesh):
        return False, "not a StaticMesh ({})".format(type(mesh).__name__)
    source = _source_file(mesh)
    if not source or not os.path.isfile(source):
        return False, "source file missing: '{}'".format(source)

    before = _folder_assets(package_path)
    before_desc = _describe(mesh)
    method = "interchange"
    ok = False
    if not vertex_colors:
        try:
            method = "interchange({})".format(_ensure_translator_settings(mesh))
            ok = _reimport_interchange(mesh)
        except Exception as exc:  # binding or Interchange failure: use the fallback
            unreal.log_warning("{} {} interchange reimport raised: {}".format(LINE_TAG, package_path, exc))
            ok = False
    if not ok:
        method = "import_task" + ("(vertex_colors={})".format(vertex_colors) if vertex_colors else "")
        if not _reimport_task(mesh, package_path, source, vertex_colors):
            return False, "both interchange reimport and import task failed (source {})".format(source)

    mesh = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(mesh, unreal.StaticMesh):
        return False, "asset lost after reimport"
    if not unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False):
        return False, "save failed"
    added = sorted(_folder_assets(package_path) - before)
    if added:
        return False, "new assets created: {}".format(", ".join(added))
    after_desc = _describe(mesh)
    same = "unchanged" if after_desc == before_desc else "CHANGED (before: {})".format(before_desc)
    return True, "method={} source={} {} {}".format(method, source, after_desc, same)


def main(argv):
    paths = [a for a in argv if a.startswith("/")]
    vertex_colors = None
    for a in argv:
        if a.startswith("--vertex-colors="):
            vertex_colors = a.split("=", 1)[1].lower()
            if vertex_colors not in VERTEX_COLOR_OPTIONS:
                raise RuntimeError("{}: --vertex-colors must be one of {}".format(
                    LINE_TAG, ", ".join(sorted(VERTEX_COLOR_OPTIONS))))
    if not paths:
        raise RuntimeError("{}: no asset path given (expected /Game/... arguments)".format(LINE_TAG))
    failures = 0
    for path in paths:
        try:
            ok, message = reimport_one(path, vertex_colors)
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
    unreal.log("{}: {} asset(s) reimported".format(LINE_TAG, len(paths)))


main(sys.argv[1:])
