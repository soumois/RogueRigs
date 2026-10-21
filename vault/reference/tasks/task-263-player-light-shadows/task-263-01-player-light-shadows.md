---
id: task-263-01-player-light-shadows
description: "The player carries a warm light that is the only source of cast shadows (rock walls), like the reference game: the walls block it and release it when they fall."
parent: task-263-player-light-shadows
kind: task
status: todo
---
# Player light and wall shadows

- goal: The player carries a warm light that is the only source of cast shadows (rock walls), like the reference game: the walls block it and release it when they fall.
- agent: cpp
- depends_on: none
- inputs: lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), rock-state-render-options (vault/reference/tech/systems/world/sys-grid/rock-state-render-options.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), sys-drone-parts (vault/reference/tech/systems/combat/sys-drone/sys-drone-parts.md), perf-risks (vault/reference/tech/performance/perf-risks.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md)
- expected_output: C++ shadow flags (first subtask): the grid rock and deposit instanced meshes cast shadows (EditDefaultsOnly property), the floor does not; robots, pickups, the drone and the miner mesh do not cast shadows (so the only cast shadows are rocks lit by the player light); documented. Then a bp subtask: a movable warm point light on BP_Miner (light component, cast shadows true, attenuation radius about 1400 uu, height about 300 uu above the miner, warm colour, source radius small), values provisional for the owner to tune.
- done_when: Build passes; existing Dig, Ore, PulseDesync, FineContactDigOpensPassage and RunFlow tests pass; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-263-player-light-shadows]] — parent global task.
