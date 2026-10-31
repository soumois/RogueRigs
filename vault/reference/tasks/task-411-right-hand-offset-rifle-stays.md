---
id: task-411-right-hand-offset-rifle-stays
description: "Global task: fix of task-410: the right hand offset counter-moves RifleMesh and Muzzle so the hand slides on a still rifle; left hand IK refreshed."
parent: tasks-to-do-index
kind: task
status: todo
---
# Right hand offset moves the hand on the rifle, the rifle stays in place

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session), after testing task-410 in PIE: "ça ne fonctionne pas la commande, le fusil bouge avec". Main session analysis relayed: with task-410 Q1 = "le fusil suit la main", the rifle is attached to WeaponSocket_R on the right hand, so moving the hand with the Two Bone IK moves the rifle too; the hand never moves relative to the grip, so the setting has no visible effect for his goal (placing the hand on the grip). He answered "ça suit la main" thinking of a future firing animation, not of this setting. Wanted: the right hand offset moves the hand relative to the rifle (the rifle stays in place on screen), while the rifle still follows the hand in the animations (firing). Owner precision 2026-10-31 (relayed, verbatim): "en fait c'est de rajouter un offset à l'animation de la main droite, mais le fusil doit prendre la position d'origine" = the offset is added on top of the right hand animation (tunable), and the rifle keeps its original position (where it is at offset 0, the current V2Grip state, which is also the task-404 place); only the tunable offset is compensated, a future firing animation that moves the hand still moves the rifle. Decided: counter-move of RifleMesh and Muzzle by the inverse offset. Remaining questions Q1 and Q2 handed back 2026-10-31. Owner answer 2026-10-31 (relayed by the main session, verbatim): "Confirme, fermé" = the plan and the defaults of Q1, Q2 accepted.
- goal: In PIE `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]` (and the ABP_Miner Class Defaults RightHandOffsetLocation / RightHandOffsetRotation) move and turn the right hand on the rifle while the rifle keeps its place, angle and size on screen and the left hand stays on the handguard; in every animation the rifle still follows the hand (the counter-move is fixed relative to the hand); offset 0 = today. ABP_Miner and assets unchanged; game rules and shot origin unchanged.
- acceptance: C++ builds; on every RightHandOffset apply (begin play, cvar change) and at weapon add, for the weapon attached on its hand socket: the mesh carrying LeftHandGrip (RifleMesh) gets relative transform = its Blueprint relative transform (from the weapon class default object) composed with the inverse offset in the socket frame (R' = R * O^-1, which with the task-410 hand delta D = S^-1 * O * S keeps the rifle world transform), Muzzle likewise (M' = M * O^-1), scale 1.25 kept; then RefreshLeftHandIK so the left hand target follows the grip in the moved hand space; offset 0 restores the Blueprint values exactly; the apply log line adds the RifleMesh and Muzzle relative transforms applied; cvar help text no longer says the rifle follows the hand; no change to ABP_Miner, BP_BurstRifle asset values, WeaponSocket_R, AddWeapon or the shot origin (floor + 50 uu); no capture (Q2); doc nodes updated; gdd burst-rifle line updated (edit on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; the task-410 line says a right hand offset moves the rifle with the hand; edit on confirmation: the rifle follows the hand in the animations, the offset moves the hand on the rifle), [[bp-burst-rifle]] (RifleMesh (0, 0, 0), scale 1.25, Muzzle (52.25, 0, 0) under the root, visual only), [[bp-miner]] and [[sys-player-classes]] (task-410: right arm Two Bone IK with effector RightHandIKEffector in RightHand bone space, Transform (Modify) Bone RightHandRotationDelta, both before the left IK; left IK target in RightHand bone space computed by RefreshLeftHandIK from the grip in weapon actor space, at weapon add only). Verified 2026-10-31 in Saved/Logs/RogueRigs.log (owner PIE session 20:30-20:31): the cvar is applied and logged ("10 0 0", "100 0 0", "0 10 0", "0 20 0"), alpha 1, effector 0.0997 bone units for 10 cm (bone scale about 100, so correct); no ABP_Miner or TwoBoneIK warning. So the command works as task-410 specified; the defect is the design choice Q1 of task-410 (rifle carried by the hand), not a bug. Note: "100 0 0" is 1 m, far beyond the arm reach. Code read: RRMinerAnimInstance.cpp RefreshRightHandOffset (D = S^-1 * O * S), RRWeapon.h (Muzzle is a public-read BlueprintReadOnly component, protected in C++: a const getter may be needed). Conflicts: none, rendering only. Risk: an offset beyond the arm reach leaves the hand short while the rifle is counter-moved by the full offset, so the rifle shifts by the unreached part (Q1). Milestone M2.
- confirmed_by_owner: yes (2026-10-31, "Confirme, fermé")
- milestone: M2
- next_step: task-411-01-rifle-counter-move
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-31 (owner "Confirme, fermé"): every default below is accepted; values provisional, owner 2026-10-31.
- Q1 Arm reach: Default: no limit in code, you keep offsets of a few cm (your test "100 0 0" was 1 m, beyond the arm; past the reach the hand stops short and the rifle shifts by the missing part). Or: the code clamps the offset to 15 cm with a warning in the log.
- Q2 Visual check: Default: no capture, you check in PIE. Or: a -game capture subtask with a few offsets.

## Subtasks
1. task-411-01-rifle-counter-move (vault/reference/tasks/task-411-right-hand-offset-rifle-stays/task-411-01-rifle-counter-move.md) — cpp: URRMinerAnimInstance: RifleMesh and Muzzle counter-moved by the inverse right hand offset, left hand IK refreshed
2. task-411-02-doc-rifle-counter-move (vault/reference/tasks/task-411-right-hand-offset-rifle-stays/task-411-02-doc-rifle-counter-move.md) — doc: sys-player-classes, bp-miner, bp-burst-rifle: the offset moves the hand on a still rifle (counter-move), left hand refresh

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-411-01-rifle-counter-move]] — step 1, cpp.
- [[task-411-02-doc-rifle-counter-move]] — step 2, doc.
- [[task-410-right-hand-offset-live]] — added the offset, cvar and ABP right arm nodes this task fixes.
- [[sys-player-classes]] — URRMinerAnimInstance RefreshRightHandOffset and RefreshLeftHandIK.
- [[bp-burst-rifle]] — RifleMesh and Muzzle Blueprint values the counter-move starts from.
- [[burst-rifle]] — gdd line on what moves the rifle.
