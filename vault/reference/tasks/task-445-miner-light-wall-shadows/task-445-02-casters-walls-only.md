---
id: task-445-02-casters-walls-only
description: "Cpp: From this light, only rock, ore and heal ore blocks cast shadows; anything found casting that should not is corrected."
parent: task-445-miner-light-wall-shadows
kind: task
status: done
---
# Miner light casts wall shadows only: cpp

- goal: From this light, only rock, ore and heal ore blocks cast shadows; anything found casting that should not is corrected.
- agent: cpp
- depends_on: task-445-01-light-shadows
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), perf-risks (vault/reference/tech/performance/perf-risks.md)
- expected_output: see done_when
- done_when: Build ok; a code read and a run read show block pieces cast, and robots, miner, drone, pickups, barrels, chest and ore crystals do not from this light; Grid test family passes; a report if nothing had to change.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-445
- usage: included in the executor run of task-445
- runs: 1
- milestone: M2
- started: 2026-11-05T21:15:07Z
- waiting_for: none
- result: No code change needed. Code read: rock pieces (RockInstances template) cast, ch 0+1 (bBlocksCastShadows true); ore and heal ore crystals cast nothing (bDepositCrystalsCastShadow false); engine FLightSceneInfoCompact::AffectsPrimitive and the Nanite shadow mask drop any caster sharing no channel with the light, so barrels (cast on, ch 0 only) keep the sun shadow and cast nothing from this ch 1 light. Asset scan: only BP_Miner serializes lighting channels, so chest, console and shuttle stay ch 0. PIE read on L_ArenaM1: 634 rock piece components cast ch 110, floor and crystals cast false ch 110, robots, miner, drone, pickups, rifle, asteroids cast false ch 100, barrels cast true ch 100; chest not spawned at the read (code and asset only). Build OK; RogueRigs.Dig 19/19 (RogueRigs.Grid family does not exist, same substitution as task-425). Render not checked: A6, owner looks.

## Relations
- [[task-445-miner-light-wall-shadows]] — parent global task.
