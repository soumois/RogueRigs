---
id: task-371-extractor-arrival
description: "Global task: At level start the miner stands beside the extractor, which takes off as soon as the fade in ends."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor: drops the miner and takes off at level start

- request: "Ok on va préparer les tâches d'intégration de l'extracteur dans le jeu : * Pour l'instant, il suffit de s'approcher de lui pour valider le niveau. * Lorsqu'on s'approche suffisamment l'écran de fin valide, le perso disparait et on voit l'extracteur décoller ( attention cela décolle comme un hélicoptère ) en fond et il sort de l'écran * Lorsqu'on arrive sur une map, lorsque le fade est fini, le joueur est déjà en dehors de l'extracteur et il décole directement. * Si des rochers bloquent la position de l'extracteur au sol, ils sont détruit. ( les ressources reste au sol s'il y en avait ) * Il faut faire tourner les hélices * Il faut une particule de souffle sous les 4 hélices" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached) Point 3 of 6: "Lorsqu'on arrive sur une map, lorsque le fade est fini, le joueur est déjà en dehors de l'extracteur et il décole directement." "préparer les tâches": nodes created, nothing launched before the owner confirms. No capture, test or bot run was requested; the capture subtasks of the 2026-10-16 owner rule were asked as Q8 and refused by the owner (2026-10-29): no capture subtask, he looks himself. Sequencing: every task here needs the rotor meshes of [[task-367-extractor-rotors]] (todo, runs after [[task-368-extractor-rebake]], in progress); the untracked SM_Extractor_Body|Rotor_*.fbx in Art/Meshes/extractor/ are partial task-367 outputs that its rerun replaces, so no subtask here uses them before task-367 is done. Questions Q1 to Q8 are asked once, all in [[task-369-extractor-level-exit]]; each global lists the ones that apply. Q4 applies here; Q8 (no capture) applies to all. Owner answer 2026-10-29 (relayed by the main session, verbatim): "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés." = the plan accepted.
- goal: Each level starts with an extractor next to the miner that takes off with the Q3 motion when the fade in ends, or at once when there is no fade (Q4).
- acceptance: Levels 2-4: extractor footprint 1 cell from the miner spawn cell, above him on screen, miner outside it; takeoff starts in the frame the fade in finishes; level 1 and a direct start: takeoff as soon as the level plays; the arrival extractor is not an exit (no board, no countdown), has no collision, robots ignore it, controls free; removed after leaving the view; doc updated; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved: lands when the alert bar is full, 37-50 cells from the miner, 45 s countdown, reaching it ends the level, missing it loses the run, landing clearance on a clear 4 x 3-cell footprint, look = the Rustwing extractor with four separate rotors), [[level-flow]] (step 1 Land, no arrival described), [[intermission]] (stats screen drawn over the paused finished level, cards screen opaque), [[run-structure]], [[destruction-rules]] (approved: only three things break blocks), [[ore-blocks]] (a deposit is never left partly destroyed; start-zone drops thrown to the ground), [[color-code]] (shuttle white + teal), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, nose +X, origin bottom centre, 50,000 triangles), tech [[sys-objectives-shuttle]], [[sys-objectives-run-flow]] (EndRun pauses the game), [[sys-run-transition-fade]] (fade in 0.5 s after the level switch; level 1 and direct PIE have no fade), [[bp-extraction-shuttle]] (white box, glow pulse, chevrons). level-flow step 1 describes no arrival: written by the Q4 edit. Fade events exist (OnScreenFadeFinished FadingIn); level 1 from the menu and a direct PIE have no fade. Blocks under the arrival footprint are task-372. Needs [[task-370-extractor-takeoff-exit]] first.
- confirmed_by_owner: yes (2026-10-29, "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés.")
- milestone: M2
- next_step: task-371-02-doc-extractor-arrival
- started: 2026-10-29T21:29:55Z
- waiting_for: none
- result: open

## Subtasks
1. task-371-01-cpp-extractor-arrival (vault/reference/tasks/task-371-extractor-arrival/task-371-01-cpp-extractor-arrival.md) — cpp: spawn an arrival extractor next to the miner spawn and start its takeoff at the end of the fade in, or at once without fade.
2. task-371-02-doc-extractor-arrival (vault/reference/tasks/task-371-extractor-arrival/task-371-02-doc-extractor-arrival.md) — doc: update sys-objectives-shuttle and sys-run-transition-fade for the arrival extractor.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-371-01-cpp-extractor-arrival]] — step 1, cpp.
- [[task-371-02-doc-extractor-arrival]] — step 2, doc.
- [[task-369-extractor-level-exit]] — holds the questions.
- [[task-370-extractor-takeoff-exit]] — must be done first: the takeoff reused here.
- [[level-flow]] — the arrival rule written by the Q4 edit.
