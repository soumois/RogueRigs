---
id: task-292-01-cpp-heavy-one-per-pack
description: "Set the Heavies and WideHeavies packs to 1 heavy and spread horde heavies along the front row (cpp)."
parent: task-292-heavy-one-per-pack
kind: task
status: done
---
# Director: one heavy per pack

- goal: In FRRWaveDirectorConfig::MakeDefault and MakeLastLevel: Heavies and WideHeavies heavy CountMin/CountMax 1 (front kept), weights unchanged; horde heavies (2..3, front) placed spread along the front row of the horde side (ends and middle) instead of the nearest adjacent cells. Update only the existing tests this breaks; no new test.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-spawner-director (vault/reference/tech/systems/enemies/sys-spawner-director.md), sys-spawner-director-defaults (vault/reference/tech/systems/enemies/sys-spawner-director/sys-spawner-director-defaults.md), bp-enemy-spawner (vault/reference/tech/blueprints/world/bp-enemy-spawner.md), sys-tests-flowfield-spawner (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-flowfield-spawner.md), enemies-index (vault/reference/gdd/enemies/enemies-index.md); files: Source/RogueRigs/Enemies/RRWaveDirector.h/.cpp, Source/RogueRigs/Tests/
- expected_output: code, updated tests, sys-spawner-director-defaults (and sys-spawner-director if the horde placement rule changes) updated with before and after
- done_when: build clean; RogueRigs.Spawner tests pass; config read back: every pack type at most 1 heavy; horde heavies not adjacent (report their cells from an existing test or log); docs updated; lint clean
- editor_state: closed (owner closes it, or Live Coding off)
- token_ceiling: 40000
- model: opus
- tokens: input 34, cache write 40919, cache read 484496, output 10093
- runs: 1
- milestone: M2
- result: Heavies and WideHeavies packs hold 1 heavy (was 2..3) in MakeDefault and MakeLastLevel; horde heavies spread along the front row (PrepareHorde, ends and middle; test cells (96,138) (109,138) (134,138)); weights 96/4 and alive cap 3 unchanged; build clean; RogueRigs.Spawner 20/20 pass; DirectorHorde gained a non-adjacency check; not seen in game.

## Relations
- [[task-292-heavy-one-per-pack]] — parent global task.
