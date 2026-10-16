---
id: task-201-01-decal-first-frame-green
description: "No green default-material frame when a ground telegraph or explosion shockwave decal appears: decal materials ready before first use, verified in -game frames."
parent: task-201-decal-first-frame-green
kind: task
status: done
---
# Decal first frame green

- goal: No green default-material frame when a ground telegraph or explosion shockwave decal appears: decal materials ready before first use, verified in -game frames.
- agent: cpp
- depends_on: none
- inputs: sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ fix, docs; frames in `Saved/Screenshots/` (not committed); one or two commits.
- done_when: build exit 0; explosion, barrel and telegraph tests pass (only broken expectations updated, no new test); the task-200 `-game` command (new prefix) shows no green square at fuse start and at the explosion; the cause found is written in the docs; game rules unchanged; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 140, cache write 183464, cache read 7931768, output 51040
- runs: 2
- milestone: M1
- result: Cause: uncooked material shader map incomplete on first decal draw, renderer falls back to the default decal material (green box). Fix: ARRDecalWarmUp draws each decal material once at level start (2 s, invisible), telegraph and shockwave materials warmed, one barrel telegraph created at start; 21/21 tests; -game frames barrelfx4_01 and _11 show no green; commits 38fcc48e, 57d10dc1

## Relations
- [[task-201-decal-first-frame-green]] — parent global task.
