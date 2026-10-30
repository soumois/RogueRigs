"""Diagnosis task-386-10: set the engine default bone compression on A_Miner_RunFwd through AnimationLibrary (triggers recompress)."""
import unreal

a = unreal.EditorAssetLibrary.load_asset("/Game/RogueRigs/Characters/Miner/Anims/A_Miner_RunFwd")
s = unreal.EditorAssetLibrary.load_asset("/Engine/Animation/DefaultAnimBoneCompressionSettings")
unreal.log("ANIMCHK5 before={} new={}".format(unreal.AnimationLibrary.get_bone_compression_settings(a), s))
unreal.AnimationLibrary.set_bone_compression_settings(a, s)
unreal.log("ANIMCHK5 after={}".format(unreal.AnimationLibrary.get_bone_compression_settings(a)))
