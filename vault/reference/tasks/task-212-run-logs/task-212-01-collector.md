---
id: task-212-01-collector
description: "cpp: URRRunLogSubsystem"
parent: task-212-run-logs
kind: task
status: todo
---
# collector

- goal: URRRunLogSubsystem (game instance, survives the map travel) collects per level: terrain destroyed by source and type, damage dealt per weapon, kills by robot type and source, upgrades picked in order, damage taken by source, XP collected; hooks on existing delegates, events over polling.
- agent: cpp
- depends_on: none
- inputs: run-log-schema (vault/reference/tech/pipelines/run-log-schema.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-progression (vault/reference/tech/systems/core/sys-progression.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (new RunLog/ folder), new doc node sys-run-log under systems-world-index.
- done_when: build succeeds; tests pass except the known failures; a -game log dump of the counters after some fighting and digging; doc written; committed.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-212-run-logs]] — parent global task.
