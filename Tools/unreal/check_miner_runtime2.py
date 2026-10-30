"""Diagnosis task-386-10: PIE miner state read: velocity, mesh mode, ABP vars, hand and foot pose."""
import unreal

w = unreal.EditorLevelLibrary.get_game_world()
for a in [x for x in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Character) if "Miner" in x.get_class().get_name()]:
    m = a.get_editor_property("mesh")
    ai = m.get_anim_instance()
    unreal.log("RTCHK2 vel={} loc_z={:.1f} mode={} ai={}".format(a.get_velocity(), a.get_actor_location().z, m.get_editor_property("animation_mode"), ai.get_class().get_name()))
    for s in ["RightHand", "LeftFoot", "Hips"]:
        t = m.get_socket_transform(s, unreal.RelativeTransformSpace.RTS_COMPONENT)
        unreal.log("RTCHK2 {} {:.1f},{:.1f},{:.1f}".format(s, t.translation.x, t.translation.y, t.translation.z))
