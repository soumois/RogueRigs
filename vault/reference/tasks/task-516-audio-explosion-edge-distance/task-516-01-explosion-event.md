---
id: task-516-01-explosion-event
description: "Cpp: explosion event at every explosion, fade measured from the edge, table line"
parent: task-516-audio-explosion-edge-distance
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-516
- usage: included in the executor run of task-516
- runs: 1
- milestone: M2
- started: 2026-11-13T08:59:32Z
- waiting_for: none
- result: Commit d645a43f. A1: URRExplosionSubsystem::Explode (every resolved explosion: barrel, kamikaze, chain links, stress) plays event Explosion via PlayEventAt with EdgeRadius = row Radius; fade distance = Dist2D(player, centre) - radius, 0 inside (URRAudioSubsystem::GetFadeDistance). A2: table line Explosion Volume 1.0, PitchJitter 0.05, MaxConcurrent 4 (executor choice), FadeInner 0, FadeOuter 1075 (Q6), Sound empty = silent until 516-02. -game run L_ArenaM1 Verbose LogRRAudio with the sound set temporarily: real barrel explosions at 0 uu from edge volume 1.000, 800 uu 0.256, 1300 uu Skip; rr.Audio.Play Explosion 900/1500/2000 0 700 -> 0.814/0.256/Skip, kamikaze radius 312 at 600 -> 0.732. rr.Audio.Play takes an optional 4th arg EdgeRadius. A4 code: build green; RogueRigs.Explosions 10/10 pass (no audio test family exists). One C4996 Cast<> warning from engine Casts.h in a unity file, no Cast added by this change.

## Relations
- [[task-516-audio-explosion-edge-distance]] — parent global task.
