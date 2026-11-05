---
id: bp-robot-hit-flash
description: "MF_RobotHitFlash: white hit flash read from Custom Primitive Data 2, the masters using it, its parameters, revert. Open to tune or extend the robot flash."
parent: blueprints-enemies-index
kind: doc
---
# MF_RobotHitFlash

- asset_path: /Game/RogueRigs/Materials/Robots/MF_RobotHitFlash (task-433).
- parent_class: none (material function).
- purpose: turns a robot plain white for a hit, with no dynamic material instance: the value is written by [[sys-enemies-base]] on the `BodyMesh` component.
- components: inputs BaseColor, Emissive (default 0, so a master without emissive leaves it unconnected). Flash = HitFlash x HitFlashStrength. Outputs: BaseColor = lerp(BaseColor, 0, Flash), Emissive = lerp(Emissive, HitFlashEmissive, Flash). World Position Offset, CPD 0 and CPD 1 are not read: baked animation stays independent.
- exposed_defaults: HitFlash 0 (Custom Primitive Data index 2), HitFlashStrength 1, HitFlashEmissive 3 (provisional, picked by the Executor for exposure -0.94 EV100 and a flat white; the owner judges the look in game). Edit the function defaults to tune every robot at once.
- events: none.
- logic_in_bp: none.
- assets: masters that call it between their base colour and emissive sources and the outputs: M_RobotMeshy (body of chaser v2, kamikaze, shooter, heavy), M_ShieldChaserV2_Anim (shield chaser v2), M_RobotEye (eye of every robot, so the eye turns white too).
- exceptions: MI_ShieldChaserV2_Shield and MI_ShieldChaserV2_Emblem set HitFlashStrength 0: the shield keeps its look when a hit lands on it. MI_Kamikaze_Core is unchanged and its master reads the flash, but C++ writes only `BodyMesh`, so `CoreMesh` stays at 0 and the core keeps its orange pulse.
- not_covered: v1 M_Chaser_Meshy_Anim and M_ShieldChaser_Anim (BP_Chaser, BP_ShieldChaser), not spawned by the game ([[bp-enemy-spawner]]); their body would need the same call if the spawner went back to them.
- verified: masters recompiled with no error logged, saved; `RogueRigs.Enemies.HitFlash` reads the instances of the five spawned classes. Not verified: the render, no capture in task-433 by owner decision.
- revert: `git checkout c5afc7f8 -- Content/RogueRigs/Materials/Robots/M_RobotMeshy.uasset Content/RogueRigs/Materials/Robots/M_RobotEye.uasset Content/RogueRigs/Art/ShieldChaserV2/M_ShieldChaserV2_Anim.uasset Content/RogueRigs/Art/ShieldChaserV2/MI_ShieldChaserV2_Shield.uasset Content/RogueRigs/Art/ShieldChaserV2/MI_ShieldChaserV2_Emblem.uasset`, then delete MF_RobotHitFlash.
- owner_agent: agent-bp

## Relations
- [[blueprints-enemies-index]] — parent index.
- [[sys-enemies-base]] — writes the value.
- [[bp-robot-meshy-material]] — M_RobotMeshy, the body master.
- [[bp-robot-eye-material]] — M_RobotEye, the eye master.
- [[bp-shield-chaser-v2]] — shield chaser master and its exceptions.
- [[bp-kamikaze]] — the core that does not flash.
- [[enemy-chaser]] — "hits are a flash", the rule it implements.
