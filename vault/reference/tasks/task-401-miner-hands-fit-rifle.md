---
id: task-401-miner-hands-fit-rifle
description: "Global task: the miner left hand holds the rifle handguard by IK in ABP_Miner while the right hand holds the grip, in every clip."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Miner hands fitted to the burst rifle

- request: Owner, 2026-10-30, verbatim (French, relayed by the main session): "On commencer à intégrer le gun 3d dans le jeu et surtout essayer de repositionner les mains pour que cela fit." Main session reading: start integrating the 3D gun model into the game and, above all, reposition the character hands so they fit the gun. Point 2 of 2 ("et surtout essayer de repositionner les mains pour que cela fit"); runs after [[task-400-burst-rifle-in-hand]]. Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q5 accepted.
- goal: In game, in idle and in every run or walk direction, the miner right hand holds the rifle pistol grip and the left hand rests on the handguard, without stretched or crossed arms.
- acceptance: SM_BurstRifle has socket LeftHandGrip on the handguard; URRMinerAnimInstance gives a left hand IK target in the right hand bone space and an alpha (1 only when the held weapon mesh has LeftHandGrip); ABP_Miner applies a Two Bone IK on the left arm after locomotion; a -game capture reports the checklist; doc nodes updated by the doc step; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[miner]] (spec approved; asset line: Meshy SK_Miner, Mixamo rifle clips, ABP_Miner; "weapon hand socket open"), [[burst-rifle]] (approved; game mesh SM_BurstRifle 60 cm, muzzle +X, top +Z, pivot at the pistol grip hand hold, 9,892 tris; "miner hand socket open"), [[burst-rifle-mesh]] (`Art/Meshes/burst-rifle/SM_BurstRifle.fbx` and four `T_BurstRifle_*.png` 2048 maps exist, not yet imported into Unreal; task-395 rebake cancelled, Meshy maps accepted), [[bp-burst-rifle]] (placeholder cube RifleMesh 60x10x10, Muzzle at +40 X, visual only), [[bp-miner]] (SK_Miner + ABP_Miner_C; weapons on the capsule at WeaponAttachOffset (0, 20, -40), WeaponAttachSocketName None, shared by every weapon), [[sys-weapons]] (shot origin = floor + ShotHeight 50 uu along the aim, independent of the mesh pose: moving the mesh to the hand changes no game rule), [[sys-player]] (ARRMiner attaches every weapon to the same socket or offset), [[task-386-miner-mixamo-animation]] (cancelled by the owner; clips are Mixamo rifle clips "avec les mains déjà bien posées, plus ou moins"; T-pose blend space defect, owner commit 5e8a0735 "Fix bug animation" on BS_Miner_Locomotion afterwards). Conflicts: none. Open values the work needs: hand socket (Q4), which weapons go to the hand (Q2), asset paths (Q3), hand fit method (Q5), capture (Q6), shot height versus visible muzzle (Q7). Unreal already holds UE5 mannequin rifle clips (`Content/Characters/Mannequins/Anims/Rifle/`), not used: the owner Mixamo clips stay the base. Milestone M2 (art). Depends on task-400 (rifle and WeaponSocket_R). Questions are shared with task-400 (Q1 to Q7, task-400 node); Q5 is the method of this task.
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: task-401-04-hands-fit-capture
- started: 2026-10-31T05:33:59Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): every default below is accepted; values provisional, owner 2026-10-30.
- Q5 and Q6 of [[task-400-burst-rifle-in-hand]] apply here (hands fit method, capture); answered with that task.

## Subtasks
1. task-401-01-left-hand-ik-target (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-01-left-hand-ik-target.md) — cpp: URRMinerAnimInstance: left hand IK target in right hand space and alpha
2. task-401-02-rifle-left-grip-socket (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-02-rifle-left-grip-socket.md) — bp: SM_BurstRifle socket LeftHandGrip on the handguard
3. task-401-03-abp-left-hand-ik (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-03-abp-left-hand-ik.md) — bp: ABP_Miner: Two Bone IK on the left arm onto LeftHandGrip
4. task-401-04-hands-fit-capture (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-04-hands-fit-capture.md) — capture: game capture: both hands on the rifle, idle and four directions
5. task-401-05-doc-hands-fit-rifle (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-05-doc-hands-fit-rifle.md) — doc: update bp-miner and sys-player-classes, record LeftHandGrip in burst-rifle-mesh

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-401-01-left-hand-ik-target]] — step 1, cpp.
- [[task-401-02-rifle-left-grip-socket]] — step 2, bp.
- [[task-401-03-abp-left-hand-ik]] — step 3, bp.
- [[task-401-04-hands-fit-capture]] — step 4, capture.
- [[task-401-05-doc-hands-fit-rifle]] — step 5, doc.
- [[task-400-burst-rifle-in-hand]] — point 1, puts the rifle in the right hand; holds Q1 to Q7 (Q2 answered 2026-10-30).
