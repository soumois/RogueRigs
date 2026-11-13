---
id: task-512-audio-shot-quieter
description: "Global task: Lower the volume of the player shot sound, every weapon, as a table value the owner retouches by ear."
parent: tasks-done-index
kind: task
status: done
---
# Tir du joueur plus discret

- request: « Alors, il faut rendre le tir plus discret et l'impact du mur doit s'écouter par rapport à la distance du player. ( et même au plus proche, il doit être plus discret ) » (owner 2026-11-13, after listening in game to the result of task-511-audio-system-base-sounds). Point 1 of 2: "rendre le tir plus discret". Point 2 (wall impact by distance) is task-513-audio-wall-impact-distance. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok pour les questions." = the plan and the defaults of Q1 accepted.
- goal: The player shot sound plays quieter for every player weapon, the new level set in the event table so the owner can retouch it by ear.
- acceptance: A1 every row of the event table that plays a player shot (Shot and any Shot.<weapon> row) has the volume decided in Q1, nothing else in the table changed; A2 rr.Audio.Play Shot in a -game run plays without warning in the LogRRAudio log (no listening possible: the owner judges by ear); A3 build green and existing audio tests pass; A4 the owner is told the value and where to retouch it.
- gdd_check: Read audio-direction and sys-audio. No conflict: mix rules of task-511 Q5 give no volume per event; the shot volume is a new provisional value (Q1), written in audio-direction by the edit of this plan. No open value needed (palette open does not apply). Milestone M3 as audio-direction. No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok pour les questions.")
- milestone: M2
- next_step: none
- started: 2026-11-13T08:33:42Z
- waiting_for: none
- executor_tokens: input 24, cache write 41352, cache read 342685, output 4149
- executor_usage: runs 1, turns 12, context start 16k peak 41k, resumes 0, 2 min, units 0.11 M, USD 0.36
- planning_tokens: input 6, cache write 9122, cache read 112404, output 3540
- planning_usage: aec6de57 "Plan shot and wall-impact sound changes" 1/7 (tasks 512, 513, 514, 515, 516, 517, 518), units 0.04 M, USD 0.14
- result: Done. Config/DefaultGame.ini [/Script/RogueRigs.RRAudioSettings]: the one player shot row Shot (every weapon falls back to it as Shot.<weapon>; no Shot.<weapon> row exists) Volume 1.0 -> 0.5, nothing else changed (commit 020a2331). Build RogueRigsEditor exit 0. -game run on L_ArenaM1 (-RenderOffScreen): rr.Audio.Play Shot and Shot.BurstRifle plays logged on row Shot, 0 LogRRAudio warnings or errors. No RogueRigs audio automation test exists, so none to run. sys-audio quotes no volume, unchanged; audio-direction already holds 0.5. Owner retouch: the Volume= of the Shot line in Config/DefaultGame.ini, by ear.

## Questions
Decided 2026-11-13 (owner "ok pour les questions."): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Volume du tir : passer de 1.0 à combien dans la table ? Default: 0.5 (environ -6 dB), même valeur pour toutes les armes du joueur.

## Subtasks
1. task-512-01-shot-volume (vault/reference/tasks/task-512-audio-shot-quieter/task-512-01-shot-volume.md) — cpp: shot volume lowered in the event table

## Relations
- [[tasks-done-index]] — parent index.
- [[task-512-01-shot-volume]] — step 1, cpp.
