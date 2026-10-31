---
id: task-410-right-hand-offset-live
description: "Global task: right hand offset on the rifle grip, tunable live by cvar and in ABP_Miner Class Defaults; rifle and left hand stay put."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Live control of the right hand offset on the rifle grip

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Je veux avoir facilement un contrôle sur le offset ?". Context relayed: task-409 just ended (A_Miner_UpperRifle_V2Grip carries the 16.38 cm in the right hand, BP_BurstRifle RifleMesh (0, 0, 0), scale 1.25, Muzzle (52.25, 0, 0)); before, he set the offset by hand in BP_BurstRifle > RifleMesh > Location (task-404); a live PIE edit was not followed by the left hand IK; no cvar exists for it. A first plan (cvar rr.Debug.RifleOffset moving the rifle, commit 4148f4f8) was handed back; owner answer 2026-10-31 (relayed by the main session, verbatim): "Je parle de la main droite, pas la gauche". Main session reading: he wants to control easily the offset of the RIGHT HAND (its position and rotation relative to the pistol grip, in the pose), the rifle staying where it is (RifleMesh 0 0 0, scale 1.25, as after task-409); not a rifle move, not the left hand refresh. The first plan was deleted and replaced by this one, questions Q1 to Q5 handed back 2026-10-31.
- goal: In PIE (or -game) the owner types `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]` and immediately sees the right hand move and turn on the pistol grip by that amount on top of the V2Grip pose, the right arm following, while the rifle keeps its place, angle and size on screen and the left hand stays on the handguard; the same values can be set permanently in the ABP_Miner Class Defaults; offset 0 = today (V2Grip, grip in the palm). No new animation; game rules and shot origin unchanged.
- acceptance: C++ builds; URRMinerAnimInstance has EditDefaultsOnly `RightHandOffsetLocation` (FVector, cm) and `RightHandOffsetRotation` (FRotator, deg), default 0, expressed in the WeaponSocket_R frame (the rifle frame: X toward the muzzle, Z rifle top; rotation pivot = the WeaponSocket_R point, the grip) (Q2); cheat cvar `rr.Debug.RightHandOffset` ("X Y Z" or "X Y Z Pitch Yaw Roll", empty = class defaults) overrides them live for the player miner, applied only when the value changes and at weapon add (Q3); from the offset the class computes BlueprintReadOnly inputs for ABP_Miner (right hand IK effector in RightHand bone space, right hand rotation delta, alpha) and, while a weapon is on its hand socket, sets the held weapon mesh carrying LeftHandGrip (RifleMesh) and the Muzzle to the inverse transform in the socket frame, so the rifle world transform is unchanged (Q1), then calls RefreshLeftHandIK so the left hand stays on LeftHandGrip; empty or 0 restores the Blueprint RifleMesh and Muzzle values; each apply logs one LogRogueRigs line with the values to type into ABP_Miner Class Defaults; a bad value logs a warning and changes nothing; ABP_Miner AnimGraph: right arm Two Bone IK (RightHand, RightForeArm, RightArm) driven by those inputs, then a Transform (Modify) Bone on RightHand for the rotation, both before the left hand Two Bone IK, elbow keeping its direction, stretching off (Q4); ABP compiles with no error, saved; with offset 0 the pose equals V2Grip; projectiles still start at floor + 50 uu; no capture (Q5); doc nodes updated; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; mesh at (0, 0, 0), scale 1.25 on WeaponSocket_R, rifle kept at its task-404 place because the right hand moved in the animation, task-409; no change: the offset defaults to 0 and the rifle stays put), [[burst-rifle-mesh]] (pivot at the pistol grip, LeftHandGrip socket on the handguard), [[bp-burst-rifle]] (RifleMesh (0, 0, 0), scale 1.25, Muzzle (52.25, 0, 0) under the root, visual only), [[bp-miner]] (WeaponSocket_R on RightHand, socket scale 0.01 against a RightHand bone scale about 100; AnimGraph Locomotion -> Layered blend per bone (Spine, UpperBodySequence = V2Grip) -> LocalToComponent -> left Two Bone IK (effector LeftHandIKTarget in RightHand bone space) -> ComponentToLocal; no right arm node), [[sys-player-classes]] (RefreshLeftHandIK only at weapon add; cvar pattern of rr.Debug.MinerUpperPose), [[sys-weapons]] (shot origin independent of the mesh), [[miner-mixamo-clips]] via [[task-409-miner-right-hand-on-grip]] (V2Grip: RightHand moved 16.38 cm in hand space, rotation kept; offset 0 keeps that). Code read: RRMinerAnimInstance.cpp. Feasible without a new animation: the rifle hangs on the right hand socket, so moving the hand alone drags the rifle with it; keeping the rifle still needs the rifle mesh counter-moved by the inverse offset, which is exact and set once per change only when the offset is in the hand (socket) frame (Q1, Q2); the left hand target is in RightHand bone space, so it must be refreshed after each change. Risk: the right arm has a fixed length; an offset beyond reach leaves the hand short of the target while the rifle is counter-moved by the full offset (gap visible), so keep offsets to a few cm. Conflicts: none, rendering only. Open values: none in the gdd; form asked as Q1 to Q5. Milestone M2.
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: task-410-01-right-hand-offset-code
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q1 Rifle while the hand moves: Default: the rifle stays exactly where it is on screen and the hand slides and turns on it (the code moves RifleMesh and Muzzle by the inverse offset at each change, the left hand stays on the handguard). Or: the rifle follows the hand (simpler, no counter-move, but the rifle moves on screen).
- Q2 Axes: Default: the rifle frame, the same axes as RifleMesh Location (X toward the muzzle, Y side, Z rifle top), in cm, rotation Pitch Yaw Roll in degrees around the grip point; exact and computed once per change. Or: the miner frame (forward, right, up), but then the rifle counter-move must be recomputed every frame because the hand turns during the animation.
- Q3 Control: Default: console `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]` live in PIE or -game (`""` returns to the saved values), and the permanent values in ABP_Miner Class Defaults `RightHandOffsetLocation` / `RightHandOffsetRotation` (the log prints the values to type there). Or: Class Defaults only, without live console.
- Q4 Right arm: Default: the arm follows the hand (a Two Bone IK on the right arm in ABP_Miner, elbow keeps its direction, no stretching; keep offsets to a few cm). Or: only the hand moves (the wrist may detach from the forearm on big offsets).
- Q5 Visual check: Default: no capture, you check it yourself in PIE (as in task-404 and task-409). Or: a -game capture subtask with a few offsets.

## Subtasks
1. task-410-01-right-hand-offset-code (vault/reference/tasks/task-410-right-hand-offset-live/task-410-01-right-hand-offset-code.md) — cpp: URRMinerAnimInstance: RightHandOffset properties, cvar rr.Debug.RightHandOffset, ABP inputs, rifle counter-move, left hand IK refresh
2. task-410-02-abp-right-arm-offset (vault/reference/tasks/task-410-right-hand-offset-live/task-410-02-abp-right-arm-offset.md) — bp: ABP_Miner: right arm Two Bone IK and RightHand rotation from the new inputs, before the left hand IK
3. task-410-03-doc-right-hand-offset (vault/reference/tasks/task-410-right-hand-offset-live/task-410-03-doc-right-hand-offset.md) — doc: sys-player-classes, bp-miner, bp-burst-rifle: right hand offset, cvar, counter-move, how to keep values

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-410-01-right-hand-offset-code]] — step 1, cpp.
- [[task-410-02-abp-right-arm-offset]] — step 2, bp.
- [[task-410-03-doc-right-hand-offset]] — step 3, doc.
- [[task-409-miner-right-hand-on-grip]] — baked the 16.38 cm right hand move into V2Grip; offset 0 keeps it.
- [[task-404-rifle-mesh-owner-offset]] — the RifleMesh offset he set by hand, and the left hand refresh limit.
- [[bp-miner]] — ABP_Miner AnimGraph that gets the right arm nodes.
- [[sys-player-classes]] — URRMinerAnimInstance that gets the offset, the cvar and the counter-move.
- [[bp-burst-rifle]] — RifleMesh and Muzzle Blueprint values the counter-move starts from.
