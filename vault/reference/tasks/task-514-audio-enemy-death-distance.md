---
id: task-514-audio-enemy-death-distance
description: "Global task: Enemy death sound fades with distance from the player, with the fade option built in task-513."
parent: tasks-to-do-index
kind: task
status: in-progress
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
- next_step: task-514-01-death-fade
- started: 2026-11-13T08:48:56Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "ok pour Q5 et Q6"): every default below is accepted; values provisional, owner 2026-11-13.
- Q5 Mort des ennemis : mêmes distances que le mur (plein jusqu'à 200 uu, silence au bord de l'écran), niveau au plus proche inchangé (1.0, il n'a demandé que la distance). Default: oui.

## Subtasks
1. task-514-01-death-fade (vault/reference/tasks/task-514-audio-enemy-death-distance/task-514-01-death-fade.md) — cpp: enemy death line uses the distance fade; sys-audio line updated

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-514-01-death-fade]] — step 1, cpp.
