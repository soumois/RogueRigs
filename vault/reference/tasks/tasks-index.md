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
- [[task-011-explosions]] — feel tweaks, then explosions: barrel, kamikaze, chains, tests. Status done.
- [[task-012-xp-level]] — tougher slower kamikaze, slower chasers, render-only shake, XP and level-up without upgrades. Status done.
- [[task-013-stress-test]] — stress test in -game: baseline at 50 to 400 robots first, final S0, S2, S7 after task-016. Status done.
- [[task-014-upgrades-levelup]] — spawn packs (100 max, 15 % kamikazes), stat upgrades with rarities, level-up heal, pause, 3 cards, one reroll. Status done.
- [[task-015-shotgun]] — shotgun conical wave with knockback, given at start with the burst rifle. Status done.
- [[task-016-console-extraction]] — alert bar, command console hack, shuttle extraction, end screens. Status done.
- [[task-017-kamikaze-radius]] — kamikaze explosion radius 250 → 312 uu. Status done.
- [[task-018-robot-perf]] — robot count optimization: trace, crowd collision, light movement, hitch fix, final stress. Status done.
- [[task-019-packaging-fix]] — asset manager config so BuildCookRun exits 0. Status done.
- [[task-020-code-review]] — C++ review for readability and generalization, refactor, Blueprint check. Status done.
- [[task-021-shooter]] — shooter robot: distance, line of sight, repositioning, laser, shots, spawner mix. Status done.
- [[task-022-heavy]] — heavy robot: telegraphed ground slam, knockback resistance, corridor capsule, spawner mix and cap. Status done.
- [[task-023-grid-perf]] — stress test of 100 uu and 67 uu cells before choosing the grid size; report to the owner. Status todo.
- [[task-024-weapons]] — rifle pierce, shotgun pellets, turret on a button, weapons as level-up cards. Status todo.
- [[task-025-damage-numbers]] — pooled Niagara damage numbers, merge, bigger crits. Status todo.
- [[task-026-pressure]] — wave director, horde banner, damage vignette, alert-bar icons, light halo test. Status todo.
- [[task-027-juice]] — XP and ore ejection and suction, kamikaze flare, ring gauge, chevrons, heal ring. Status todo.
- [[task-028-balance]] — larger heavy slam, shield chasers, doubled upgrade values, XP pacing. Status todo.
