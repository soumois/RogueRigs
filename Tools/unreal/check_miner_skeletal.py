"""Read back the Miner skeletal mesh (task-386-03): bones, bounds, triangles, materials."""
import unreal

D = "/Game/RogueRigs/Characters/Miner/"
mesh = unreal.EditorAssetLibrary.load_asset(D + "SK_Miner")
b = mesh.get_bounds()
unreal.log("MINERCHECK height={:.2f} extent=({:.2f},{:.2f},{:.2f})".format(
    b.box_extent.z * 2, b.box_extent.x, b.box_extent.y, b.box_extent.z))
unreal.log("MINERCHECK skeleton={}".format(mesh.skeleton.get_path_name()))
for m in mesh.get_editor_property("materials"):
    unreal.log("MINERCHECK slot={} mat={}".format(m.material_slot_name, m.material_interface.get_path_name()))
try:
    sub = unreal.get_editor_subsystem(unreal.SkeletalMeshEditorSubsystem)
    info = sub.get_lod_build_settings(mesh, 0) if hasattr(sub, "get_lod_build_settings") else None
    unreal.log("MINERCHECK lodcount={} numtris={}".format(sub.get_lod_count(mesh), sub.get_num_triangles(mesh, 0) if hasattr(sub, "get_num_triangles") else "n/a"))
except Exception as e:
    unreal.log("MINERCHECK tri error {}".format(e))
