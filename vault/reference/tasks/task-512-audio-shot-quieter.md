---
id: task-512-audio-shot-quieter
description: "Global task: Lower the volume of the player shot sound, every weapon, as a table value the owner retouches by ear."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Tir du joueur plus discret

- request: « Alors, il faut rendre le tir plus discret et l'impact du mur doit s'écouter par rapport à la distance du player. ( et même au plus proche, il doit être plus discret ) » (owner 2026-11-13, after listening in game to the result of task-511-audio-system-base-sounds). Point 1 of 2: "rendre le tir plus discret". Point 2 (wall impact by distance) is task-513-audio-wall-impact-distance.
- goal: The player shot sound plays quieter for every player weapon, the new level set in the event table so the owner can retouch it by ear.
- acceptance: A1 every row of the event table that plays a player shot (Shot and any Shot.<weapon> row) has the volume decided in Q1, nothing else in the table changed; A2 rr.Audio.Play Shot in a -game run plays without warning in the LogRRAudio log (no listening possible: the owner judges by ear); A3 build green and existing audio tests pass; A4 the owner is told the value and where to retouch it.
- gdd_check: Read audio-direction and sys-audio. No conflict: mix rules of task-511 Q5 give no volume per event; the shot volume is a new provisional value (Q1), written in audio-direction by the edit of this plan. No open value needed (palette open does not apply). Milestone M3 as audio-direction. No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-512-01-shot-volume
- started: open
- waiting_for: owner answers to Q1 and confirmation of the plan
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q1 Volume du tir : passer de 1.0 à combien dans la table ? Default: 0.5 (environ -6 dB), même valeur pour toutes les armes du joueur.

## Subtasks
1. task-512-01-shot-volume (vault/reference/tasks/task-512-audio-shot-quieter/task-512-01-shot-volume.md) — cpp: shot volume lowered in the event table

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-512-01-shot-volume]] — step 1, cpp.
