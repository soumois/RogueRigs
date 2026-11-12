---
id: task-511-02-sound-candidates
description: "Sound: Find three CC0 candidates per event against the palette and get the owner's pick."
parent: task-511-audio-system-base-sounds
kind: task
status: in-progress
---
# Audio system and the first 8 sound effects: sound

- goal: Find three CC0 candidates per event against the palette and get the owner's pick.
- agent: sound
- depends_on: task-511-01-audio-system
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: Saved/Scratch/audio-candidates/<event>/ with 24 numbered files and one list (source, licence, duration, palette note); the owner's pick per event recorded in this node.
- done_when: The owner named one candidate per event (wait-answers until then; a request for another series gives a new series for that event only).
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-12T21:11:14Z
- waiting_for: none
- result: open

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
