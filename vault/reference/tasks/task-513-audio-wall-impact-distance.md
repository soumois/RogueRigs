---
id: task-513-audio-wall-impact-distance
description: "Global task: Separate wall impact sound, quieter and fading with distance from the player; values in the event table."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Impact sur un mur : plus discret et selon la distance au joueur

- request: « Alors, il faut rendre le tir plus discret et l'impact du mur doit s'écouter par rapport à la distance du player. ( et même au plus proche, il doit être plus discret ) » (owner 2026-11-13, after listening in game to the result of task-511-audio-system-base-sounds). Point 2 of 2: "l'impact du mur doit s'écouter par rapport à la distance du player ( et même au plus proche, il doit être plus discret )". Point 1 is task-512-audio-shot-quieter.
- goal: A player bullet that hits a wall plays its impact sound quieter than today even next to the player, and quieter the farther the impact is from the player, silent beyond the distance decided; an impact on an enemy sounds as today. Every value is in the event table.
- acceptance: A1 a player bullet hitting a wall (as defined in Q2) plays a wall impact event distinct from the enemy impact, with its own table line; the enemy impact line and its sound are unchanged; A2 the wall impact level depends on the distance player to impact point: full level up to the inner distance, linear fade, silent beyond the outer distance, the level at the player and both distances as in Q3 and Q4, all three read from the table line; A3 check without listening: in a -game run with Verbose LogRRAudio, a wall impact near the player and one far from it log different levels and one beyond the outer distance plays nothing; existing audio tests pass, build green; A4 sys-audio documents the wall event and the distance fade fields, and the owner is told which table values to retouch.
- gdd_check: Read audio-direction and sys-audio. sys-audio says bSpatial is placed on screen with no distance fade, so the fade is new behaviour in the audio system (comportement). Conflict with task-511 Q1 (one impact event for enemy or block): the owner now separates the wall impact; the event list line stays, the split is recorded by the edit of this plan. Open: the definition of wall (Q2), levels and distances (Q3, Q4); not read: the camera, so the screen edge distance in uu is measured by the Executor and written in the table. Palette open does not apply (same sound). Milestone M3. No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-513-01-wall-impact-fade
- started: open
- waiting_for: owner answers to Q2 to Q4 and confirmation of the plan
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q2 « Mur » = tout ce qui n'est pas un ennemi : blocs de toutes sortes (destructibles ou non) et bord de la carte ; l'impact sur un ennemi reste exactement comme aujourd'hui. Default: oui.
- Q3 Niveau au plus proche du joueur : Default: 0.5 (moitié du niveau actuel), modifiable dans la table.
- Q4 Distance : Default: niveau plein jusqu'à 200 uu (environ 3 blocs) du joueur, puis baisse linéaire jusqu'au silence à la distance du bord de l'écran (mesurée d'après la caméra par l'Executor et écrite dans la table) ; même règle en mouvement, la distance est prise au moment de l'impact.

## Subtasks
1. task-513-01-wall-impact-fade (vault/reference/tasks/task-513-audio-wall-impact-distance/task-513-01-wall-impact-fade.md) — cpp: separate wall impact event with a distance fade from the player, values in the table
2. task-513-02-audio-doc (vault/reference/tasks/task-513-audio-wall-impact-distance/task-513-02-audio-doc.md) — doc: sys-audio updated: wall impact event, distance fade fields

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-513-01-wall-impact-fade]] — step 1, cpp.
- [[task-513-02-audio-doc]] — step 2, doc.
