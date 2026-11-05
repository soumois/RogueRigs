---
id: task-444-02-doc
description: "Doc: Record the rotor shadow flags."
parent: task-444-rotors-no-shadow
kind: task
status: done
---
# Extractor rotors cast no shadow: doc

- goal: Record the rotor shadow flags.
- agent: doc
- depends_on: task-444-01-rotor-shadow-off
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md)
- expected_output: see done_when
- done_when: Nodes state it; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-444
- usage: included in the executor run of task-444
- runs: 1
- milestone: M2
- started: 2026-11-05T21:10:39Z
- waiting_for: none
- result: bp-extraction-shuttle (shadows bullet, verified, relations) and lights-no-shadows (shuttle no longer uncovered, relation) updated in f15d8b90; lint clean.

## Relations
- [[task-444-rotors-no-shadow]] — parent global task.
