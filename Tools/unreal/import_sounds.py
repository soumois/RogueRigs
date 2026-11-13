"""Import (or reimport) WAV files as SoundWave assets, keeping one asset per file name.

Unreal Editor Python, run headless with the PythonScript commandlet, editor closed (task-511):

    UnrealEditor-Cmd.exe <RogueRigs.uproject> -run=pythonscript
        -script="<abs path>/Tools/unreal/import_sounds.py <dest folder> <wav> [<wav> ...]"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

Each <wav> becomes <dest folder>/<file name without extension> (replaced if it exists), saved, and
prints one line starting with "SOUNDIMPORT" with the asset path, duration and channels.
Any failure raises at the end, so the commandlet logs "Python script executed with errors".
"""

import os
import sys

import unreal

LINE_TAG = "SOUNDIMPORT"


def _import(dest, wav):
    name = os.path.splitext(os.path.basename(wav))[0]
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", wav)
    task.set_editor_property("destination_path", dest)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", False)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    path = dest + "/" + name
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not isinstance(asset, unreal.SoundWave):
        return False, path, "no SoundWave after import"
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False):
        return False, path, "save failed"
    return True, path, "duration=%.3f channels=%d" % (
        asset.get_editor_property("duration"), asset.get_editor_property("num_channels"))


def main(argv):
    if len(argv) < 2:
        raise RuntimeError("usage: import_sounds.py <dest folder> <wav> [<wav> ...]")
    dest, files = argv[0], argv[1:]
    failures = 0
    for wav in files:
        ok, path, info = _import(dest, os.path.abspath(wav))
        unreal.log("%s %s %s %s" % (LINE_TAG, path, "ok" if ok else "FAIL", info))
        failures += 0 if ok else 1
    if failures:
        raise RuntimeError("%d sound(s) failed" % failures)


main(sys.argv[1:])
