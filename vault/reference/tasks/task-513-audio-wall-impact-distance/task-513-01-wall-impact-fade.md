---
id: task-513-01-wall-impact-fade
description: "Cpp: separate wall impact event with a distance fade from the player, values in the table"
parent: task-513-audio-wall-impact-distance
kind: task
status: todo
---
# Impact sur un mur : plus discret et selon la distance au joueur: cpp

- goal: separate wall impact event with a distance fade from the player, values in the table
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), task-511-audio-system-base-sounds (vault/reference/tasks/task-511-audio-system-base-sounds.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 to A3 of the global hold; the fade is a per-line option of the event table usable by any event, used only by the wall impact line; commit.
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
- [[task-513-audio-wall-impact-distance]] — parent global task.
