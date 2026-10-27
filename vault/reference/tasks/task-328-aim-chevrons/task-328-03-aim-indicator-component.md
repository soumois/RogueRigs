---
id: task-328-03-aim-indicator-component
description: "C++ aim indicator decal component on the miner: follows the aim, fixed length, stops at the first rock block, hidden at death; one -game capture."
parent: task-328-aim-chevrons
kind: task
status: todo
---
# Aim chevrons: C++ component and capture

- goal: the miner shows the chevron decal of [[aim-indicator]] in game, oriented by the aim each frame.
- agent: cpp
- depends_on: task-328-02-chevron-material
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-input-aim (vault/reference/tech/systems/core/sys-player/sys-player-input-aim.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md: decal component pattern, `ARRDecalWarmUp`, `DisableDecalsOn`, hiding rules), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md: `ARRArenaGrid::HasLineOfSight` and cell stepping along the aim), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), the change summary of task-328-02.
- expected_output: a decal component (UDecalComponent subclass, as `URRGroundTelegraphComponent`) as a default subobject of `ARRMiner`, material default `/Game/RogueRigs/FX/M_AimChevrons` by config path (missing asset: one warning, nothing drawn). Each frame (event-driven where possible, tick acceptable: it moves with the aim) it is placed at the miner's feet, yawed to `GetAimDirection()`, projected straight down, and writes `VisibleLength` = the fixed length, or the 2D distance to the first rock block on the aim line if Q1 is kept (no grid: fixed length). Hidden when the miner is dead (in `HandleDeath` and when `IsDead`); shown otherwise, mouse and stick alike (per Q3). Decal warm-up through `ARRDecalWarmUp`. No collision, no effect on aim, fire, turret placement or drone. Values only from the global `request` answers. No new automation test; only fix existing tests this change breaks. Then one `-game` capture on L_ArenaM1 with the chevrons visible and one rock block on the aim line, compared with the owner base screenshot `Art/Source/aim-indicator/base-screenshot.png`, sent to the owner (per Q5). A change summary for the doc subtask.
- done_when: build succeeds; existing automation tests pass; the capture shows white-gold chevrons in front of the miner pointing along the aim and stopping before the rock block (if Q1 kept); committed by explicit path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-328-aim-chevrons]] — parent global task.
