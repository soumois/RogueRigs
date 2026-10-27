---
id: task-328-03-aim-indicator-component
description: "C++ aim indicator decal component on the miner: follows the aim, fixed length, stops at the first rock block, hidden at death; no capture."
parent: task-328-aim-chevrons
kind: task
status: done
---
# Aim chevrons: C++ component

- goal: the miner shows the chevron decal of [[aim-indicator]] in game, oriented by the aim each frame.
- agent: cpp
- depends_on: task-328-02-chevron-material
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-input-aim (vault/reference/tech/systems/core/sys-player/sys-player-input-aim.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md: decal component pattern, `ARRDecalWarmUp`, `DisableDecalsOn`, hiding rules), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md: `ARRArenaGrid::HasLineOfSight` and cell stepping along the aim), the change summary of task-328-02.
- expected_output: a decal component (UDecalComponent subclass, as `URRGroundTelegraphComponent`) as a default subobject of `ARRMiner`, material default `/Game/RogueRigs/FX/M_AimChevrons` by config path (missing asset: one warning, nothing drawn). Each frame (event-driven where possible, tick acceptable: it moves with the aim) it is placed at the miner's feet, yawed to `GetAimDirection()`, projected straight down, and writes `VisibleLength` = the smaller of the fixed length and the 2D distance to the first rock block on the aim line (owner 2026-10-27; no grid: fixed length). Hidden when the miner is dead (in `HandleDeath` and when `IsDead`); shown otherwise, mouse and stick alike (owner 2026-10-27). Decal warm-up through `ARRDecalWarmUp`. No brightness compensation for the light mask (owner: "Laisser s'éteindre"). No collision, no effect on aim, fire, turret placement or drone. Values only from aim-indicator and the global `request`. No new automation test; only fix existing tests this change breaks. No `-game` capture and no screenshot comparison (owner 2026-10-27: "Pas de capture, lance juste l'éditeur que je puisse tester"): the Executor reports, and the main session launches the Unreal Editor for the owner to test. A change summary for the doc subtask, stating plainly that the look is unverified.
- done_when: build succeeds; existing automation tests pass; the component is a default subobject of `ARRMiner` that loads `M_AimChevrons` and writes its parameters (checked by reading the code and the build or test log, not by a capture); committed by explicit path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 82, cache write 279104, cache read 3956194, output 29949
- usage: turns 41, context start 11k peak 145k, resumes 0, 15 min, units 0.89 M
- runs: 1
- milestone: M3
- started: 2026-10-27T15:39:29Z
- waiting_for: none
- result: URRAimIndicatorComponent on ARRMiner (commit e87a5eb1), build OK, 271 tests pass, 2 old failures (JuiceKillDropsStreamingCubes, LevelUpUpgradeLoop) unrelated; fixed length 416 uu derived by cpp (last chevron tip), not a decided value; look unverified; material uasset had later saved edits, committed separately

## Relations
- [[task-328-aim-chevrons]] — parent global task.
