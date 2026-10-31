"""Import A_Miner_UpperRifle_V2Grip as AnimSequence on SKEL_Miner (task-409-03; same settings as import_miner_upper_rifle.py).

Editor Python, run in the open editor: `py "<abs path>/Tools/unreal/import_miner_upper_rifle.py"`
in the status-bar Cmd box. Same FbxImportUI settings as import_miner_clips.py. Prints "UPPERGRIP" lines.
"""
import unreal

SRC = "C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Art/Meshes/Miner/Anims/"
DEST = "/Game/RogueRigs/Characters/Miner/Anims"
SKEL = "/Game/RogueRigs/Characters/Miner/SKEL_Miner"
NAMES = ["UpperRifle_V2Grip"]

skel = unreal.EditorAssetLibrary.load_asset(SKEL)
tasks = []
for n in NAMES:
    opts = unreal.FbxImportUI()
    opts.set_editor_property("import_mesh", False)
    opts.set_editor_property("import_as_skeletal", False)
    opts.set_editor_property("import_materials", False)
    opts.set_editor_property("import_textures", False)
    opts.set_editor_property("import_animations", True)
    opts.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_ANIMATION)
    opts.set_editor_property("skeleton", skel)
    t = unreal.AssetImportTask()
    t.set_editor_property("filename", SRC + "A_Miner_" + n + ".fbx")
    t.set_editor_property("destination_path", DEST)
    t.set_editor_property("destination_name", "A_Miner_" + n)
    t.set_editor_property("replace_existing", True)
    t.set_editor_property("automated", True)
    t.set_editor_property("save", False)
    t.set_editor_property("options", opts)
    tasks.append(t)
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)

for n in ["UpperRifle_V2"] + NAMES:
    p = DEST + "/A_Miner_" + n
    a = unreal.EditorAssetLibrary.load_asset(p)
    if a is None:
        unreal.log_error("UPPERGRIP missing " + n)
        continue
    if n in NAMES:
        for prop, val in (("enable_root_motion", False), ("loop_animation" if False else "rate_scale", 1.0)):
            try:
                a.set_editor_property(prop, val)
            except Exception as e:
                unreal.log("UPPERGRIP setprop {} failed {}".format(prop, e))
        try:
            a.set_editor_property("loop", True)
        except Exception as e:
            unreal.log("UPPERGRIP loop failed {}".format(e))
        unreal.EditorAssetLibrary.save_asset(p)
    try:
        loop = a.get_editor_property("loop")
    except Exception:
        loop = "n/a"
    unreal.log("UPPERGRIP {} len={:.4f} frames={} skel={} rootmotion={} loop={} class={}".format(
        n, a.get_play_length(), unreal.AnimationLibrary.get_num_frames(a),
        a.get_editor_property("skeleton").get_path_name(),
        a.get_editor_property("enable_root_motion"), loop, a.get_class().get_name()))
