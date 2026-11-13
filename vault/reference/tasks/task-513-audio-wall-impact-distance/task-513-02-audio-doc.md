---
id: task-513-02-audio-doc
description: "Doc: sys-audio updated: wall impact event, distance fade fields"
parent: task-513-audio-wall-impact-distance
kind: task
status: done
---
# Impact sur un mur : plus discret et selon la distance au joueur: doc

- goal: sys-audio updated: wall impact event, distance fade fields
- agent: doc
- depends_on: task-513-01-wall-impact-fade
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: see done_when
- done_when: A4 of the global holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-513
- usage: included in the executor run of task-513
- runs: 1
- milestone: M2
- started: 2026-11-13T08:47:36Z
- waiting_for: none
- result: Commit of sys-audio (Impact.Wall call site, distance_fade field with the measured values, rr.Audio.Play offset and rr.Audio.ScreenEdge, gotchas); lint clean. Owner retouch points in DefaultGame.ini line Impact.Wall: Volume (level near the player, 0.5), FadeInner (200), FadeOuter (1075 = side screen edge; top corners reach 2090). Executor run over the light budget (see subtask 01 result).

## Relations
- [[task-513-audio-wall-impact-distance]] — parent global task.
