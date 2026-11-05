---
id: task-442-01-swirls-emitter
description: "Bp: The streaks emitter of the rotor wash is replaced by soft faint swirls."
parent: task-442-extractor-wash-no-rain
kind: task
status: todo
---
# Extractor rotor wash that does not read as rain: bp

- goal: The streaks emitter of the rotor wash is replaced by soft faint swirls.
- agent: bp
- depends_on: none
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: see done_when
- done_when: System compiles with no error, no velocity-aligned thin sprite left in it, User.Intensity still drives the spawn and alpha, count read back within the bound.
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
- [[task-442-extractor-wash-no-rain]] — parent global task.
