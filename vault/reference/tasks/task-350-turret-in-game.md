---
id: task-350-turret-in-game
description: "Global task: The placed turret shows the Meshy base and a head yawing toward its target instead of the grey box, shots leaving the canon."
parent: tasks-to-do-index
kind: task
status: todo
---
# Turret in game: Meshy base and rotating head replace the grey box

- request: Owner message 2026-10-28 (relayed by the main session, English translation of his French message): "Ok, integrate the turret in game now (if you open Unreal, leave it open)." Input: the meshes of [[task-349-turret-mobile-head]]: `Art/Meshes/turret/SM_TurretBase.fbx` (slot `turret_body`) and `SM_TurretHead.fbx` (slots `turret_body`, `turret_canon`), centimetres, canon along +X at rest, head origin on the yaw axis, head offset on the base (0.501, 0.518, 65.20) cm in Blender axes (Unreal mirrors Y: (0.501, -0.518, 65.20), [[bp-drone-materials]]), 15,974 tris in total; body maps `Art/Meshes/turret/T_Turret_Body_BaseColor.png`, `_Normal.png`, `_Roughness.png`, `_Metallic.png`; canon maps `T_Turret_Canon_BaseColor.png`, `_Normal.png`, `_Roughness.png`; details in [[art-prompts-turret]] ("Game mesh" and "Base and head"). Owner answer 2026-10-28 (relayed by the main session): "ok" = every default of Q1 to Q6 accepted; he was told to save his Unreal work before answering, treated as done (the editor may be closed for subtask 01).
- owner_instruction: if Unreal is opened during this task, it is LEFT OPEN at the end: no subtask and no Executor step closes the editor after the bp subtasks (owner 2026-10-28).
- goal: In game, a placed turret is drawn with the Meshy base and head at the size the owner validated for the grey box; only the head turns toward the locked robot; bullets and the muzzle flash leave the canon; every turret rule (placement, stats, targeting, range, sight, lifetime, no collision) is unchanged.
- acceptance: BP_Turret shows SM_TurretBase and SM_TurretHead with MI_TurretBody and MI_TurretCanon (Q5), at the size of Q2; the base keeps its placement yaw while the head yaws toward the target (Q3); bullets and flash start at the canon tip at the current shot height (Q4); no collision, no cast shadow; `RogueRigs.Turret.*` and `Integration.PlaceTurretInputKillsRobot` pass; -game frames before and after sent to the owner; sys-turret, bp-turret and the new turret material doc updated; editor left open; lint clean; committed.
- gdd_check: read 2026-10-28: [[turret]] (approved: placement button, stat copy, nearest-target lock with sight, invincible, no collision; drawn 50 % larger than the first grey box, owner 2026-10-06), [[sys-turret]] and the code `Source/RogueRigs/Weapons/RRTurret.cpp` (Tick calls `SetActorRotation` toward the target: today the WHOLE actor turns, so a fixed base needs C++; `GetShotOrigin` = actor + `ShotHeight` 50 uu, also used for range and line of sight; `OnTurretFired(Origin, Direction)` drives the flash), [[bp-turret]] (grey box: EventTick also yaws TurretHead and orbits the barrel and flash in BP; MCP cannot re-parent components), [[sys-drone-parts]] (precedent: moving parts are C++ constructor components driven in Tick), [[bp-objective-console]] and [[bp-drone-materials]] (import precedents: FBX in cm, materials and textures not imported by the FBX, Nanite off, lightmap UVs off, Y mirrored), [[bp-robot-meshy-material]] (Meshy robots use base colour only, no normal map), [[lights-no-shadows]] (every `ARRWeapon` primitive, Blueprint ones included, gets no cast shadow in C++ `PostInitializeComponents`: nothing to set, the bp subtask checks it; the turret has no blob shadow today and none is added), [[sys-tests-weapon-types]] (`RogueRigs.Turret.Placement` checks the actor yaw at placement; must still pass). Hard rules: gameplay in C++, Blueprint thin, Blueprint docs through the doc subtask. No GDD conflict; no rule changes (Q3, Q4 keep timing, range and sight). Questions asked 2026-10-28, all DECIDED by the owner "ok" the same day (values provisional, owner 2026-10-28):
  - Q1 editor: the C++ build needs the editor closed. Default: if it is open when the task starts, the Executor closes it for subtask 01 (save your work in it before confirming), reopens it for the bp subtasks and leaves it open at the end, as you asked. DECIDED: default (owner work saved).
  - Q2 size: default and recommendation: uniform scale so the new turret is as tall as today's grey-box turret (top of the head about 96 uu, the 50 % enlargement you validated), read back from BP_Turret by the bp agent; the footprint follows (about 115 uu wide, under 2 cells of 67 uu). DECIDED: default.
  - Q3 head turn: default and recommendation: the head snaps to the target as the whole turret does today (it fires as soon as it locks, no rule change); the base keeps the yaw it had at placement (facing the aim). DECIDED: default.
  - Q4 shot start: default and recommendation: bullets and the flash start at the canon tip (its horizontal position as the head turns), at today's shot height 50 uu; range and line of sight still measured from the turret centre, so no gameplay change. DECIDED: default.
  - Q5 materials: default and recommendation: one material M_TurretMeshy with two instances, MI_TurretBody (body base colour, roughness and metallic maps) and MI_TurretCanon (canon base colour and roughness, metallic 0); the Meshy normal maps are NOT used, like the robots, so the faces flattened by task-347 stay flat on screen. DECIDED: default.
  - Q6 muzzle flash: default: the current white-gold sphere flash is kept, moved to the canon tip; no new effect. DECIDED: default.
- confirmed_by_owner: yes (2026-10-28, "ok" to the plan and every default of Q1-Q6, relayed by the main session)
- milestone: M2
- next_step: task-350-01-turret-head-yaw
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-350-01-turret-head-yaw (vault/reference/tasks/task-350-turret-in-game/task-350-01-turret-head-yaw.md) — cpp: before capture; ARRTurret keeps its placement yaw and turns a C++ HeadPivot toward the target, Muzzle on the head, shots from the canon tip.
2. task-350-02-turret-import (vault/reference/tasks/task-350-turret-in-game/task-350-02-turret-import.md) — bp: import the turret textures, M_TurretMeshy with MI_TurretBody and MI_TurretCanon, SM_TurretBase and SM_TurretHead with their slots.
3. task-350-03-bp-turret-meshes (vault/reference/tasks/task-350-turret-in-game/task-350-03-bp-turret-meshes.md) — bp: BP_Turret shows base and head meshes on the C++ pivots, grey box and EventTick orbit removed.
4. task-350-04-turret-capture (vault/reference/tasks/task-350-turret-in-game/task-350-04-turret-capture.md) — cpp: -game after capture and turret tests, editor left open.
5. task-350-05-turret-doc (vault/reference/tasks/task-350-turret-in-game/task-350-05-turret-doc.md) — doc: update sys-turret and bp-turret, create bp-turret-materials.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-350-01-turret-head-yaw]] — step 1, cpp.
- [[task-350-02-turret-import]] — step 2, bp.
- [[task-350-03-bp-turret-meshes]] — step 3, bp.
- [[task-350-04-turret-capture]] — step 4, cpp.
- [[task-350-05-turret-doc]] — step 5, doc.
- [[task-349-turret-mobile-head]] — produced the two meshes.
