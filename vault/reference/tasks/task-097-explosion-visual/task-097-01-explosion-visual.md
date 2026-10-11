---
id: task-097-01-explosion-visual
description: "The visible explosion is about 5 % larger than its collision radius and holds its final state 0.1 s before fading."
parent: task-097-explosion-visual
kind: task
status: todo
---
# Explosion visual size and hold

- goal: The visible explosion is about 5 % larger than its collision radius and holds its final state 0.1 s before fading.
- agent: cpp
- depends_on: task-096-01-kamikaze-death-fuse
- inputs: explosions (vault/reference/gdd/terrain/explosions.md), sys-explosions (vault/reference/tech/systems/sys-explosions.md), bp-explosion-fx (vault/reference/tech/blueprints/bp-explosion-fx.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner decision 2026-10-10 (written in explosions): the visible part of every explosion (kamikaze, barrel, any source of the explosion system: ask nothing, apply to all, the Orchestrator will tell the owner) is about 5 % larger than the collision radius so the player is never hit outside what is shown, and it stays 0.1 s at its final state before it fades. Find how the visual is driven (sys-explosions, BP_ExplosionFX doc: radius passed to the Blueprint or Niagara, timeline); do the C++ side: the visual radius passed to the effect = collision radius x 1.05 (a data value, default 1.05, `VisualRadiusScale`), and the hold duration 0.1 s as data (`VisualHoldSeconds`) available to the effect (parameter or event). The collision radius and damage stay unchanged. If the hold or the scale lives in the Blueprint (timeline, Niagara user parameters) and cannot be driven from C++, finish the C++ part, document precisely what the Blueprint must change and report it: a bp subtask will be added. Tests: collision radius unchanged, visual radius = 1.05 x radius, hold value readable. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; C++ part done and the Blueprint part listed if needed; docs sys-explosions updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-097-explosion-visual]] — parent global task.
