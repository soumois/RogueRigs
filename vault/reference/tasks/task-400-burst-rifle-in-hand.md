---
id: task-400-burst-rifle-in-hand
description: "Global task: import SM_BurstRifle, put it on BP_BurstRifle in a miner right hand socket, remove the weapon grey cubes."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Burst rifle 3D mesh in the miner right hand

- request: Owner, 2026-10-30, verbatim (French, relayed by the main session): "On commencer à intégrer le gun 3d dans le jeu et surtout essayer de repositionner les mains pour que cela fit." Main session reading: start integrating the 3D gun model into the game and, above all, reposition the character hands so they fit the gun. Point 1 of 2 ("intégrer le gun 3d dans le jeu"); point 2 is [[task-401-miner-hands-fit-rifle]]. Owner answer to Q2, 2026-10-30, verbatim (French, relayed by the main session): "Q2 : Tu retires le cube, les autres tirs partiront du même endroit." Reading: the grey cube is removed from the weapons (the BP_Shotgun ShotgunMesh is the only other cube; BP_TurretLauncher has no mesh); the other weapons keep the same shot origin (the shared floor + ShotHeight point of `GetShotOrigin`, unchanged). Q1, Q3 to Q7 not answered yet. Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q1, Q3, Q4, Q5, Q6, Q7 accepted.
- goal: In game the miner holds the Meshy burst rifle (SM_BurstRifle) in his right hand instead of the grey cube on the capsule; game rules unchanged.
- acceptance: Unreal holds SM_BurstRifle (9,892 tris, 60 cm) with M_BurstRifle from the four maps; ARRWeapon has a per-weapon hand socket used by ARRMiner when set; SKEL_Miner has socket WeaponSocket_R on the right hand; BP_BurstRifle shows SM_BurstRifle (cube removed), Muzzle at the mesh muzzle tip, attached to WeaponSocket_R; BP_Shotgun has no ShotgunMesh cube (owner Q2); shotgun and turret launcher keep the capsule offset and every weapon keeps the shared shot origin; shot origin unchanged; a -game capture reports the checklist; doc nodes updated by the doc step; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[miner]] (spec approved; asset line: Meshy SK_Miner, Mixamo rifle clips, ABP_Miner; "weapon hand socket open"), [[burst-rifle]] (approved; game mesh SM_BurstRifle 60 cm, muzzle +X, top +Z, pivot at the pistol grip hand hold, 9,892 tris; "miner hand socket open"), [[burst-rifle-mesh]] (`Art/Meshes/burst-rifle/SM_BurstRifle.fbx` and four `T_BurstRifle_*.png` 2048 maps exist, not yet imported into Unreal; task-395 rebake cancelled, Meshy maps accepted), [[bp-burst-rifle]] (placeholder cube RifleMesh 60x10x10, Muzzle at +40 X, visual only), [[bp-miner]] (SK_Miner + ABP_Miner_C; weapons on the capsule at WeaponAttachOffset (0, 20, -40), WeaponAttachSocketName None, shared by every weapon), [[sys-weapons]] (shot origin = floor + ShotHeight 50 uu along the aim, independent of the mesh pose: moving the mesh to the hand changes no game rule), [[sys-player]] (ARRMiner attaches every weapon to the same socket or offset), [[task-386-miner-mixamo-animation]] (cancelled by the owner; clips are Mixamo rifle clips "avec les mains déjà bien posées, plus ou moins"; T-pose blend space defect, owner commit 5e8a0735 "Fix bug animation" on BS_Miner_Locomotion afterwards). Conflicts: none. Open values the work needs: hand socket (Q4), which weapons go to the hand (Q2, answered: cubes removed, same shot origin), asset paths (Q3), hand fit method (Q5), capture (Q6), shot height versus visible muzzle (Q7). Unreal already holds UE5 mannequin rifle clips (`Content/Characters/Mannequins/Anims/Rifle/`), not used: the owner Mixamo clips stay the base. Milestone M2 (art).
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: task-400-03-miner-hand-socket
- started: 2026-10-30T23:54:46Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): the defaults of Q1, Q3, Q4, Q5, Q6, Q7 are accepted; values provisional, owner 2026-10-30.
- Q1 Split: your sentence holds two points, so two global tasks: task-400 the 3D rifle in the right hand, task-401 the hands fitted to it (left hand on the handguard by IK). task-401 runs after task-400. Default: yes, two tasks.
- Q2 (answered 2026-10-30, "Q2 : Tu retires le cube, les autres tirs partiront du même endroit.") Which weapons go to the hand: only the burst rifle, through a per-weapon socket name on ARRWeapon (C++); the grey ShotgunMesh cube is removed from BP_Shotgun (subtask 05), the turret launcher has no mesh; every weapon keeps the shared shot origin (floor + ShotHeight 50 uu along the aim); the shotgun MuzzleFlash stays where it is.
- Q3 Unreal assets: Default: `/Game/RogueRigs/Weapons/BurstRifle/` with SM_BurstRifle (scale 1, 60 cm, no decimation), M_BurstRifle with base colour, normal, metallic and roughness maps, T_BurstRifle_* textures; no collision.
- Q4 Hand socket: Default: socket `WeaponSocket_R` on the right hand bone of SKEL_Miner, placed so the pistol grip sits in the palm and the muzzle points along the aim in Rifle Aiming Idle; values set by the bp agent from the bone axes, provisional; you can retouch it in the Skeleton editor.
- Q5 Hands fit method (task-401): Default A: right hand = the socket of Q4; left hand moved at runtime by a Two Bone IK in ABP_Miner onto a socket `LeftHandGrip` on the rifle handguard, target computed in C++ in the right hand space (no lag), for all nine clips; fingers as in the clips. Or B: edit the arms in the nine clips in Blender (heavier, per clip).
- Q6 Visual check: Default: each task ends with a -game capture by the capture agent against a checklist, frames sent to you (rule 2026-10-16). Or you check in the editor yourself and the capture subtasks are removed.
- Q7 Shot height: the bullets start at 50 uu above the floor (below the 100 uu rocks), while the rifle in the hand is higher, so on screen the bullets leave below the barrel. Default: keep the rule (rules versus rendering), only the visible Muzzle point moves to the mesh tip.

## Subtasks
1. task-400-01-weapon-hand-socket (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-01-weapon-hand-socket.md) — cpp: per-weapon hand socket on ARRWeapon, used by ARRMiner when attaching
2. task-400-02-burst-rifle-import (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-02-burst-rifle-import.md) — bp: import SM_BurstRifle with M_BurstRifle and its four textures
3. task-400-03-miner-hand-socket (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-03-miner-hand-socket.md) — bp: SKEL_Miner socket WeaponSocket_R on the right hand bone
4. task-400-04-bp-burst-rifle-mesh (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-04-bp-burst-rifle-mesh.md) — bp: BP_BurstRifle shows SM_BurstRifle, Muzzle at the tip, hand socket WeaponSocket_R
5. task-400-05-shotgun-cube-removal (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-05-shotgun-cube-removal.md) — bp: remove the grey ShotgunMesh cube from BP_Shotgun (owner Q2)
6. task-400-06-rifle-in-hand-capture (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-06-rifle-in-hand-capture.md) — capture: game capture: the 3D rifle in the right hand, idle and running, no shotgun cube
7. task-400-07-doc-burst-rifle-in-hand (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-07-doc-burst-rifle-in-hand.md) — doc: update bp-burst-rifle, bp-shotgun, bp-miner, sys-weapons, sys-player-classes

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-400-01-weapon-hand-socket]] — step 1, cpp.
- [[task-400-02-burst-rifle-import]] — step 2, bp.
- [[task-400-03-miner-hand-socket]] — step 3, bp.
- [[task-400-04-bp-burst-rifle-mesh]] — step 4, bp.
- [[task-400-05-shotgun-cube-removal]] — step 5, bp.
- [[task-400-06-rifle-in-hand-capture]] — step 6, capture.
- [[task-400-07-doc-burst-rifle-in-hand]] — step 7, doc.
- [[task-401-miner-hands-fit-rifle]] — point 2, hands fitted to this rifle.
- [[burst-rifle-mesh]] — the mesh files imported here.
- [[bp-burst-rifle]] — the Blueprint whose mesh changes.
- [[bp-shotgun]] — the Blueprint whose cube is removed.
