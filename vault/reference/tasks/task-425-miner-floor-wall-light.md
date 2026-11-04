---
id: task-425-miner-floor-wall-light
description: "Global task: Step 1 of the darkener rework: a point light on the miner that lights only floor and terrain blocks, through lighting channels."
parent: tasks-done-index
kind: task
status: done
---
# Miner light that lights only the floor and the walls

- request: Owner 2026-11-04 (verbatim): "Le but de cette conversation est de retravailler le shader d'assombrissement, on va travailler par étape. Plutôt qu'avoir par shader le fait d'illuminer le centre, j'aimerais une light sur le personnage qui ne touche que le sol et les murs. Ensuite, je voudrais que le shader actuel soit modifié pour retirer l'effet lumineux et me propose par cercle autour du centre de l'écran avec une valeur max et min. ( min = valeur du noir rajouté au centre de l'écran, max = valeur de noir rajouté au coin de l'écran et un fade entre ces deux valeurs ) Le radius = distance entre le centre et un coin de l'écran. N'hésite pas à me poser des questions, je ne suis pas technical artiste, donc je n'ai peut être pas exprimé mon idée avec le bon vocabulaire technique" This task is step 1 ("une light sur le personnage qui ne touche que le sol et les murs"); step 2 is task-426-screen-radial-darkener. Owner answer 2026-11-04 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5 accepted.
- goal: BP_Miner carries a point light on lighting channel 1 only; the floor and terrain block render components of the grid are on channels 0 and 1; everything else stays on channel 0 only, so the light touches only the floor and walls. The existing mask shader is not changed in this task.
- acceptance: A1 floor and rock/ore tops around the miner are visibly brighter with the light than without it. A2 robots, the miner, the drone, pickups, barrels and the chest inside the pool look the same with and without the light. A3 lit pool about 400 uu radius around the miner. A4 floor under the miner below about 205/255, no white burn. A5 the light follows the miner. Values per Questions (defaults until answered).
- gdd_check: visual-style (approved spec, provisional lines): no conflict; handover-2026-10-21-lighting-floor already records the owner wish "later a light on the player that only touches floor and walls". Former PlayerLight revert recipe in bp-miner (task-272-02) and tuning history in lights-dark-ambient; this one differs (lighting channel 1 only). Light colour, shadows, pool size and which blocks count as walls were not decided: Q1 to Q4. No open gdd value blocks. Grid render components are built in C++ (ARRArenaGrid chunk HISMs from templates, rock piece ISMs), so the channels are set in C++. Milestone M2.
- confirmed_by_owner: yes (2026-11-04, "Tout par défaut")
- milestone: M2
- next_step: task-425-04-light-doc
- started: 2026-11-04T16:52:35Z
- waiting_for: none
- result: Steps 01 cpp, 02 bp, 04 doc done; capture 03 waived on the owner's word (visual check after task-426). Light intensity 4 is a first guess, not render-checked.

## Questions
Decided 2026-11-04 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-04.
- Q1 Shadows: should the walls cast a shadow on the floor from this light? Default: no shadow (cheaper; the moving shadowed light was a known cost in perf-risks).
- Q2 Colour: Default: warm cream 255/194/158 (the last miner light you had). Other choice: pure white.
- Q3 Size and strength: Default: lit pool about 400 uu radius around the miner (about 1/5 of the screen width, as in the DRGS reference), floor under the miner never burnt white (below about 205/255); the bp agent tunes intensity on captures.
- Q4 What counts as "les murs": Default: the floor plus all terrain blocks: rock, ore and their crystals. Not lit: barrels, chest, robots, miner, drone, pickups, floating asteroids, space background.
- Q5 Step by step: Default: step 1 (this task) runs, you see its capture, and step 2 (task-426-screen-radial-darkener) starts only after your go. Until step 2 the old shader still brightens the centre, so the centre will look brighter than final.

## Subtasks
1. task-425-01-grid-lighting-channels (vault/reference/tasks/task-425-miner-floor-wall-light/task-425-01-grid-lighting-channels.md) — cpp: floor and terrain block render components on lighting channels 0 and 1
2. task-425-02-miner-point-light (vault/reference/tasks/task-425-miner-floor-wall-light/task-425-02-miner-point-light.md) — bp: point light on BP_Miner, lighting channel 1 only
3. task-425-03-light-capture (vault/reference/tasks/task-425-miner-floor-wall-light/task-425-03-light-capture.md) — capture: check that the light touches only floor and walls
4. task-425-04-light-doc (vault/reference/tasks/task-425-miner-floor-wall-light/task-425-04-light-doc.md) — doc: document the light and the grid lighting channels

## Relations
- [[tasks-done-index]] — parent index.
- [[task-425-01-grid-lighting-channels]] — step 1, cpp.
- [[task-425-02-miner-point-light]] — step 2, bp.
- [[task-425-03-light-capture]] — step 3, capture.
- [[task-425-04-light-doc]] — step 4, doc.
- [[visual-style]] — gdd look node updated with the decided light
- [[mpp-player-light-mask]] — current mask, unchanged here
- [[bp-miner]] — carries the light; former PlayerLight recipe
- [[task-426-screen-radial-darkener]] — step 2, after this task
