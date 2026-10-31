---
id: task-405-left-elbow-joint-target
description: "Global task: find why the ABP_Miner Two Bone IK Joint Target Location has no effect and make the left elbow follow it."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Left elbow follows the Two Bone IK joint target

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Pour l'arme je préferais cette position (X=9.898091,Y=-1.129037,Z=-13.000000) et un scale de 1.25. Par contre pour la main gauche, je peux mettre ce que je veux dans Joint Target location, ça ne change rien". Point 2 of 2 ("Par contre pour la main gauche, je peux mettre ce que je veux dans Joint Target location, ça ne change rien"); runs after [[task-404-rifle-mesh-owner-offset]].
- goal: Changing Joint Target Location on the ABP_Miner Two Bone IK visibly moves the miner left elbow while the left hand stays on LeftHandGrip; the cause of the current no-effect is written down.
- acceptance: Cause found and recorded among: IK off where the owner looks (alpha 0 in the ABP editor preview, no held weapon), arm fully stretched (shoulder to LeftHandGrip distance at or above upper arm + forearm length, so no bend), node pin bound or overridden, wrong joint target space or bone; fix applied inside ABP_Miner (Q4); PIE with the owner rifle position: two joint targets at least 40 cm apart give left elbow positions at least 5 cm apart while the hand stays within 3 uu of LeftHandGrip, standing and in the four forced directions; doc node updated by the doc step; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; game mesh SM_BurstRifle "overall length 60 cm", held at socket WeaponSocket_R, left hand on LeftHandGrip by IK, shot origin and height unchanged, provisional owner 2026-10-30), [[burst-rifle-mesh]] (SM_BurstRifle pivot at the pistol grip, muzzle +X, mesh x -18.19 to 41.81 cm; socket LeftHandGrip (22, 0, 0) on the handguard x 14 to 27), [[bp-burst-rifle]] (RifleMesh relative location 0, scale 1; inherited Muzzle at (41.8, 0, 0) under the root, not under RifleMesh; Muzzle is visual only), [[bp-miner]] (WeaponSocket_R on RightHand, world scale 1; ABP_Miner Two Bone IK on LeftHand, effector LeftHandIKTarget in RightHand bone space, joint target (45, 10, 95) cm Component Space, stretching on 1.0 to 1.2, provisional), [[sys-player-classes]] (URRMinerAnimInstance computes LeftHandIKTarget once, on OnWeaponAdded, from the held weapon mesh socket LeftHandGrip in the weapon actor space, so it follows the RifleMesh relative location and scale; alpha 1 only with a held weapon, so 0 in the ABP editor preview where no weapon is held; a live edit of RifleMesh in PIE does not refresh it), [[sys-weapons]] (shot origin = floor + ShotHeight 50 uu along the aim, independent of the mesh). Milestone M2 (art). Conflicts: none (rendering only). Open values: the fix when the cause lies outside the ABP node (Q4). The task-401-03 check was numeric only (hand-to-grip 0.00001 uu, "elbow only numerically checked"). Candidate causes from the nodes: alpha 0 in the ABP editor preview (no weapon held there, so the IK node does nothing); stretching on with the grip at the edge of reach (straight arm, the joint target only picks a bend plane); joint target space or pin. task-404 moves the grip 13 cm lower and further forward, which changes the reach: diagnose after it.
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: task-405-01-joint-target-diagnose-fix
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q4 Fix scope: Default: the bp agent diagnoses (node settings, pin bindings, space, stretch values, PIE shoulder-to-grip distance against arm length, elbow position for two joint targets, ABP editor preview alpha) and fixes only inside ABP_Miner when the cause is there (space, pin, binding, stretch settings). If the cause is the reach (grip too far from the shoulder, arm straight) or the editor preview (IK off without a held weapon, which needs C++ or a preview setup), it stops with the measured cause and options for you, without touching another asset. Or you allow it to move LeftHandGrip back along the handguard (x 14 to 27 cm) when the reach is the cause.
- Q3 of [[task-404-rifle-mesh-owner-offset]] (visual check) applies here.

## Subtasks
1. task-405-01-joint-target-diagnose-fix (vault/reference/tasks/task-405-left-elbow-joint-target/task-405-01-joint-target-diagnose-fix.md) — bp: ABP_Miner: diagnose the no-effect joint target and fix it inside the ABP
2. task-405-02-doc-left-elbow-joint-target (vault/reference/tasks/task-405-left-elbow-joint-target/task-405-02-doc-left-elbow-joint-target.md) — doc: update bp-miner left hand IK section with the cause and the fix

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-405-01-joint-target-diagnose-fix]] — step 1, bp.
- [[task-405-02-doc-left-elbow-joint-target]] — step 2, doc.
- [[task-404-rifle-mesh-owner-offset]] — point 1, runs first; holds Q1 to Q3.
- [[task-401-miner-hands-fit-rifle]] — task that added the Two Bone IK.
- [[bp-miner]] — documents the ABP_Miner left hand IK.
