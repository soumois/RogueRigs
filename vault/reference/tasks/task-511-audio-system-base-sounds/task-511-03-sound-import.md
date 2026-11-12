---
id: task-511-03-sound-import
description: "Bp: Import the 8 picked sounds and fill the event table."
parent: task-511-audio-system-base-sounds
kind: task
status: todo
---
# Audio system and the first 8 sound effects: bp

- goal: Import the 8 picked sounds and fill the event table.
- agent: bp
- depends_on: task-511-02-sound-candidates
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Content/RogueRigs/Audio/
- expected_output: see done_when
- done_when: A4 holds; the temporary test sounds are gone from the table; a -game run log shows A2 and A3 with the kept sounds.
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
- [[task-511-audio-system-base-sounds]] — parent global task.
