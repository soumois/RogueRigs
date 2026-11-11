"""Create the volume groups of the options screen (task-505-01): SC_Master with children SC_Music and SC_Effects, and the
sound mix Mix_Volumes that URRGameUserSettings fills with the slider volumes (URROptionsSettings paths).

Unreal Editor Python, run headless with the PythonScript commandlet, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject> -run=pythonscript
        -script="<abs path>/Tools/unreal/create_volume_sound_classes.py"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

Existing assets are reused (idempotent). Prints one line starting with "RRAUDIO" per asset, then the parent read back.
"""

import unreal

FOLDER = "/Game/RogueRigs/Audio"
LINE_TAG = "RRAUDIO"


def get_or_create(name, asset_class, factory):
    path = f"{FOLDER}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        asset = unreal.EditorAssetLibrary.load_asset(path)
        unreal.log(f"{LINE_TAG} reused {path}")
        return asset
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, FOLDER, asset_class, factory)
    if asset is None:
        raise RuntimeError(f"could not create {path}")
    unreal.log(f"{LINE_TAG} created {path}")
    return asset


master = get_or_create("SC_Master", unreal.SoundClass, unreal.SoundClassFactory())
music = get_or_create("SC_Music", unreal.SoundClass, unreal.SoundClassFactory())
effects = get_or_create("SC_Effects", unreal.SoundClass, unreal.SoundClassFactory())
mix = get_or_create("Mix_Volumes", unreal.SoundMix, unreal.SoundMixFactory())

# ChildClasses is the edited property; its PostEditChange sets the ParentClass of the added child only, so add them one
# at a time.
master.set_editor_property("child_classes", [music])
master.set_editor_property("child_classes", [music, effects])

for asset in (master, music, effects, mix):
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False):
        raise RuntimeError(f"could not save {asset.get_path_name()}")

children = [c.get_name() for c in master.get_editor_property("child_classes")]
unreal.log(f"{LINE_TAG} SC_Master children={children}")
for child in (music, effects):
    try:
        parent = child.get_editor_property("parent_class")
        unreal.log(f"{LINE_TAG} {child.get_name()} parent={parent.get_name() if parent else None}")
    except Exception as error:  # ParentClass may not be exposed to Python
        unreal.log(f"{LINE_TAG} {child.get_name()} parent not readable: {error}")
