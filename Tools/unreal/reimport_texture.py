"""Reimport textures in place from the source file stored in their import data, keeping settings.

Unreal Editor Python, run headless with the PythonScript commandlet, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject> -run=pythonscript
        -script="<abs path>/Tools/unreal/reimport_texture.py /Game/Path/T_A /Game/Path/T_B"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

For each asset path: record the texture settings (compression, mips, filter, sRGB, streaming,
address modes, LOD group), reimport through the Interchange manager (fallback: AssetImportTask
over the same asset), write the recorded settings back (a reimport may reset them), save, and
print one line starting with "TEXREIMPORT" with the source, size and settings read back.
Any new asset in the folder counts as a failure.

Any failure raises at the end, so the commandlet logs "Python script executed with errors" and
the process exits non-zero.
"""

import os
import sys

import unreal

LINE_TAG = "TEXREIMPORT"

# Settings kept across the reimport.
KEPT = ("compression_settings", "mip_gen_settings", "filter", "srgb", "never_stream",
        "address_x", "address_y", "lod_group")


def _source_file(tex):
    aid = tex.get_editor_property("asset_import_data")
    if aid is None:
        return ""
    return aid.get_first_filename() or ""


def _folder_assets(package_path):
    folder = package_path.rsplit("/", 1)[0]
    return set(unreal.EditorAssetLibrary.list_assets(folder, recursive=False, include_folder=False))


def _settings(tex):
    return {k: tex.get_editor_property(k) for k in KEPT}


def _fmt_settings(values):
    return " ".join("{}={}".format(k, values[k]) for k in KEPT)


def _reimport_interchange(tex):
    manager = unreal.InterchangeManager.get_interchange_manager_scripted()
    params = unreal.ImportAssetParameters()
    params.set_editor_property("is_automated", True)
    params.set_editor_property("reimport_asset", tex)
    result = manager.reimport_asset(tex, params)
    manager.wait_until_all_tasks_done(False)
    return result[0] if isinstance(result, tuple) else bool(result)


def _reimport_task(package_path, source):
    folder, name = package_path.rsplit("/", 1)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", folder)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("replace_existing_settings", False)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", False)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    return len(task.get_editor_property("imported_object_paths")) > 0


def reimport_one(package_path):
    """Reimport one texture; return (ok, message)."""
    if not unreal.EditorAssetLibrary.does_asset_exist(package_path):
        return False, "asset not found"
    tex = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(tex, unreal.Texture2D):
        return False, "not a Texture2D ({})".format(type(tex).__name__)
    source = _source_file(tex)
    if not source or not os.path.isfile(source):
        return False, "source file missing: '{}'".format(source)

    before_assets = _folder_assets(package_path)
    kept = _settings(tex)
    method = "interchange"
    try:
        ok = _reimport_interchange(tex)
    except Exception as exc:
        unreal.log_warning("{} {} interchange reimport raised: {}".format(LINE_TAG, package_path, exc))
        ok = False
    if not ok:
        method = "import_task"
        if not _reimport_task(package_path, source):
            return False, "both interchange reimport and import task failed (source {})".format(source)

    tex = unreal.EditorAssetLibrary.load_asset(package_path)
    if not isinstance(tex, unreal.Texture2D):
        return False, "asset lost after reimport"
    reset = [k for k in KEPT if tex.get_editor_property(k) != kept[k]]
    if reset:
        tex.modify()
        for k in KEPT:
            tex.set_editor_property(k, kept[k])
        tex.post_edit_change()
    if not unreal.EditorAssetLibrary.save_loaded_asset(tex, only_if_is_dirty=False):
        return False, "save failed"
    added = sorted(_folder_assets(package_path) - before_assets)
    if added:
        return False, "new assets created: {}".format(", ".join(added))
    after = _settings(tex)
    if after != kept:
        return False, "settings differ after restore: {}".format(_fmt_settings(after))
    return True, "method={} source={} size={}x{} {} restored=[{}]".format(
        method, source, tex.blueprint_get_size_x(), tex.blueprint_get_size_y(),
        _fmt_settings(after), ", ".join(reset))


def main(argv):
    paths = [a for a in argv if a.startswith("/")]
    if not paths:
        raise RuntimeError("{}: no asset path given (expected /Game/... arguments)".format(LINE_TAG))
    failures = 0
    for path in paths:
        try:
            ok, message = reimport_one(path)
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
