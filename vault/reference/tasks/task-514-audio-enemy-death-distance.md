---
id: task-514-audio-enemy-death-distance
description: "Global task: Enemy death sound fades with distance from the player, with the fade option built in task-513."
parent: tasks-done-index
kind: task
status: done
---
# Mort des ennemis selon la distance au joueur

- request: « ok pour les questions. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok pour Q5 et Q6" = the plan and the defaults of Q5 accepted.

il faut aussi ce même systéme pour la mort des ennemis.

Applique le son d'xp au minerai ramassé pour l'instant.

Il faudrait aussi le prompt pour les explosions et qui changerait aussi selon la distance, sachant que la distance se calculerait par rapport au bord de l'explosion.

Il faudrait aussi le son quand on creuse. » (owner 2026-11-13, answer to task-512/513 questions). Just before: « Je ne publierais pas le jeu avec ces sons, c'est juste pour avoir des sons placeholder qui seront remplacé plus tard. Ou est-ce que je peux avoir le son coupé, car je n'ai pas entendu le level up. » (level up: open diagnosis by the owner, not planned). Point: "ce même systéme pour la mort des ennemis" (read by the relay as the distance fade of task-513). Owner 2026-11-13 on that reading: « Lecture de « ce même système » : Oui c'est ça. »
- goal: The enemy death sound gets quieter the farther the death is from the player, silent beyond the outer distance, using the table fade option of task-513.
- acceptance: A1 the EnemyDeath table line uses the distance fade with the values of Q5; A2 in a -game run with Verbose LogRRAudio a near and a far death log different levels, one beyond the outer distance plays nothing; A3 build green, audio tests pass; A4 sys-audio lists the death line among faded events and the owner is told the values to retouch.
- gdd_check: Read audio-direction, sys-audio. Needs the per-line fade of task-513 (done first). Conflict: none (death is a placed world sound). Open: values (Q5). No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok pour Q5 et Q6")
- milestone: M2
- next_step: none
- started: 2026-11-13T08:48:56Z
- waiting_for: none
- executor_tokens: input 50, cache write 40324, cache read 893095, output 7868
- executor_usage: runs 1, turns 25, context start 17k peak 53k, resumes 0, 5 min, units 0.18 M, USD 0.54
- planning_tokens: input 8, cache write 10424, cache read 160328, output 4327
- planning_usage: aec6de57 "Plan shot and wall-impact sound changes" 1/7 (tasks 512, 513, 514, 515, 516, 517, 518), units 0.05 M, USD 0.17
- result: Done. Commit 0a7f44e9. A1: DefaultGame.ini EnemyDeath row adds FadeInner=200, FadeOuter=1075, Volume 1.0 unchanged (Q5 defaults). A2: -game run on L_ArenaM1 with Verbose LogRRAudio: rr.Audio.Play EnemyDeath at 100/650/1050/2000 uu -> volume 1.000/0.486/0.029/Skip; a real robot death at 1560 uu logged Skip. A3: config line only, no code change: build green (no test run per build-toolchain Test scope; no audio test family exists). A4: sys-audio distance_fade lists Impact.Wall and EnemyDeath as faded rows. Owner retouch points: EnemyDeath line in Config/DefaultGame.ini, Volume (1.0), FadeInner (200), FadeOuter (1075).

## Questions
Decided 2026-11-13 (owner "ok pour Q5 et Q6"): every default below is accepted; values provisional, owner 2026-11-13.
- Q5 Mort des ennemis : mêmes distances que le mur (plein jusqu'à 200 uu, silence au bord de l'écran), niveau au plus proche inchangé (1.0, il n'a demandé que la distance). Default: oui.

## Subtasks
1. task-514-01-death-fade (vault/reference/tasks/task-514-audio-enemy-death-distance/task-514-01-death-fade.md) — cpp: enemy death line uses the distance fade; sys-audio line updated

## Relations
- [[tasks-done-index]] — parent index.
- [[task-514-01-death-fade]] — step 1, cpp.
- [[sys-audio]] — documents the faded EnemyDeath row.
