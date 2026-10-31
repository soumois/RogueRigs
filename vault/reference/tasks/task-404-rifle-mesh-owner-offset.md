---
id: task-404-rifle-mesh-owner-offset
description: "Global task: BP_BurstRifle RifleMesh at the owner relative location and scale 1.25, Muzzle moved to the scaled barrel tip."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Burst rifle mesh at the owner position and scale 1.25

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Pour l'arme je préferais cette position (X=9.898091,Y=-1.129037,Z=-13.000000) et un scale de 1.25. Par contre pour la main gauche, je peux mettre ce que je veux dans Joint Target location, ça ne change rien". Point 1 of 2 ("Pour l'arme je préferais cette position ... et un scale de 1.25"); point 2 is [[task-405-left-elbow-joint-target]]. Owner answer 2026-10-31 (relayed by the main session, verbatim): "1 C'était directement sur le RifleMesh que j'ai testé ces valeurs" = the values are the RifleMesh relative transform in BP_BurstRifle, tested by the owner in the editor (decided, not a default).
- goal: In game the burst rifle mesh sits where the owner placed it: RifleMesh relative location (9.898091, -1.129037, -13.0), uniform scale 1.25, rotation unchanged; Muzzle at the scaled barrel tip; the left hand still on LeftHandGrip; game rules unchanged.
- acceptance: BP_BurstRifle RifleMesh relative location reads back (9.898091, -1.129037, -13.0), relative scale (1.25, 1.25, 1.25), rotation unchanged; Muzzle relative location (62.148091, -1.129037, -13.0) (Q1); BP compiled and saved; PIE: projectiles still spawn at z 50 (shot origin unchanged), LeftHandIKTarget recomputed from the moved grip and the left hand within 3 uu of LeftHandGrip standing; doc nodes updated by the doc step; burst-rifle gdd records the in-hand transform (Q2); lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; game mesh SM_BurstRifle "overall length 60 cm", held at socket WeaponSocket_R, left hand on LeftHandGrip by IK, shot origin and height unchanged, provisional owner 2026-10-30), [[burst-rifle-mesh]] (SM_BurstRifle pivot at the pistol grip, muzzle +X, mesh x -18.19 to 41.81 cm; socket LeftHandGrip (22, 0, 0) on the handguard x 14 to 27), [[bp-burst-rifle]] (RifleMesh relative location 0, scale 1; inherited Muzzle at (41.8, 0, 0) under the root, not under RifleMesh; Muzzle is visual only), [[bp-miner]] (WeaponSocket_R on RightHand, world scale 1; ABP_Miner Two Bone IK on LeftHand, effector LeftHandIKTarget in RightHand bone space, joint target (45, 10, 95) cm Component Space, stretching on 1.0 to 1.2, provisional), [[sys-player-classes]] (URRMinerAnimInstance computes LeftHandIKTarget once, on OnWeaponAdded, from the held weapon mesh socket LeftHandGrip in the weapon actor space, so it follows the RifleMesh relative location and scale; alpha 1 only with a held weapon, so 0 in the ABP editor preview where no weapon is held; a live edit of RifleMesh in PIE does not refresh it), [[sys-weapons]] (shot origin = floor + ShotHeight 50 uu along the aim, independent of the mesh). Milestone M2 (art). Conflict: [[burst-rifle]] says the game mesh is 60 cm long; scale 1.25 shows it 75 cm (Q2). Open values: Muzzle position after the scale (Q1). LeftHandGrip needs no change: it is a mesh socket, so it moves and scales with RifleMesh (grip at 22 cm becomes 27.5 cm forward of the pistol grip, 13 cm lower in the socket frame).
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: task-404-01-rifle-mesh-transform
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q1 Muzzle: Muzzle is an inherited component under the weapon root, not under RifleMesh, so it does not follow the mesh by itself. Default: move it to the scaled barrel tip, relative location (9.898091 + 1.25 x 41.8, -1.129037, -13.0) = (62.148091, -1.129037, -13.0); it stays visual only, the bullets keep starting from the shared origin (floor + 50 uu along the aim). Or leave Muzzle at (41.8, 0, 0).
- Q2 GDD length: [[burst-rifle]] says the rifle is 60 cm; at scale 1.25 it shows 75 cm in game. Default: the gdd records the in-hand transform (location, scale 1.25, 75 cm in game) and the mesh asset stays 60 cm, owner 2026-10-31.
- Q3 Visual check: Default: no capture subtask for task-404 and task-405; you chose these values in the editor and you test the joint target yourself, so you validate in the editor (the task-401 captures were ambiguous from the top-down camera). Or add a -game capture subtask at the end of each task.

## Subtasks
1. task-404-01-rifle-mesh-transform (vault/reference/tasks/task-404-rifle-mesh-owner-offset/task-404-01-rifle-mesh-transform.md) — bp: BP_BurstRifle: RifleMesh at the owner location and scale 1.25, Muzzle at the scaled tip
2. task-404-02-doc-rifle-mesh-offset (vault/reference/tasks/task-404-rifle-mesh-owner-offset/task-404-02-doc-rifle-mesh-offset.md) — doc: update bp-burst-rifle with the RifleMesh transform and Muzzle

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-404-01-rifle-mesh-transform]] — step 1, bp.
- [[task-404-02-doc-rifle-mesh-offset]] — step 2, doc.
- [[task-405-left-elbow-joint-target]] — point 2, runs after this task (the new grip position changes the left arm reach).
- [[bp-burst-rifle]] — the Blueprint whose RifleMesh and Muzzle change.
- [[burst-rifle]] — gdd node that records the in-hand transform (Q2).
