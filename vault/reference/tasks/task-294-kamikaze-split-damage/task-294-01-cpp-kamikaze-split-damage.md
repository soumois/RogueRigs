---
id: task-294-01-cpp-kamikaze-split-damage
description: "Add a miner damage column to ExplosionStats; kamikaze 40 to robots and 30 to the miner (cpp)."
parent: task-294-kamikaze-split-damage
kind: task
status: done
---
# Separate miner damage for explosions

- goal: ExplosionStats.csv and FRRExplosionStatsRow: a miner damage value (barrel rows = their current Damage, so unchanged); Kamikaze Damage 30 -> 40, miner damage 30; URRExplosionSubsystem applies the miner value to ARRMiner targets, the other to robots; no-one-shot cap applied after. Update only the tests this breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md), sys-tests-explosions-barrel (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-explosions-barrel.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), explosions (vault/reference/gdd/terrain/explosions.md); files: Content/RogueRigs/Data/Tables/ExplosionStats.csv, Source/RogueRigs/Explosions/
- expected_output: CSV, code, updated tests, sys-explosions updated with before and after
- done_when: build clean; explosion and kamikaze tests pass; a test or log shows 40 on a robot and 30 on the miner from a kamikaze, barrel values unchanged; docs updated; lint clean
- editor_state: closed (owner closes it, or Live Coding off)
- token_ceiling: 35000
- model: opus
- tokens: input 30, cache write 55456, cache read 568769, output 12427
- runs: 1
- milestone: M2
- result: MinerDamage column in ExplosionStats; Kamikaze 40 to robots, 30 to the miner; barrel 40/40 unchanged; no-one-shot cap kept. Build clean, 25/25 tests (Explosions, Kamikaze, Barrel, BarrelFuse, 4 Integration). Commit da5a6ff9.

## Relations
- [[task-294-kamikaze-split-damage]] — parent global task.
