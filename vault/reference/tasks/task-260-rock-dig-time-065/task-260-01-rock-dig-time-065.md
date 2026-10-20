---
id: task-260-01-rock-dig-time-065
description: "Rock blocks take 0.65 s instead of 0.5 s to dig at mining speed 1."
parent: task-260-rock-dig-time-065
kind: task
status: todo
---
# Rock dig time 0.65 s

- goal: Rock blocks take 0.65 s instead of 0.5 s to dig at mining speed 1.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md)
- expected_output: ARRArenaGrid RockHealth default 0.5 -> 0.65 (provisional, owner 2026-10-20); no BP_ArenaGrid override (read the asset file); GDD drone.md and destruction rules text and sys docs updated; existing tests that expect 0.5 s updated.
- done_when: Build passes; RogueRigs.Dig, RogueRigs.Ore, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow pass; docs and lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-260-rock-dig-time-065]] — parent global task.
