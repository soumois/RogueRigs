"""Rename the Miner skeleton and physics asset to the project names (task-386-03).

Run in the editor: `py "<abs path>/Tools/unreal/rename_miner_assets.py"` in the status-bar Cmd box.
"""
import unreal

D = "/Game/RogueRigs/Characters/Miner/"
for old, new in [("SK_Miner_Skeleton", "SKEL_Miner"), ("SK_Miner_PhysicsAsset", "PHYS_Miner")]:
    if unreal.EditorAssetLibrary.does_asset_exist(D + old):
        ok = unreal.EditorAssetLibrary.rename_asset(D + old, D + new)
        unreal.log("MINERRENAME {} -> {} {}".format(old, new, ok))
unreal.EditorAssetLibrary.save_directory(D)
mesh = unreal.EditorAssetLibrary.load_asset(D + "SK_Miner")
unreal.log("MINERRENAME skeleton={} phys={}".format(mesh.skeleton.get_path_name(),
           mesh.get_editor_property("physics_asset")))
