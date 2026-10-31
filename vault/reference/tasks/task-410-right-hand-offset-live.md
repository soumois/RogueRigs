---
id: task-410-right-hand-offset-live
description: "Global task: right hand offset on the V2Grip pose, live by cvar and in ABP_Miner Class Defaults; arm by IK, rifle follows the hand."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Live control of the right hand offset

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Je veux avoir facilement un contrôle sur le offset ?". Context relayed: task-409 just ended (A_Miner_UpperRifle_V2Grip carries the 16.38 cm in the right hand, BP_BurstRifle RifleMesh (0, 0, 0), scale 1.25, Muzzle (52.25, 0, 0)); before, he set the offset by hand in BP_BurstRifle > RifleMesh > Location (task-404); no cvar exists for it. A first plan (cvar rr.Debug.RifleOffset moving the rifle, commit 4148f4f8) was handed back; owner answer 2026-10-31 (relayed, verbatim): "Je parle de la main droite, pas la gauche" = he wants to control easily the offset of the RIGHT HAND (position and rotation, in the pose); that plan was deleted and replaced by this one (commit f661cfe8). Owner answer 2026-10-31 to Q1 to Q5 (relayed by the main session, verbatim): "Q1 ça suit la main, c'est normal. Plus tard, si je veux faire une animation de tir et que la main droite bouge, c'est normal que le fusil bouge avec. le reste par défaut" = Q1 alternative: the rifle follows the hand (no counter-move of RifleMesh or Muzzle, they stay (0, 0, 0) and (52.25, 0, 0)); Q2 to Q5 defaults.
- goal: In PIE (or -game) the owner types `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]` and immediately sees the right hand move and turn by that amount on top of the V2Grip pose, the right arm following by IK, the rifle carried by the hand (it moves with it), the left hand staying on the rifle handguard; the same values can be set permanently in the ABP_Miner Class Defaults; offset 0 = today (V2Grip). No new animation; game rules and shot origin unchanged.
- acceptance: C++ builds; URRMinerAnimInstance has EditDefaultsOnly `RightHandOffsetLocation` (FVector, cm) and `RightHandOffsetRotation` (FRotator, deg), default 0, in the WeaponSocket_R frame (the rifle frame: X toward the muzzle, Y side, Z rifle top; rotation pivot = the WeaponSocket_R point, the grip) (Q2); cheat cvar `rr.Debug.RightHandOffset` ("X Y Z" or "X Y Z Pitch Yaw Roll", empty = class defaults) overrides them live for the player miner, applied only when the value changes (Q3); from the offset the class computes BlueprintReadOnly inputs for ABP_Miner (right hand IK effector in RightHand bone space, right hand rotation delta, alpha); BP_BurstRifle, RifleMesh and Muzzle untouched (Q1); each apply logs one LogRogueRigs line with the values to type into ABP_Miner Class Defaults; a bad value logs a warning and changes nothing; ABP_Miner AnimGraph: right arm Two Bone IK (RightHand, RightForeArm, RightArm) then a Transform (Modify) Bone on RightHand for the rotation, both before the left hand Two Bone IK, elbow keeping its direction, stretching off (Q4); the left hand target needs no refresh (it is in RightHand bone space and the rifle is rigid on the hand); ABP compiles with no error, saved; offset 0 = V2Grip pose (alpha 0); projectiles still start at floor + 50 uu; no capture (Q5); doc nodes updated; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; mesh at (0, 0, 0), scale 1.25 on WeaponSocket_R, task-409; edit applied on confirmation: the rifle follows the right hand, an offset or a firing animation moving the hand moves the rifle), [[burst-rifle-mesh]] (pivot at the pistol grip, LeftHandGrip on the handguard), [[bp-burst-rifle]] (RifleMesh (0, 0, 0), Muzzle (52.25, 0, 0), visual only; unchanged), [[bp-miner]] (WeaponSocket_R on RightHand, socket scale 0.01 against a RightHand bone scale about 100; AnimGraph Locomotion -> Layered blend per bone (Spine, UpperBodySequence = V2Grip) -> LocalToComponent -> left Two Bone IK (effector LeftHandIKTarget in RightHand bone space) -> ComponentToLocal; no right arm node), [[sys-player-classes]] (cvar pattern of rr.Debug.MinerUpperPose; RefreshLeftHandIK at weapon add), [[sys-weapons]] (shot origin independent of the mesh), [[task-409-miner-right-hand-on-grip]] (V2Grip: RightHand moved 16.38 cm in hand space, rotation kept; offset 0 keeps it). Code read: RRMinerAnimInstance.cpp. Feasible without a new animation: right arm IK plus a bone rotation in ABP_Miner, fed by the anim instance. Since the rifle hangs on the right hand socket it moves with the hand (Q1), and the left hand IK target, expressed in RightHand bone space from a rigid grip, stays on the handguard without a refresh because the right arm nodes run before the left IK. Risk: the right arm has a fixed length; an offset beyond reach leaves the hand short of the target, so keep offsets to a few cm. Conflicts: none, rendering only. Open values: none. Milestone M2.
- confirmed_by_owner: yes (2026-10-31, "Q1 ça suit la main, c'est normal. Plus tard, si je veux faire une animation de tir et que la main droite bouge, c'est normal que le fusil bouge avec. le reste par défaut")
- milestone: M2
- next_step: task-410-01-right-hand-offset-code
- started: 2026-10-31T20:09:04Z
- waiting_for: owner closes the Unreal editor (PID 56208) for task-410-01 cpp build
- result: open

## Questions
Decided 2026-10-31, owner answer (relayed by the main session, verbatim): "Q1 ça suit la main, c'est normal. Plus tard, si je veux faire une animation de tir et que la main droite bouge, c'est normal que le fusil bouge avec. le reste par défaut"; values provisional, owner 2026-10-31.
- Q1 Rifle while the hand moves: owner chose the alternative: the rifle follows the hand (no counter-move; RifleMesh and Muzzle unchanged).
- Q2 Axes: default: the rifle frame (WeaponSocket_R: X toward the muzzle, Y side, Z rifle top), cm; rotation Pitch Yaw Roll in degrees around the grip point.
- Q3 Control: default: console `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]` live (`""` returns to the saved values), permanent values in ABP_Miner Class Defaults `RightHandOffsetLocation` / `RightHandOffsetRotation`; the log prints the values to type.
- Q4 Right arm: default: the arm follows by a Two Bone IK on the right arm, elbow keeps its direction, no stretching.
- Q5 Visual check: default: no capture, the owner checks in PIE.

## Subtasks
1. task-410-01-right-hand-offset-code (vault/reference/tasks/task-410-right-hand-offset-live/task-410-01-right-hand-offset-code.md) — cpp: URRMinerAnimInstance: RightHandOffset properties, cvar rr.Debug.RightHandOffset, ABP inputs (effector, rotation delta, alpha)
2. task-410-02-abp-right-arm-offset (vault/reference/tasks/task-410-right-hand-offset-live/task-410-02-abp-right-arm-offset.md) — bp: ABP_Miner: right arm Two Bone IK and RightHand rotation from the new inputs, before the left hand IK
3. task-410-03-doc-right-hand-offset (vault/reference/tasks/task-410-right-hand-offset-live/task-410-03-doc-right-hand-offset.md) — doc: sys-player-classes and bp-miner: right hand offset, cvar, ABP nodes, how to keep values

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-410-01-right-hand-offset-code]] — step 1, cpp.
- [[task-410-02-abp-right-arm-offset]] — step 2, bp.
- [[task-410-03-doc-right-hand-offset]] — step 3, doc.
- [[task-409-miner-right-hand-on-grip]] — baked the 16.38 cm right hand move into V2Grip; offset 0 keeps it.
- [[task-404-rifle-mesh-owner-offset]] — the RifleMesh offset he set by hand.
- [[bp-miner]] — ABP_Miner AnimGraph that gets the right arm nodes.
- [[sys-player-classes]] — URRMinerAnimInstance that gets the offset and the cvar.
- [[burst-rifle]] — gdd node recording that the rifle follows the right hand.
