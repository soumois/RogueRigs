---
id: task-443-01-height-signal
description: "Cpp: The extractor gives its current height above the floor to the Blueprint when it changes, event driven."
parent: task-443-extractor-ground-blast
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-443-extractor-ground-blast]] — parent global task.
