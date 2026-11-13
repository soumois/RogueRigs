---
id: task-514-01-death-fade
description: "Cpp: enemy death line uses the distance fade; sys-audio line updated"
parent: task-514-audio-enemy-death-distance
kind: task
status: todo
---
# Mort des ennemis selon la distance au joueur: cpp

- goal: enemy death line uses the distance fade; sys-audio line updated
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), task-513-audio-wall-impact-distance (vault/reference/tasks/task-513-audio-wall-impact-distance.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 to A4 of the global hold; starts after task-513 is done; commit.
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
- [[task-514-audio-enemy-death-distance]] — parent global task.
