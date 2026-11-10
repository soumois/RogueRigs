"""Set the material of named slots of static meshes, save, read back (task-499-02, editor closed).

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/set_mesh_slot_materials.py /Game/Path/SM_A:slot=/Game/Path/M_X [/Game/Path/SM_A:slot2=/Game/Path/M_Y ...]"

Each argument is <mesh path>:<slot name>=<material path>. Prints one line per mesh starting with
"SLOTMATS" with every slot and its material after the save. Raises when a slot or asset is missing.
"""
import sys

import unreal

errors = []
meshes = {}
for arg in sys.argv[1:]:
    mesh_path, rest = arg.split(":", 1)
    slot, mat_path = rest.split("=", 1)
    mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
    mat = unreal.EditorAssetLibrary.load_asset(mat_path)
    if mesh is None or mat is None:
        errors.append("missing asset in " + arg)
        continue
    mats = list(mesh.static_materials)
    hit = False
    for i, sm in enumerate(mats):
        if str(sm.material_slot_name) == slot:
            sm.material_interface = mat
            mats[i] = sm
            hit = True
    if not hit:
        errors.append("no slot {} on {}".format(slot, mesh_path))
        continue
    mesh.set_editor_property("static_materials", mats)
    meshes[mesh_path] = mesh
for path, mesh in meshes.items():
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
    m = unreal.EditorAssetLibrary.load_asset(path)
    unreal.log("SLOTMATS {} {}".format(path, ", ".join("{}={}".format(s.material_slot_name, s.material_interface.get_name() if s.material_interface else "None") for s in m.static_materials)))
if errors:
    for e in errors:
        unreal.log_error("SLOTMATS error " + e)
    raise RuntimeError("; ".join(errors))
