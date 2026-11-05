---
id: task-443-03-blueprint-wiring
description: "Bp: BP_ExtractionShuttle feeds the height to the four wash components."
parent: task-443-extractor-ground-blast
kind: task
status: todo
---
# Extractor rotor blast on the ground: bp

- goal: BP_ExtractionShuttle feeds the height to the four wash components.
- agent: bp
- depends_on: task-443-02-blast-emitter
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md)
- expected_output: see done_when
- done_when: Compiles and saves, no Event Tick, PIE on a landed extractor logs no error and reads the expected inputs.
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
