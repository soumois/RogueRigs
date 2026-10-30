"""Import the Miner skeletal mesh at true cm scale (task-386-03).

Editor Python, run in the open editor: `py "<abs path>/Tools/unreal/import_miner_skeletal.py"`
in the status-bar Cmd box. SkeletalMeshTools.import_file gave a mesh 100x too small, so the legacy
FbxImportUI path is used (new skeleton SKEL_Miner, physics asset created, no animation, no material,
no texture). Existing Miner assets are deleted first. Prints "MINERIMPORT" lines.
"""
import unreal

SRC = "C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Art/Meshes/Miner/SK_Miner.fbx"
DEST = "/Game/RogueRigs/Characters/Miner"

for n in ["SK_Miner", "SK_Miner_Skeleton", "SK_Miner_PhysicsAsset", "SKEL_Miner", "PHYS_Miner"]:
    p = DEST + "/" + n
    if unreal.EditorAssetLibrary.does_asset_exist(p):
        unreal.EditorAssetLibrary.delete_asset(p)

opts = unreal.FbxImportUI()
opts.set_editor_property("import_mesh", True)
opts.set_editor_property("import_as_skeletal", True)
opts.set_editor_property("import_materials", False)
opts.set_editor_property("import_textures", False)
opts.set_editor_property("import_animations", False)
opts.set_editor_property("create_physics_asset", True)
opts.set_editor_property("skeleton", None)
task = unreal.AssetImportTask()
task.set_editor_property("filename", SRC)
task.set_editor_property("destination_path", DEST)
task.set_editor_property("destination_name", "SK_Miner")
task.set_editor_property("replace_existing", True)
task.set_editor_property("automated", True)
task.set_editor_property("save", True)
task.set_editor_property("options", opts)
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

mesh = unreal.EditorAssetLibrary.load_asset(DEST + "/SK_Miner")
if mesh is None:
    unreal.log_error("MINERIMPORT failed")
else:
    b = mesh.get_bounds()
    unreal.log("MINERIMPORT origin=({:.3f},{:.3f},{:.3f}) extent=({:.3f},{:.3f},{:.3f})".format(
        b.origin.x, b.origin.y, b.origin.z, b.box_extent.x, b.box_extent.y, b.box_extent.z))
    unreal.log("MINERIMPORT skeleton={} phys={}".format(mesh.skeleton.get_path_name(),
               mesh.get_editor_property("physics_asset")))
