---
id: task-444-01-rotor-shadow-off
description: "Bp: The four rotors cast no shadow, the hull does."
parent: task-444-rotors-no-shadow
kind: task
status: todo
---
# Extractor rotors cast no shadow: bp

- goal: The four rotors cast no shadow, the hull does.
- agent: bp
- depends_on: none
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md)
- expected_output: see done_when
- done_when: Flags read back off on the four rotors of a spawned extractor and unchanged on the hull; compile and save with no error.
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
- [[task-444-rotors-no-shadow]] — parent global task.
