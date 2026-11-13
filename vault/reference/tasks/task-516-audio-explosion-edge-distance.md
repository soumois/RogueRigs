---
id: task-516-audio-explosion-edge-distance
description: "Global task: Placed explosion sound event, level fading with the distance from the explosion edge; import of the owner file."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Son des explosions selon la distance au bord de l'explosion

- request: « ok pour les questions. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok pour Q5 et Q6" = the plan and the defaults of Q6 accepted.

il faut aussi ce même systéme pour la mort des ennemis.

Applique le son d'xp au minerai ramassé pour l'instant.

Il faudrait aussi le prompt pour les explosions et qui changerait aussi selon la distance, sachant que la distance se calculerait par rapport au bord de l'explosion.

Il faudrait aussi le son quand on creuse. » (owner 2026-11-13, answer to task-512/513 questions). Just before: « Je ne publierais pas le jeu avec ces sons, c'est juste pour avoir des sons placeholder qui seront remplacé plus tard. Ou est-ce que je peux avoir le son coupé, car je n'ai pas entendu le level up. » (level up: open diagnosis by the owner, not planned). Point: "le prompt pour les explosions et qui changerait aussi selon la distance, sachant que la distance se calculerait par rapport au bord de l'explosion." The relay gives the prompt in the chat; this task only wires and imports.
- goal: Every explosion plays one placed explosion sound whose level fades with the distance between the player and the explosion edge; the owner's generated file is imported and set on that line.
- acceptance: A1 every explosion (Q6) calls one explosion event; distance = player to explosion centre minus its radius, 0 inside; A2 fade per Q6, values in the table line; in a -game run with Verbose LogRRAudio a near and a far explosion log different levels, beyond the outer distance nothing; A3 the owner file is imported as an SW_ asset, listed in sound_manifest.json, set on the line; the manifest marks all kept sounds "placeholder, à remplacer avant publication"; A4 build green, audio tests pass; sys-audio documents the event, the edge distance and the placeholder status.
- gdd_check: Read audio-direction, sys-audio. task-511 Q1 left explosions out (slice 2): the owner adds them now. Needs the fade of task-513. Open: which explosions and the fade values (Q6). The sound file comes from the owner. No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok pour Q5 et Q6")
- milestone: M2
- next_step: task-516-03-explosion-doc
- started: 2026-11-13T09:04:05Z
- waiting_for: none
- executor_tokens: input 84, cache write 95726, cache read 3069406, output 20555
- executor_usage: runs 1, turns 42, context start 17k peak 108k, resumes 0, 6 min, units 0.53 M, USD 1.50
- planning_tokens: input 10, cache write 20096, cache read 213155, output 4963
- planning_usage: aec6de57 "Plan shot and wall-impact sound changes" 1/8 (tasks 512, 513, 514, 515, 516, 517, 518, 519), units 0.07 M, USD 0.24
- result: open

## Questions
Decided 2026-11-13 (owner "ok pour Q5 et Q6"): every default below is accepted; values provisional, owner 2026-11-13.
- Q6 Explosions : toutes celles du jeu (kamikaze, baril, autres) sur un seul événement ; niveau plein tant que le joueur est dans le rayon ou au bord, puis baisse linéaire jusqu'au silence à la distance du bord de l'écran au-delà du bord ; tant que ton fichier n'est pas là, la ligne est muette. Default: oui.

## Subtasks
1. task-516-01-explosion-event (vault/reference/tasks/task-516-audio-explosion-edge-distance/task-516-01-explosion-event.md) — cpp: explosion event at every explosion, fade measured from the edge, table line
2. task-516-02-explosion-import (vault/reference/tasks/task-516-audio-explosion-edge-distance/task-516-02-explosion-import.md) — bp: owner explosion file imported, manifest updated (placeholder marks)
3. task-516-03-explosion-doc (vault/reference/tasks/task-516-audio-explosion-edge-distance/task-516-03-explosion-doc.md) — doc: sys-audio: explosion event, edge distance, placeholder status

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-516-01-explosion-event]] — step 1, cpp.
- [[task-516-02-explosion-import]] — step 2, bp.
- [[task-516-03-explosion-doc]] — step 3, doc.
