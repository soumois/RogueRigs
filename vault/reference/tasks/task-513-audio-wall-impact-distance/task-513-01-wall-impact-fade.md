---
id: task-513-01-wall-impact-fade
description: "Cpp: separate wall impact event with a distance fade from the player, values in the table"
parent: task-513-audio-wall-impact-distance
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-513
- usage: included in the executor run of task-513
- runs: 1
- milestone: M2
- started: 2026-11-13T08:36:41Z
- waiting_for: none
- result: Commit 14c24a29. A1: player shot stopped by anything not a robot (blocks, map edge) plays Impact.Wall, own table line; Impact line unchanged (enemy hits log volume 1.000). A2: per-row FadeInner/FadeOuter, used only by Impact.Wall: Volume 0.5, 200 uu, 1075 uu (left/right screen edge at 1920x1080 measured in game by rr.Audio.ScreenEdge; top 1237, bottom 579, top corners 2090). A3: -game run with Verbose LogRRAudio: real wall hits at 722 uu volume 0.210, 946 uu 0.086, beyond logged Skip; rr.Audio.Play Impact.Wall at 100/650/1050/2000 uu -> 0.500/0.250/0.028/skip. Build green; RogueRigs.Weapons ImpactSparks, ProjectileStopsOnRock, PierceStopsOnRock pass; ImpactSparkAsset fails identically without the change (pre-existing, rifle projectile spark asset). No audio test family exists. ALERT light budget (42 turns, 0.54 M at closing): cause the build doc path points to another project folder, one rebuild to prove the ImpactSparkAsset failure pre-existing, three -game runs (PowerShell ExecCmds quoting, game starts on the main menu: rr.Menu.NewRun needed).

## Relations
- [[task-513-audio-wall-impact-distance]] — parent global task.
