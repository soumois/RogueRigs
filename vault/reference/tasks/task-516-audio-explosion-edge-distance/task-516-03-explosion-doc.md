---
id: task-516-03-explosion-doc
description: "Doc: sys-audio: explosion event, edge distance, placeholder status"
parent: task-516-audio-explosion-edge-distance
kind: task
status: done
---
# Son des explosions selon la distance au bord de l'explosion: doc

- goal: sys-audio: explosion event, edge distance, placeholder status
- agent: doc
- depends_on: task-516-02-explosion-import
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: see done_when
- done_when: A4 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-516
- usage: included in the executor run of task-516
- runs: 1
- milestone: M2
- started: 2026-11-13T09:05:44Z
- waiting_for: none
- result: Commit 62548c23. sys-audio: Explosion call site in URRExplosionSubsystem::Explode, PlayEventAt EdgeRadius and GetFadeDistance (distance minus radius, 0 inside), Explosion row values (Volume 1.0, FadeInner 0, FadeOuter 1075, Q6), rr.Audio.Play EdgeRadius argument, every kept sound placeholder 'à remplacer avant publication', relation to task-516; lint clean. ALERT at the 516-02 closing: executor run over the light budget (42 turns for 40, 0.53 M units of 0.6 M): cause, a C++ change plus a sound preparation and import plus two -game checks within one light task, and one task-next call that needed a manual restart of subtask 02 (next-subtask arguments given without the global).

## Relations
- [[task-516-audio-explosion-edge-distance]] — parent global task.
