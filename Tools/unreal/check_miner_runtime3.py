"""Diagnosis task-386-10: in PIE, play RunFwd on the miner mesh (single node) and log the play position over successive calls."""
import unreal

w = unreal.EditorLevelLibrary.get_game_world()
for a in [x for x in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Character) if "Miner" in x.get_class().get_name()]:
    m = a.get_editor_property("mesh")
    ai = m.get_anim_instance()
    if ai.get_class().get_name() != "AnimSingleNodeInstance":
        m.set_editor_property("visibility_based_anim_tick_option", unreal.VisibilityBasedAnimTickOption.ALWAYS_TICK_POSE_AND_REFRESH_BONES)
        m.play_animation(unreal.EditorAssetLibrary.load_asset("/Game/RogueRigs/Characters/Miner/Anims/A_Miner_RunFwd"), True)
        unreal.log("RTCHK3 started single node")
    else:
        t = m.get_socket_transform("RightHand", unreal.RelativeTransformSpace.RTS_COMPONENT)
        unreal.log("RTCHK3 pos={} playing={} hand={:.1f},{:.1f},{:.1f} rendered_recent={}".format(m.get_position(), m.is_playing(), t.translation.x, t.translation.y, t.translation.z, m.was_recently_rendered(1.0)))
