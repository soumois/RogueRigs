"""Import the nine Miner in-place clips as AnimSequences on SKEL_Miner (task-386-04).

Editor Python, run in the open editor: `py "<abs path>/Tools/unreal/import_miner_clips.py"`
in the status-bar Cmd box. Legacy FbxImportUI path (MCP import gives 100x scale). Prints "MINERCLIPS" lines.
"""
import unreal

SRC = "C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Art/Meshes/Miner/Anims/"
DEST = "/Game/RogueRigs/Characters/Miner/Anims"
SKEL = "/Game/RogueRigs/Characters/Miner/SKEL_Miner"
NAMES = ["Idle", "RunFwd", "WalkFwd", "RunBwd", "WalkBwd", "RunLeft", "StrafeLeft", "RunRight", "StrafeRight"]

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

for n in NAMES:
    p = DEST + "/A_Miner_" + n
    a = unreal.EditorAssetLibrary.load_asset(p)
    if a is None:
        unreal.log_error("MINERCLIPS missing " + n)
        continue
    for prop, val in (("enable_root_motion", False), ("loop", True)):
        try:
            a.set_editor_property(prop, val)
        except Exception as e:
            unreal.log("MINERCLIPS setprop {} failed {}".format(prop, e))
    unreal.EditorAssetLibrary.save_asset(p)
    try:
        loop = a.get_editor_property("loop")
    except Exception:
        loop = "n/a"
    unreal.log("MINERCLIPS {} len={:.4f} skel={} rootmotion={} loop={} class={}".format(
        n, a.get_play_length(), a.get_editor_property("skeleton").get_path_name(),
        a.get_editor_property("enable_root_motion"), loop, a.get_class().get_name()))
unreal.log("MINERCLIPS assets=" + ",".join(str(x) for x in unreal.EditorAssetLibrary.list_assets("/Game/RogueRigs/Characters/Miner", recursive=True)))
