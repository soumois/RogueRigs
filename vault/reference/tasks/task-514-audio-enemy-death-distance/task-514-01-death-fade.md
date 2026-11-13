---
id: task-514-01-death-fade
description: "Cpp: enemy death line uses the distance fade; sys-audio line updated"
parent: task-514-audio-enemy-death-distance
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-514
- usage: included in the executor run of task-514
- runs: 1
- milestone: M2
- started: 2026-11-13T08:48:56Z
- waiting_for: none
- result: Commit 0a7f44e9. A1: DefaultGame.ini EnemyDeath row adds FadeInner=200, FadeOuter=1075, Volume 1.0 unchanged (Q5 defaults). A2: -game run on L_ArenaM1 with Verbose LogRRAudio: rr.Audio.Play EnemyDeath at 100/650/1050/2000 uu -> volume 1.000/0.486/0.029/Skip; a real robot death at 1560 uu logged Skip. A3: config line only, no code change: build green (no test run per build-toolchain Test scope; no audio test family exists). A4: sys-audio distance_fade lists Impact.Wall and EnemyDeath as faded rows. Owner retouch points: EnemyDeath line in Config/DefaultGame.ini, Volume (1.0), FadeInner (200), FadeOuter (1075).

## Relations
- [[task-514-audio-enemy-death-distance]] — parent global task.
