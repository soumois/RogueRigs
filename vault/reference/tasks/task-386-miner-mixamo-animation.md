---
id: task-386-miner-mixamo-animation
description: "Global task: Save the nine owner Mixamo clips, make them in place, import the miner and clips, build ABP_Miner and put them on BP_Miner."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips

- request: Owner, 2026-10-30, verbatim (French, relayed by the main session, no /task): "Voilà quelques animations mixamo (avec les mains déjà bien posées, plus ou moins, pour l'arme)", with nine files in `C:/Users/soumo/Downloads/`: `Rifle Run.fbx`, `Strafe Left.fbx`, `Strafe.fbx`, `Run Backwards.fbx`, `Run Right.fbx`, `Run Left.fbx`, `Walk With Rifle.fbx`, `Backwards Rifle Walk.fbx`, `Rifle Aiming Idle.fbx` (318 to 599 KB, dated 2026-10-29 22:18 to 22:24). Context relayed by the main session: after task-382 the owner discussed two routes to animate the miner and his last sentence chose the Mixamo route: Mixamo clips on the miner skeleton, a new Animation Blueprint instead of the Quinn ABP_Unarmed, clips played in place, movement from gameplay; the Unreal-rig file he offered is no longer the chosen option. Planner reading of the bytes (2026-10-30, copies in the session scratchpad, read by a Node FBX parser with the path as argument, nothing imported): all nine are binary FBX 7.7 (FBX SDK 2020.2), "without skin" (no Geometry, no Deformer), one AnimStack `mixamo.com` each, 30 fps; their skeleton is exactly the miner one: armature `SmartRigArmature`, 65 `mixamorig:*` bones plus `headfront`, the same 66 bone names as `Art/Source/miner-meshy/Meshy_AI_Character_output.fbx` (diff empty). They are NOT in place: the Hips root bone moves horizontally over each cycle (Hips height 0.9 in file units, read as metres): Rifle Run 2.0 forward in 0.73 s (about 2.7 m/s), Walk With Rifle 3.2 forward in 1.43 s (2.2 m/s), Run Backwards 1.3 back in 0.53 s (2.5 m/s), Backwards Rifle Walk 1.1 back in 1.27 s (0.9 m/s), Run Left 2.1 left in 0.50 s (4.2 m/s), Strafe Left 1.3 left in 1.03 s (1.3 m/s), Run Right 2.2 right in 0.50 s (4.4 m/s), Strafe 2.0 right in 0.67 s (3.0 m/s, so `Strafe` is the right strafe), Rifle Aiming Idle 0 in 3.10 s; approximate first-to-last values, the 3d step measures them exactly. One owner point (animate the miner with these clips) = one global; its subtasks span 3d, cpp, bp, capture and doc. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- goal: In game the miner shows the owner Meshy mesh instead of Quinn, driven by a new ABP_Miner that blends the nine owner Mixamo clips, made in place, by move speed and move direction relative to the aim.
- acceptance: The nine downloads are saved byte-identical in `Art/Source/miner-mixamo/`; `Art/Meshes/Miner/SK_Miner.fbx` and nine `Art/Meshes/Miner/Anims/A_Miner_*.fbx` share one skeleton and every clip moves its Hips less than 1 cm horizontally per cycle; Unreal holds SK_Miner (25,744 triangles, about 170 cm), SKEL_Miner, its material with the two Meshy textures, nine AnimSequences on SKEL_Miner, BS_Miner_Locomotion and ABP_Miner (parent URRMinerAnimInstance); URRMinerAnimInstance builds and gives ground speed, direction relative to the aim, blend speed and play rate (Q4); BP_Miner uses SK_Miner and ABP_Miner_C, feet on the floor, mesh facing the aim, capsule and weapon attachment unchanged; a -game capture reports the checklist of step 08; doc nodes updated by the doc step; vault node miner-mixamo-clips records files and measures; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[miner]] (spec approved: moves freely, aims with mouse or right stick; asset line: Meshy auto-rig delivered, animation route, Unreal import and triangle budget open), [[sys-player]] (`bUseControllerRotationYaw` and `bOrientRotationToMovement` off: the pawn yaw follows only the aim, so moving away from or sideways to the aim needs backward and strafe clips: the nine clips fit the game; MaxWalkSpeed 600 uu/s = 6 m/s from MoveSpeed, changed by upgrades: faster than every clip, Q4), [[sys-player-input-aim]] (stick input is analog, so speeds under the maximum happen and use the walk clips), [[bp-miner]] (SKM_Quinn_Simple with ABP_Unarmed_C, location (0, 0, -95), yaw -90, capsule half height 88 radius 34; weapons attached to the capsule at WeaponAttachOffset (0, 20, -40), WeaponAttachSocketName None since task-008-03), [[bp-burst-rifle]] (placeholder cube, Muzzle at +40 X; on `hand_r` the muzzle pointed up and shots flew over rock: why the weapon stays on the capsule, Q6), [[art-prompts-miner]] (rigged mesh measures: 25,744 tris, 66 bones, root `mixamorig:Hips`, no clip, textures texture_0 and normal 2048), [[asset-pipeline]] (no miner triangle budget; stale line "miner auto-rig, Tripo, deferred": corrected in the edits; Blender FBX export with FBX_SCALE_UNITS), [[cpp-bp-split]] (logic in C++: speed, direction and play rate are computed in a C++ anim instance, ABP_Miner only wires its graph), [[unreal-mcp-quirks]] (MCP import of FBX gave a 100x scale; editor Python FbxImportUI + AssetImportTask worked; no skeletal or animation import done before in this project: first time, risk noted in steps 03 to 06). Conflicts: none. Open values the work needs: triangle budget (Q5), in-place method (Q2), clip mapping (Q3), speed mismatch (Q4), weapon hand socket (Q6, default: stays open). Decided by the owner words: animation route = Mixamo clips on the miner skeleton, in place, new ABP_Miner, movement from gameplay (edit in miner.md). Not in scope: weapon in the hands, death or hit clips, deleting the Quinn template assets, new tests. Untrusted downloads: copied and read only with the path as argument. Milestone M2 (art).
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- next_step: task-386-14-capture-palier2
- started: 2026-10-30T06:09:35Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 Scope: one task for your point "animate the miner with these clips": save and measure the nine FBX, make them in place, import the miner mesh and the clips into Unreal, a 2D blend space, a small C++ anim instance (speed, direction to the aim, play rate), a new ABP_Miner, then BP_Miner shows SK_Miner with ABP_Miner instead of SKM_Quinn_Simple and ABP_Unarmed (Quinn assets stay in the project, unused by BP_Miner), a game capture and the doc. No death or hit clip. Default: yes, all of it in this task.
- Q2 In place: the nine clips are not in place (the Hips moves 1 to 3 m per cycle, see request). Default: the 3d agent makes them in place in headless Blender (Hips horizontal motion removed, vertical bob kept), no new download. Or you download them again from Mixamo with "In Place" ticked.
- Q3 Clip mapping (direction relative to the aim): forward Rifle Run (run) and Walk With Rifle (walk); backward Run Backwards (run) and Backwards Rifle Walk (walk); left Run Left (run) and Strafe Left (walk); right Run Right (run) and Strafe (it moves right); standing: Rifle Aiming Idle. Default: all nine in one 2D blend space (direction -180 to 180, speed), each clip at its measured direction and speed.
- Q4 Speed: the miner moves at 600 uu/s (6 m/s, more with upgrades), faster than every clip (2.5 to 4.4 m/s for the runs): the feet would slide and the legs look slow. Default: the C++ anim instance speeds up the clips so the step rhythm matches: play rate = ground speed / run speed of the clip in that direction, between 1 and 2.5 (provisional), the blend uses ground speed / play rate. Or play the clips at their own speed and accept the sliding.
- Q5 Triangle budget of the miner: none in the GDD. Default: 25,744 as delivered, no decimation (one miner on screen; turret 16,000, extractor 50,000), provisional.
- Q6 Weapon in the hands: the burst rifle is a placeholder cube attached to the capsule, and its muzzle sets the shot height (on the hand it shot over the rocks, task-007-05). Default: not in this task, the weapon stays where it is and the hands hold an empty grip; a hand socket is its own task when you ask.

## Subtasks
1. task-386-01-miner-clips-in-place (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-01-miner-clips-in-place.md) — 3d: save the nine Mixamo FBX, measure them, export SK_Miner and nine in-place clips on one skeleton from headless Blender, record them in miner-mixamo-clips
2. task-386-02-miner-anim-instance (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-02-miner-anim-instance.md) — cpp: URRMinerAnimInstance: ground speed, direction to the aim, blend speed and play rate for ABP_Miner
3. task-386-03-miner-skeletal-import (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-03-miner-skeletal-import.md) — bp: import SK_Miner with SKEL_Miner, its physics asset and a material from the two Meshy textures
4. task-386-04-miner-clips-import (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-04-miner-clips-import.md) — bp: import the nine in-place clips as AnimSequences on SKEL_Miner
5. task-386-05-miner-locomotion-blendspace (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-05-miner-locomotion-blendspace.md) — bp: BS_Miner_Locomotion: 2D blend space, direction and speed, nine clips at their measured positions
6. task-386-06-abp-miner (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-06-abp-miner.md) — bp: ABP_Miner: Animation Blueprint of URRMinerAnimInstance playing BS_Miner_Locomotion
7. task-386-07-bp-miner-mesh-swap (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-07-bp-miner-mesh-swap.md) — bp: BP_Miner: SK_Miner and ABP_Miner_C instead of SKM_Quinn_Simple and ABP_Unarmed, feet on the floor, facing the aim
8. task-386-10-miner-anim-diagnosis (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-10-miner-anim-diagnosis.md) — bp: diagnose the T-pose (cancelled 2026-10-30, replaced by the stepped approach 9 to 13)
9. task-386-11-abp-palier1-idle (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-11-abp-palier1-idle.md) — bp: step 1: ABP_Miner reduced to Rifle Aiming Idle looping
10. task-386-12-capture-palier1 (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-12-capture-palier1.md) — capture: step 1 seen on screen, no T-pose
11. task-386-13-abp-palier2-run (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-13-abp-palier2-run.md) — bp: step 2: idle to Rifle Run driven by speed
12. task-386-14-capture-palier2 (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-14-capture-palier2.md) — capture: step 2 seen on screen, run plays
13. task-386-15-abp-palier3-directions (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-15-abp-palier3-directions.md) — bp: step 3: walk and the other directions, reusing or rebuilding BS_Miner_Locomotion by groups
14. task-386-08-miner-animation-capture (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-08-miner-animation-capture.md) — capture: final -game capture of the animated miner against the checklist C1 to C7
15. task-386-09-doc-miner-animation (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-09-doc-miner-animation.md) — doc: update bp-miner and sys-player-classes, create bp-abp-miner for ABP_Miner, BS_Miner_Locomotion and the imported miner assets
## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-386-01-miner-clips-in-place]] — step 1, 3d.
- [[task-386-02-miner-anim-instance]] — step 2, cpp.
- [[task-386-03-miner-skeletal-import]] — step 3, bp.
- [[task-386-04-miner-clips-import]] — step 4, bp.
- [[task-386-05-miner-locomotion-blendspace]] — step 5, bp.
- [[task-386-06-abp-miner]] — step 6, bp.
- [[task-386-07-bp-miner-mesh-swap]] — step 7, bp.
- [[task-386-10-miner-anim-diagnosis]] — cancelled diagnosis of the T-pose.
- [[task-386-11-abp-palier1-idle]] — step 9, bp, idle only.
- [[task-386-12-capture-palier1]] — step 10, capture of the idle.
- [[task-386-13-abp-palier2-run]] — step 11, bp, run forward.
- [[task-386-14-capture-palier2]] — step 12, capture of the run.
- [[task-386-15-abp-palier3-directions]] — step 13, bp, other directions.
- [[task-386-08-miner-animation-capture]] — step 14, final capture.
- [[task-386-09-doc-miner-animation]] — step 15, doc.
- [[miner]] — the character this animates.
- [[art-prompts-miner]] — the rigged mesh these clips play on.
- [[task-382-miner-rigged-meshy-files]] — saved and measured the rigged mesh.
- [[bp-miner]] — the Blueprint whose mesh and anim class change.
