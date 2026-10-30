"""Diagnosis task-386-10: in PIE, read the miner mesh anim mode, anim class, anim instance variables and a bone pose."""
import unreal

w = unreal.EditorLevelLibrary.get_game_world()
unreal.log("RTCHK world={}".format(w.get_name() if w else None))
for a in [x for x in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Character) if "Miner" in x.get_class().get_name()]:
    m = a.get_editor_property("mesh")
    unreal.log("RTCHK actor={} class={} mesh_asset={} anim_mode={} anim_class={}".format(
        a.get_name(), a.get_class().get_name(),
        m.get_editor_property("skeletal_mesh_asset").get_name() if m.get_editor_property("skeletal_mesh_asset") else None,
        m.get_editor_property("animation_mode"), m.get_editor_property("anim_class")))
    ai = m.get_anim_instance()
    unreal.log("RTCHK   anim_instance={}".format(ai.get_class().get_name() if ai else None))
    if ai:
        for p in ["ground_speed", "direction", "blend_speed", "locomotion_play_rate"]:
            try:
                unreal.log("RTCHK   {}={}".format(p, ai.get_editor_property(p)))
            except Exception as e:
                unreal.log("RTCHK   {} err {}".format(p, e))
    for sock in ["hand_r", "RightHand", "Head"]:
        try:
            t = m.get_socket_transform(sock, unreal.RelativeTransformSpace.RTS_COMPONENT)
            unreal.log("RTCHK   bone {} comp loc={}".format(sock, t))
        except Exception as e:
            unreal.log("RTCHK   bone {} err {}".format(sock, e))
