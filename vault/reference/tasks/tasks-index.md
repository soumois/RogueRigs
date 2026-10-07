---
id: tasks-index
description: "Orchestrator work items, one node per task with goal, agent, inputs, expected output, token ceiling and status. Open to pick up or log work."
parent: root
kind: note
---
# Tasks

One global task per owner request (template `task-global.md`, id `task-<number>-<slug>`), with its subtasks in a folder of the same name (template `task.md`, id `task-<number>-<step>-<slug>`). Status lives in the nodes, so any session or account can resume from `next_step`. Done tasks stay until the milestone retrospective.

Resuming after the pause of 2026-10-06: read [[handover-2026-10-06]] first.

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
- [[task-023-grid-perf]] — stress test of 100 uu and 67 uu cells before choosing the grid size; report to the owner. Status done.
- [[task-024-weapons]] — rifle pierce, shotgun pellets, turret on a button, weapons as level-up cards. Status done.
- [[task-025-damage-numbers]] — pooled Niagara damage numbers, merge, bigger crits. Status done.
- [[task-026-pressure]] — wave director, horde banner, damage vignette, alert-bar icons, light halo test. Status done.
- [[task-027-juice]] — XP and ore ejection and suction, kamikaze flare, ring gauge, chevrons, heal ring. Status done.
- [[task-028-balance]] — larger heavy slam, shield chasers, doubled upgrade values, XP pacing. Status done.
- [[task-029-fine-grid]] — 67 uu cells: chunked rendering, incremental flow field, converted layout, arc mining by contact. Status done.
- [[task-030-pacing-feedback]] — first play test: enemies from 1 s, slower ramp, shooters blended, robots 20 % slower, XP retune. Status done.
- [[task-031-directional-spawn]] — second play test: dummy removed, closer spawns toward the run heading, robots 30 % slower, steadfast shooters, faster early levels. Status done.
- [[task-032-third-test]] — third play test: robot recycling, console locator, overtime, slower robots, faster heavy, English texts, upgrade weights, weapons every 5 levels, bigger turret. Status done.
- [[task-033-tests-speed]] — robots all at 180 uu/s; existing tests updated to the owner data changes. Status done.
- [[task-034-fourth-test]] — fourth play test: projectile hitbox, whole-vein mining, heal pickup, small terrain capsules, kamikaze, softer early waves, crowd, ammo HUD. Status done.
- [[task-035-heal-amount]] — a heal vein restores 60 HP. Status done.
- [[task-036-recycle-relocate]] — robots left behind are moved back ahead instead of removed. Status done.
- [[task-037-ammo-hud]] — weapon slot ammo HUD, reload and turret recharge, padlocks. Status done.
- [[task-038-shooter-detour]] — Shooter without line of sight walks around rock like the other robots until it can fire, never moving away from the miner. Status done.
- [[task-039-heavy-radius]] — Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows. Status done.
- [[task-040-locator-arrow]] — Console locator: real arrow about 64 px, pulsing, with the distance in metres. Status done.
- [[task-041-front-detour]] — In the front sector the spawn detour filter is off, so robots spawn beyond the rock the miner digs toward. Status done.
- [[task-042-hack-time]] — Console hack time 20 → 16 s. Status done.
- [[task-043-vein-shake]] — All blocks of a vein being mined shake together. Status done.
- [[task-044-fewer-kamikazes]] — At most 1 kamikaze per pack in one pack out of two; hordes 1–2 kamikazes (was 3–5). Status done.
