---
id: task-443-01-height-signal
description: "Cpp: The extractor gives its current height above the floor to the Blueprint when it changes, event driven."
parent: task-443-extractor-ground-blast
kind: task
status: done
---
# Extractor rotor blast on the ground: cpp

- goal: The extractor gives its current height above the floor to the Blueprint when it changes, event driven.
- agent: cpp
- depends_on: none
- inputs: sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), extraction (vault/reference/gdd/run/extraction.md)
- expected_output: see done_when
- done_when: Build ok; a value of 0 when landed and the rise height at the end of the rise, also while paused during takeoff; Extraction test family passes.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-443
- usage: included in the executor run of task-443
- runs: 1
- milestone: M2
- started: 2026-11-05T20:44:47Z
- waiting_for: none
- result: GetHeightAboveFloor (BlueprintPure) and OnHeightAboveFloorChanged (BlueprintImplementableEvent) on ARRShuttle: 0 at BeginPlay and landed, follows the rise to RiseHeight 400, also while paused (test Takeoff extended), holds its peak in the nose-down flight so the blast does not come back. StartTakeoff also sets every UNiagaraComponent of the actor to solo and tickable when paused, because the batched Niagara tick stops with the world (engine source read; effect while paused not yet observed). Build ok; RogueRigs.Extraction 7/7 passed.

## Relations
- [[task-443-extractor-ground-blast]] — parent global task.
