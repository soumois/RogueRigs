---
id: task-200-01-decal-green-square-check
description: "Second -game run of the barrel fuse and explosion: does the green square return on the first decal frame once shaders are cached? Report only."
parent: task-200-decal-green-square-check
kind: task
status: in-progress
---
# Decal green square check

- goal: Second -game run of the barrel fuse and explosion: does the green square return on the first decal frame once shaders are cached? Report only.
- agent: cpp
- depends_on: task-198-02-shockwave-ring-width, task-199-01-barrel-burst-bigger (so its frames also show them)
- inputs: sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: frames in `Saved/Screenshots/` (not committed), the command line, a report.
- done_when: frames at the fuse start and the explosion exist and were looked at; the report says whether the green square appears and its likely cause; nothing committed by the agent.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-200-decal-green-square-check]] — parent global task.
