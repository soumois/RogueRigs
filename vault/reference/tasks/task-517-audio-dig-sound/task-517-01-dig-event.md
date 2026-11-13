---
id: task-517-01-dig-event
description: "Cpp: dig event and table line (silent until the file)"
parent: task-517-audio-dig-sound
kind: task
status: todo
---
# Son quand on creuse: cpp

- goal: dig event and table line (silent until the file)
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1, A2, A4 hold with the line silent if no file; commit.
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
- [[task-517-audio-dig-sound]] — parent global task.
