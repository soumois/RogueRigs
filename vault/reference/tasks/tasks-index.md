---
id: tasks-index
description: "Orchestrator work items, one node per task with goal, agent, inputs, expected output, token ceiling and status. Open to pick up or log work."
parent: root
kind: note
---
# Tasks

One global task per owner request (template `task-global.md`, id `task-<number>-<slug>`), with its subtasks in a folder of the same name (template `task.md`, id `task-<number>-<step>-<slug>`). Status lives in the nodes, so any session or account can resume from `next_step`. Done tasks stay until the milestone retrospective.

## Relations
- [[root]] — the vault map.
- [[task-000-enable-gas]] — enable the GAS plugin and modules; blocks all GAS code. Executed as task-004-01. Status done.
- [[task-001-arena-m1]] — M1 arena: layout JSON, C++ grid, grey-box map visible in the editor. Status done.
- [[task-002-cleanup-twinstick]] — delete TopDown and Strategy templates, TwinStick character on L_ArenaM1, 60 % rock layout. Status done.
- [[task-003-arena-80-paths]] — M1 arena 80 × 80 as a path network, map follow-ups, project rename. Status done.
- [[task-004-miner-gas]] — GAS enabled, C++ miner with ASC and CSV base stats replacing the TwinStick pawn. Status done.
- [[task-005-drone-orbit]] — drone actor circling the miner (grey-box sphere). Status done.
- [[task-006-dig]] — the drone digs pushed blocks (red laser, shake, drops, pickups) and the floor flicker fix. Status done.
- [[task-007-burst-rifle-dummy]] — burst rifle with GAS damage and an immobile target dummy. Status done.
- [[task-008-feel-fixes]] — owner PIE feedback: shot height, free movement, burst pause, pickups, shake, damage text, bigger dummy. Status done.
- [[task-009-chaser]] — chaser robot: player damage, i-frames, death, flow field, enemy base, spawner, HUD. Status done.
- [[task-010-damage-numbers-video]] — quick damage numbers above every enemy for the assignment video. Status done.
- [[task-011-explosions]] — feel tweaks, then explosions: barrel, kamikaze, chains, tests. Status in-progress.
