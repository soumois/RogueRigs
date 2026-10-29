"""Import the extractor body and four rotor FBX at true cm scale (task-369-01).

Editor Python, run in the open editor: `py "<abs path>/Tools/unreal/import_extractor_meshes.py"`
in the status-bar Cmd box. StaticMeshTools.import_file gives a mesh 100x too small, so the legacy
FbxImportUI path is used (no material, no texture import, combined meshes). An existing asset of the
same name is deleted first. Prints one "EXTRIMPORT" line per mesh with bounds and triangle count.
"""
import unreal

SRC = "C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Art/Meshes/extractor/"
DEST = "/Game/RogueRigs/Art/Extractor"
NAMES = ["SM_Extractor_Body", "SM_Extractor_Rotor_FL", "SM_Extractor_Rotor_FR",
         "SM_Extractor_Rotor_RL", "SM_Extractor_Rotor_RR"]

for name in NAMES:
    path = DEST + "/" + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.EditorAssetLibrary.delete_asset(path)
    opts = unreal.FbxImportUI()
    opts.set_editor_property("import_mesh", True)
    opts.set_editor_property("import_materials", False)
    opts.set_editor_property("import_textures", False)
    opts.set_editor_property("import_as_skeletal", False)
    opts.static_mesh_import_data.set_editor_property("combine_meshes", True)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", SRC + name + ".fbx")
    task.set_editor_property("destination_path", DEST)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    task.set_editor_property("options", opts)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.EditorAssetLibrary.load_asset(path)
    if mesh is None:
        unreal.log_error("EXTRIMPORT {} failed".format(name))
        continue
    b = mesh.get_bounding_box()
    unreal.log("EXTRIMPORT {} min=({:.3f},{:.3f},{:.3f}) max=({:.3f},{:.3f},{:.3f})".format(
        name, b.min.x, b.min.y, b.min.z, b.max.x, b.max.y, b.max.z))
