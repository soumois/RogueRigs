---
id: task-299-01-cpp-kamikaze-fuse-shake
description: "Jitter the kamikaze VisualRoot during the fuse with the rock shake amplitudes, frequency rising 8 to 24 Hz (cpp)."
parent: task-299-kamikaze-fuse-shake
kind: task
status: todo
---
# Kamikaze fuse shake

- goal: ARRKamikaze: while Armed (proximity or kill fuse), add to VisualRoot a horizontal and vertical jitter with the rock shake amplitudes (ShakeAmplitude 8, ShakeVerticalAmplitude 3 of ARRArenaGrid; read the code for the exact form and reuse it), frequency from 8 Hz to 24 Hz over the fuse (EditDefaultsOnly, code defaults); hover bob and core scale (ArmedCoreScale as built) unchanged; offset cleared on explosion, death and ResetForNewLife. Update only the tests this breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-kamikaze (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-kamikaze.md), sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), bp-kamikaze (vault/reference/tech/blueprints/enemies/bp-kamikaze.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md); files: Source/RogueRigs/Enemies/RRKamikaze.h/.cpp, Source/RogueRigs/Grid/RRArenaGrid.h/.cpp (read only)
- expected_output: code, sys-enemies-kamikaze updated (visuals line) with before and after
- done_when: build clean; kamikaze tests pass (armed drift under 1 uu); shake offset timeline over a fuse reported (amplitude, rate at start and end); docs updated; lint clean
- editor_state: closed (owner closes it, or Live Coding off)
- token_ceiling: 35000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-299-kamikaze-fuse-shake]] — parent global task.
