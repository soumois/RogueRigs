---
id: task-522-01-distance-sound-classes
description: "Cpp: generic distance sound base class and explosion child; existing faded rows moved onto them"
parent: task-522-audio-distance-sound-rotors
kind: task
status: done
---
# Son des hélices de l'extracteur et code générique de son à distance: cpp

- goal: generic distance sound base class and explosion child; existing faded rows moved onto them
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), task-516-audio-explosion-edge-distance (vault/reference/tasks/task-516-audio-explosion-edge-distance.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 holds; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-522
- usage: included in the executor run of task-522
- runs: 1
- milestone: M2
- started: 2026-11-13T13:34:31Z
- waiting_for: none
- result: Done (7ee67e99). A1: Audio/RRDistanceSound.h/.cpp: FRRDistanceSound (point; PlayAt one-shot, UpdateLoop/StopLoop for loop rows with level following the distance each tick, start/stop at FadeOuter, pause stop via the subsystem; GetDistance virtual; GetFade) and child FRRExplosionSound (distance minus Radius, floor 0). Subsystem fade now measured by the distance sound (GetFadeDistance/GetDistanceFade removed, PlayEventAt without EdgeRadius, StartDistanceLoop/ReleaseDistanceLoop added). Impact.Wall (RRProjectile), EnemyDeath (RREnemy), Explosion (RRExplosionSubsystem) and rr.Audio.Play go through them. -game Verbose LogRRAudio: Impact.Wall 0/200/600/1075 uu = 0.500/0.500/0.271/Skip, EnemyDeath 1.000/1.000/0.543/Skip, Explosion radius 300 at edge 0/200/600/1075 = 1.000/0.814/0.442/Skip: same as the previous formula. Build green, no new warning (verify_scope: build only + rr.Audio.Play check; no audio test family).

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
