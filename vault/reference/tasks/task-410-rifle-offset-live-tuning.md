---
id: task-410-rifle-offset-live-tuning
description: "Global task: console variable rr.Debug.RifleOffset sets the held rifle offset live in PIE, left hand IK and Muzzle follow."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Live control of the rifle offset in the right hand

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Je veux avoir facilement un contrôle sur le offset ?". Context relayed by the main session: task-409 just ended (A_Miner_UpperRifle_V2Grip carries the 16.38 cm in the right hand, BP_BurstRifle RifleMesh at (0, 0, 0), scale 1.25, Muzzle (52.25, 0, 0)); the owner means the rifle/hand offset (rifle position relative to the right hand and the pistol grip). Before, he set it by hand in BP_BurstRifle > RifleMesh > Location (task-404), but a live edit in PIE is not followed by the left hand IK (refresh limit documented in task-404) and no cvar exists for it (only rr.Debug.MinerUpperPose and rr.Debug.MinerAnimFreeze). He wants to adjust that offset easily, ideally live to see the result. Planner reading: the offset is the RifleMesh relative location on WeaponSocket_R (the rifle stays attached to the right hand and follows it; offset 0 = the task-409 state, grip in the palm); questions Q1 to Q5 handed back 2026-10-31.
- goal: In PIE (or -game) the owner types `rr.Debug.RifleOffset X Y Z` (cm, the same axes and meaning as BP_BurstRifle > RifleMesh > Location) and immediately sees the held rifle move relative to the right hand, the rifle still attached to WeaponSocket_R, the left hand following its LeftHandGrip and the Muzzle moved by the same amount; an empty value restores the Blueprint values; the log prints the values to type into BP_BurstRifle to keep them. Debug only: nothing saved, game rules and shot origin unchanged.
- acceptance: C++ builds (Development Editor); console variable `rr.Debug.RifleOffset` (string "X Y Z", ECVF_Cheat, default empty) exists; on a new value the player miner hand-held weapon mesh that carries LeftHandGrip (RifleMesh on BP_BurstRifle) gets relative location (X, Y, Z), rotation and scale unchanged (1.25 kept); Muzzle relative location = its Blueprint value + (X, Y, Z) minus the Blueprint RifleMesh location (so (52.25 + X, Y, Z) today); LeftHandIKTarget recomputed through RefreshLeftHandIK on the same frame, so the left hand stays on LeftHandGrip; an empty value restores the Blueprint RifleMesh and Muzzle values and refreshes the IK; an unparsable value logs a warning and changes nothing; each applied value logs one LogRogueRigs line with the RifleMesh Location and Muzzle values to copy into BP_BurstRifle; applied only when the value changes (no per-frame work beyond the existing cvar read pattern of rr.Debug.MinerUpperPose); a new weapon spawn keeps the current cvar value; robots and other weapons untouched; projectiles still start at floor + 50 uu (shot origin independent of the mesh); no capture (Q5); doc nodes updated by the doc step; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; in the hand the mesh sits at (0, 0, 0), scale 1.25 on WeaponSocket_R, the rifle keeps its task-404 place because the right hand moved in the animation, task-409; no change: the cvar is a debug tool, the decided value stays in the BP), [[burst-rifle-mesh]] (pivot at the pistol grip, muzzle +X at 41.81 cm, LeftHandGrip socket), [[bp-burst-rifle]] (RifleMesh (0, 0, 0), scale 1.25, Muzzle (52.25, 0, 0) under the root not under RifleMesh, visual only), [[bp-miner]] (WeaponSocket_R on RightHand; left hand Two Bone IK effector LeftHandIKTarget in RightHand bone space; V2Grip upper body), [[sys-player-classes]] (URRMinerAnimInstance: RefreshLeftHandIK computes LeftHandIKTarget only at weapon add, so a live RifleMesh edit is not followed, task-404 Q5 B never done; existing cheats rr.Debug.MinerUpperPose, rr.Debug.MinerAnimFreeze, rr.Debug.MinerAnim, rr.Debug.MinerAnimForce, all in RRMinerAnimInstance.cpp), [[sys-weapons]] (shot origin floor + ShotHeight 50 along the aim, independent of the mesh and of Muzzle). Code read: RRMinerAnimInstance.cpp (cvar pattern: string cvar read in NativeUpdateAnimation, applied only when it changes, player-controlled miner only). What the offset moves: the rifle relative to the hand (RifleMesh location), so the rifle stays attached to the hand and follows every animation; a non-zero value moves the pistol grip off the right palm by that amount, because since task-409 the hand already carries the 16.38 cm (Q1; keeping the grip in the palm while moving hand and rifle together needs a right arm IK, alternative B). Conflicts: none, rendering and debug only. Open values: none in the gdd; form and scope asked as Q1 to Q5. Milestone M2 (art tuning).
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: task-410-01-rifle-offset-cvar
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q1 What the offset moves: Default A: the rifle relative to the right hand (it is the RifleMesh Location you used in task-404): the rifle stays attached to the hand and follows it in every animation, the left hand follows the rifle; 0 0 0 = today (grip in the palm); any other value moves the pistol grip off the right palm by that amount. Or B: hand and rifle move together (a right arm IK in ABP_Miner, the grip stays in the palm, the arm may stretch); larger work: cpp + ABP_Miner bp subtask.
- Q2 How to set it: Default: console variable `rr.Debug.RifleOffset X Y Z` in cm typed in the PIE (or -game) console, applied at once; `rr.Debug.RifleOffset ""` restores the BP values; typing `rr.Debug.RifleOffset` alone shows the current value. Or additionally: the left hand also follows a live edit of RifleMesh Location in the PIE Details panel (editor-only refresh, task-404 Q5 B), less certain to work on a component edit.
- Q3 Rotation and scale: Default: location only (X Y Z); rotation 0 and scale 1.25 stay from BP_BurstRifle. Or: optional rotation too, `rr.Debug.RifleOffset X Y Z Pitch Yaw Roll`.
- Q4 Keeping the value: Default: the cvar saves nothing; once happy you type the logged values into BP_BurstRifle > RifleMesh > Location and Muzzle (the log prints both lines). Or: you give the values and a later request has the bp agent write them.
- Q5 Visual check: Default: no capture, you check it yourself in PIE (as in task-404 and task-409). Or: a -game capture subtask with a few offsets.

## Subtasks
1. task-410-01-rifle-offset-cvar (vault/reference/tasks/task-410-rifle-offset-live-tuning/task-410-01-rifle-offset-cvar.md) — cpp: cvar rr.Debug.RifleOffset "X Y Z": held weapon mesh relative location live, Muzzle moved by the same delta, left hand IK refreshed
2. task-410-02-doc-rifle-offset (vault/reference/tasks/task-410-rifle-offset-live-tuning/task-410-02-doc-rifle-offset.md) — doc: sys-player-classes (cvar, live IK refresh through it), bp-burst-rifle and bp-miner (how to tune the offset and keep it)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-410-01-rifle-offset-cvar]] — step 1, cpp.
- [[task-410-02-doc-rifle-offset]] — step 2, doc.
- [[task-404-rifle-mesh-owner-offset]] — set the RifleMesh offset by hand and documented the left hand refresh limit (Q5 B not done).
- [[task-409-miner-right-hand-on-grip]] — moved the offset into the right hand animation, RifleMesh back to 0.
- [[sys-player-classes]] — URRMinerAnimInstance, RefreshLeftHandIK and the existing debug cvars.
- [[bp-burst-rifle]] — RifleMesh and Muzzle values the cvar overrides.
