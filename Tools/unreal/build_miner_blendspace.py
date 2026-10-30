"""Build BS_Miner_Locomotion (task-386-05). Direction -180..180, Speed 0..427.1."""
import unreal
D = "/Game/RogueRigs/Characters/Miner/"
A = D + "Anims/"
NAME = "BS_Miner_Locomotion"
PATH = A + NAME
MAXS = 427.1
skel = unreal.EditorAssetLibrary.load_asset(D + "SKEL_Miner")
if unreal.EditorAssetLibrary.does_asset_exist(PATH):
    unreal.EditorAssetLibrary.delete_asset(PATH)
f = unreal.BlendSpaceFactoryNew()
f.set_editor_property("target_skeleton", skel)
bs = unreal.AssetToolsHelpers.get_asset_tools().create_asset(NAME, A[:-1], unreal.BlendSpace, f)
unreal.log("BSBUILD created {}".format(bs))
try:
    unreal.log("BSBUILD props blend_parameters={}".format(bs.get_editor_property("blend_parameters")))
except Exception as e:
    unreal.log("BSBUILD blend_parameters err {}".format(e))
try:
    unreal.log("BSBUILD sample_data={}".format(bs.get_editor_property("sample_data")))
except Exception as e:
    unreal.log("BSBUILD sample_data err {}".format(e))

def bp(name, lo, hi, grid, wrap):
    p = unreal.BlendParameter()
    p.set_editor_property("display_name", name)
    p.set_editor_property("min", lo)
    p.set_editor_property("max", hi)
    p.set_editor_property("grid_num", grid)
    return p
try:
    params = [bp("Direction", -180.0, 180.0, 4, False), bp("Speed", 0.0, MAXS, 4, False), bp("None", 0.0, 100.0, 4, False)]
    bs.set_editor_property("blend_parameters", params)
except Exception as e:
    unreal.log("BSBUILD params err {}".format(e))

S = [("Idle", -180, 0), ("Idle", -90, 0), ("Idle", 0, 0), ("Idle", 90, 0), ("Idle", 180, 0),
     ("WalkFwd", 0, 224.7), ("RunFwd", 0, 278.8),
     ("StrafeRight", 90, 292.2), ("RunRight", 90, 427.1),
     ("StrafeLeft", -90, 122.7), ("RunLeft", -90, 427.1),
     ("WalkBwd", -180, 88.0), ("RunBwd", -180, 243.7),
     ("WalkBwd", 180, 88.0), ("RunBwd", 180, 243.7)]
try:
    samples = []
    for clip, d, s in S:
        smp = unreal.BlendSample()
        smp.set_editor_property("animation", unreal.EditorAssetLibrary.load_asset(A + "A_Miner_" + clip))
        smp.set_editor_property("sample_value", unreal.Vector(d, s, 0))
        smp.set_editor_property("rate_scale", 1.0)
        samples.append(smp)
    bs.set_editor_property("sample_data", samples)
except Exception as e:
    unreal.log("BSBUILD samples err {}".format(e))
unreal.EditorAssetLibrary.save_asset(PATH)
