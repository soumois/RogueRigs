---
id: task-516-01-explosion-event
description: "Cpp: explosion event at every explosion, fade measured from the edge, table line"
parent: task-516-audio-explosion-edge-distance
kind: task
status: in-progress
---
# Son des explosions selon la distance au bord de l'explosion: cpp

- goal: explosion event at every explosion, fade measured from the edge, table line
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), task-513-audio-wall-impact-distance (vault/reference/tasks/task-513-audio-wall-impact-distance.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1, A2, A4 (code part) hold with the line silent if no file; starts after task-513 is done; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T08:59:32Z
- waiting_for: none
- result: open

## Relations
- [[task-516-audio-explosion-edge-distance]] — parent global task.
