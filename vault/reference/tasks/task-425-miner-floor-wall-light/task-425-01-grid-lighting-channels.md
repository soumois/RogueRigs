---
id: task-425-01-grid-lighting-channels
description: "Cpp: floor and terrain block render components on lighting channels 0 and 1"
parent: task-425-miner-floor-wall-light
kind: task
status: todo
---
# Miner light that lights only the floor and the walls: cpp

- goal: In ARRArenaGrid, every floor chunk HISM and every terrain block render component (rock piece ISMs, ore and heal-ore crystal HISMs, special ore) gets LightingChannels channel0 true and channel1 true (decided per Q4; barrels stay channel 0 only); collision ISMs untouched. Set it where chunk components are created from the templates (CreateChunkComponent), one place, with a header comment. Nothing else on the grid changes.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-rock-pieces (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-pieces.md), sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), agent-cpp (vault/reference/agents/agent-cpp.md), vault/reference/tasks/task-425-miner-floor-wall-light.md (request, Questions)
- expected_output: C++ change in Source/RogueRigs/Grid/, build green, existing grid tests pass, change summary for the doc subtask, commit.
- done_when: Editor build succeeds; existing RogueRigs grid automation tests pass; code sets channel1 on floor and block render chunk components only (barrel and collision components excluded); committed.
- editor_state: Unreal editor closed for the build.
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-425-miner-floor-wall-light]] — parent global task.
