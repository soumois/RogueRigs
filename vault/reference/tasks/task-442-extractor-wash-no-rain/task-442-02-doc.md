---
id: task-442-02-doc
description: "Doc: Document the new emitter."
parent: task-442-extractor-wash-no-rain
kind: task
status: done
---
# Extractor rotor wash that does not read as rain: doc

- goal: Document the new emitter.
- agent: doc
- depends_on: task-442-01-swirls-emitter
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md)
- expected_output: see done_when
- done_when: The node states the current emitter; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-442
- usage: included in the executor run of task-442
- runs: 1
- milestone: M2
- started: 2026-11-05T20:43:00Z
- waiting_for: none
- result: bp-extractor-rotor-wash states the Swirls emitter, M_ExtractorRotorSwirl, the x4 scale and what stays unverified; lint clean.

## Relations
- [[task-442-extractor-wash-no-rain]] — parent global task.
