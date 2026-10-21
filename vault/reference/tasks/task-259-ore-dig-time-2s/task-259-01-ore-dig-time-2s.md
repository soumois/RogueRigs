---
id: task-259-01-ore-dig-time-2s
description: "Ore blocks (ore, special ore and heal ore) take 2 s instead of 1.5 s to dig at mining speed 1."
parent: task-259-ore-dig-time-2s
kind: task
status: done
---
# Ore dig time 2 s

- goal: Ore blocks (ore, special ore and heal ore) take 2 s instead of 1.5 s to dig at mining speed 1.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md)
- expected_output: ARRArenaGrid OreHealth, HealOreHealth and SpecialOreHealth defaults 1.5 -> 2.0 (provisional, owner 2026-10-20); check that BP_ArenaGrid does not override them (read the asset file); deposit drop interval and loot-over-dig checked still consistent (drop interval versus health); GDD drone.md, ore-blocks.md, heal-ore.md and the sys docs updated; existing tests that expect 1.5 s updated.
- done_when: Build passes; RogueRigs.Dig, RogueRigs.Ore, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow pass; docs and lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: opus
- tokens: input 110, cache write 122135, cache read 4637221, output 8215
- runs: 1
- milestone: M1
- result: OreHealth, HealOreHealth and SpecialOreHealth 1.5 -> 2.0 (provisional, owner 2026-10-20); DropInterval also 1.5 -> 2.0 so non-deposit ore blocks keep one pickup at the break (beyond the brief, kept the old rule); no BP_ArenaGrid, map or table override (OreTypes.csv has no health column). Tests updated for 2 s (pops 30 frames apart, break at frame 120), HealVein test no longer miscounts a pooled pickup; 27 of 27 pass. GDD ore-blocks, heal-ore, drone and sys docs updated. Not seen in play; FineContactDigOpensPassage doc numbers older than the measured ones.

## Relations
- [[task-259-ore-dig-time-2s]] — parent global task.
