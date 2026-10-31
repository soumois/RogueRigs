---
id: task-409-miner-right-hand-on-grip
description: "Global task: new upper body animation from V2 with the right hand on the rifle grip; RifleMesh offset reset so the rifle stays in place."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session), with a close screenshot of the miner holding the rifle (right hand far from the pistol grip): "est-ce qu'il y aurait moyen que la main droite soit prés de la poignée ? ( est-ce que je peux Controller ça ? )". Planner answer relayed 2026-10-31: probable cause = the task-404 RifleMesh offset (9.898091, -1.129037, -13.0) cm in BP_BurstRifle moved the rifle, whose pivot is the pistol grip, about 16 cm away from WeaponSocket_R in the right palm; he can bring the grip back himself (RifleMesh location toward 0, or the socket), but the rifle then goes back up to the shoulder; keeping the rifle where he put it needs the hand moved in the animation. Options: default = he adjusts it himself, no task; B = the rifle stays where he put it and the right hand goes down onto the grip in the animation. Owner answer 2026-10-31 (relayed by the main session, verbatim): "B". Owner precision 2026-10-31 (relayed, verbatim): "Le but c'est de garder le fusil où il est et de bouger la main"; the main session read it as "RifleMesh stays (9.898091, -1.129037, -13.0), scale 1.25, not reset to (0, 0, 0)". Planner note: the rifle hangs on the right hand socket, so if the hand moves with the RifleMesh value unchanged, the rifle moves with the hand and the gap stays; keeping the rifle where it is on screen while the hand moves therefore needs either the RifleMesh value reset to 0 at the same time as the hand moves by that offset (rifle world place unchanged, default) or the rifle attached to another bone than the hand (Q4).
- goal: In game the right hand holds the rifle pistol grip and the rifle stays at the world place the owner chose in task-404 (same position, orientation and scale 1.25): a new animation A_Miner_UpperRifle_V2Grip made from A_Miner_UpperRifle_V2 moves the right hand by the RifleMesh offset (hand rotation unchanged), ABP_Miner plays it on the upper body, BP_BurstRifle RifleMesh location goes back to (0, 0, 0); left hand IK unchanged; V1, V2, V3 and A_Miner_Idle kept; game rules and shot origin unchanged.
- acceptance: Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V2Grip.fbx made from A_Miner_UpperRifle_V2.fbx in headless Blender: on every frame the WeaponSocket_R point of the new pose within 0.5 cm of where the V2 pose puts the current grip (socket transform applied to (9.898091, -1.129037, -13.0) cm), RightHand component-space rotation within 0.5 deg of V2, fingers unchanged in hand space, elbow per Q2, every bone outside the right arm identical to V2, same skeleton, frame count and rate; imported on SKEL_Miner in /Game/RogueRigs/Characters/Miner/Anims/ with the task-386 settings; ABP_Miner UpperBodySequence class default = A_Miner_UpperRifle_V2Grip, compiles; BP_BurstRifle RifleMesh relative location (0, 0, 0), rotation (0, 0, 0), scale (1.25, 1.25, 1.25), Muzzle (52.25, 0, 0) (= 1.25 x 41.8), compiled and saved; capture before and after from the same two close views: right hand on the grip, rifle at the same place, left hand on the handguard (Q3); miner and burst-rifle gdd lines updated (edits on confirmation); doc nodes updated; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[miner]] (approved; upper body = A_Miner_UpperRifle_V2, elbow +6 cm, hand unchanged, task-408; edit on confirmation), [[burst-rifle]] (approved; in the hand the mesh sits at (9.898091, -1.129037, -13) cm, scale 1.25, 75 cm in game, task-404; edit on confirmation: location (0, 0, 0), same world place), [[burst-rifle-mesh]] (SM_BurstRifle pivot at the pistol grip hand hold, muzzle +X at 41.81 cm), [[bp-miner]] (WeaponSocket_R on RightHand derived with the grip between RightHand and RightHandMiddle1, so at RifleMesh 0 the grip is in the palm; left hand IK target in RightHand bone space, so the left hand follows the rifle; UpperBodySequence = V2), [[bp-burst-rifle]] (RifleMesh (9.898091, -1.129037, -13.0), scale 1.25, Muzzle (62.148091, -1.129037, -13.0), visual only), [[sys-player-classes]] (LeftHandIKTarget recomputed at weapon add; rr.Debug.MinerUpperPose, rr.Debug.MinerAnimFreeze), [[miner-mixamo-clips]] (how V1..V3 were built). Why the animation: the rifle hangs on the right hand, so a right hand IK in the ABP would drag the rifle with it; moving the hand by the RifleMesh offset in the animation and resetting the offset to 0 keeps the rifle world transform when the hand rotation is kept. Conflicts: none, rendering only (shot origin floor + 50 uu unchanged). Open values: number of variants (Q1), elbow (Q2), visual check (Q3), rifle kept on screen versus RifleMesh value kept (Q4). Risk: the hand moves about 16 cm down and forward; if the arm cannot reach without stretching on some frame, the 3d step reports the distance and stops. Milestone M2 (art).
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: task-409-01-hand-grip-look-before
- started: open
- waiting_for: owner answers Q1 to Q4 and confirmation of the plan
- result: open

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q1 Number of animations: Default: one, A_Miner_UpperRifle_V2Grip = V2 with the right hand moved exactly onto the grip (the rifle position is fixed, so there is one exact hand place). Or three (V2Grip_A to C) that differ by the elbow bend, using the V1, V2 and V3 elbows as the bend direction, compared on one frame as in task-408.
- Q2 Right elbow: Default: it follows the hand naturally, bending in the same direction as in V2 (it comes down with the hand). Or it keeps its V2 height as far as the arm allows (the 3d step reports the frames where it cannot).
- Q4 "garder le fusil où il est": the rifle hangs on the right hand, so it follows the hand; with the RifleMesh value left at (9.898091, -1.129037, -13.0) a moved hand drags the rifle along and the gap stays. Default: the rifle stays where it is ON SCREEN (same place, angle and size, checked by the capture) and only the hand moves; to get that, the RifleMesh value goes to (0, 0, 0) in the same task, because the hand now carries the offset (step 04). Or: the RifleMesh value is not touched and the rifle is attached to the upper body (a chest or shoulder bone) instead of the hand; that is a cpp change of the weapon attach and of the left hand IK, re-planned on your answer.
- Q3 Visual check: Default: a capture of the current look first (V2 with the rifle offset), then the same two close views after the change, side by side, sent to you. Or no capture: you validate in the editor or in game yourself.

## Subtasks
1. task-409-01-hand-grip-look-before (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-01-hand-grip-look-before.md) — capture: look before: current V2 with the rifle offset, two close views (Q3)
2. task-409-02-upper-rifle-v2-grip (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-02-upper-rifle-v2-grip.md) — 3d: headless Blender: A_Miner_UpperRifle_V2Grip from V2, right hand moved onto the grip, hand rotation kept
3. task-409-03-import-v2-grip (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-03-import-v2-grip.md) — bp: import A_Miner_UpperRifle_V2Grip on SKEL_Miner, ABP_Miner UpperBodySequence set to it
4. task-409-04-rifle-mesh-offset-reset (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-04-rifle-mesh-offset-reset.md) — bp: BP_BurstRifle: RifleMesh location (0, 0, 0), scale 1.25 kept, Muzzle (52.25, 0, 0)
5. task-409-05-hand-grip-capture (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-05-hand-grip-capture.md) — capture: after: same two close views, before and after side by side for the owner
6. task-409-06-doc-hand-grip (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-06-doc-hand-grip.md) — doc: doc: bp-miner (V2Grip, upper body source) and bp-burst-rifle (RifleMesh and Muzzle)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-409-01-hand-grip-look-before]] — step 1, capture.
- [[task-409-02-upper-rifle-v2-grip]] — step 2, 3d.
- [[task-409-03-import-v2-grip]] — step 3, bp.
- [[task-409-04-rifle-mesh-offset-reset]] — step 4, bp.
- [[task-409-05-hand-grip-capture]] — step 5, capture.
- [[task-409-06-doc-hand-grip]] — step 6, doc.
- [[task-408-miner-right-arm-in-animation]] — made A_Miner_UpperRifle_V2, the source of the new animation.
- [[task-404-rifle-mesh-owner-offset]] — set the RifleMesh offset this task moves into the hand.
- [[bp-miner]] — documents ABP_Miner and WeaponSocket_R.
- [[bp-burst-rifle]] — documents RifleMesh and Muzzle.
