"""Build BS_Miner_Locomotion (task-386-17): 2D blend space, X=MoveX and Y=MoveY from -1 to 1, four samples.
MoveX forward(+)/back(-), MoveY right(+)/left(-), vector length 1 = max speed (URRMinerAnimInstance)."""
import unreal
D = "/Game/RogueRigs/Characters/Miner/"
A = D + "Anims/"
NAME = "BS_Miner_Locomotion"
PATH = A + NAME
skel = unreal.EditorAssetLibrary.load_asset(D + "SKEL_Miner")
if unreal.EditorAssetLibrary.does_asset_exist(PATH):
    unreal.EditorAssetLibrary.delete_asset(PATH)
f = unreal.BlendSpaceFactoryNew()
f.set_editor_property("target_skeleton", skel)
bs = unreal.AssetToolsHelpers.get_asset_tools().create_asset(NAME, A[:-1], unreal.BlendSpace, f)

def bp(name, lo, hi):
    p = unreal.BlendParameter()
    p.set_editor_property("display_name", name)
    p.set_editor_property("min", lo)
    p.set_editor_property("max", hi)
    p.set_editor_property("grid_num", 4)
    return p
bs.set_editor_property("blend_parameters", [bp("MoveX", -1.0, 1.0), bp("MoveY", -1.0, 1.0), bp("None", 0.0, 100.0)])

S = [("RunFwd", 1, 0), ("RunBwd", -1, 0), ("RunLeft", 0, -1), ("RunRight", 0, 1)]
samples = []
for clip, x, y in S:
    smp = unreal.BlendSample()
    smp.set_editor_property("animation", unreal.EditorAssetLibrary.load_asset(A + "A_Miner_" + clip))
    smp.set_editor_property("sample_value", unreal.Vector(x, y, 0))
    smp.set_editor_property("rate_scale", 1.0)
    samples.append(smp)
bs.set_editor_property("sample_data", samples)
unreal.EditorAssetLibrary.save_asset(PATH)
