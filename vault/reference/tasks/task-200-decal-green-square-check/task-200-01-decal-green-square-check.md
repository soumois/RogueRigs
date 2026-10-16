---
id: task-200-01-decal-green-square-check
description: "Second -game run of the barrel fuse and explosion: does the green square return on the first decal frame once shaders are cached? Report only."
parent: task-200-decal-green-square-check
kind: task
status: done
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
- model: opus
- tokens: input 40, cache write 72787, cache read 861396, output 13193
- runs: 1
- milestone: M1
- result: Done. Green decal square returns in two -game runs with a warm shader cache, on the first frame of each new decal (fuse start, explosion): not the shader compile; probable default decal material while a new dynamic material instance is not ready; crater black for that frame; ring (60 uu, 0.8 s) and bigger burst seen; nothing changed, owner to decide

## Relations
- [[task-200-decal-green-square-check]] — parent global task.
