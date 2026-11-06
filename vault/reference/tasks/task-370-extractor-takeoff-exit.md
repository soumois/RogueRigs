---
id: task-370-extractor-takeoff-exit
description: "Global task: At the contact the end screen opens, the miner disappears and the extractor takes off like a helicopter behind it and leaves the screen."
parent: tasks-done-index
kind: task
status: done
---
# Extractor: helicopter takeoff behind the end screen

- request: "Ok on va préparer les tâches d'intégration de l'extracteur dans le jeu : * Pour l'instant, il suffit de s'approcher de lui pour valider le niveau. * Lorsqu'on s'approche suffisamment l'écran de fin valide, le perso disparait et on voit l'extracteur décoller ( attention cela décolle comme un hélicoptère ) en fond et il sort de l'écran * Lorsqu'on arrive sur une map, lorsque le fade est fini, le joueur est déjà en dehors de l'extracteur et il décole directement. * Si des rochers bloquent la position de l'extracteur au sol, ils sont détruit. ( les ressources reste au sol s'il y en avait ) * Il faut faire tourner les hélices * Il faut une particule de souffle sous les 4 hélices" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached) Point 2 of 6: "Lorsqu'on s'approche suffisamment l'écran de fin valide, le perso disparait et on voit l'extracteur décoller ( attention cela décolle comme un hélicoptère ) en fond et il sort de l'écran". "préparer les tâches": nodes created, nothing launched before the owner confirms. No capture, test or bot run was requested; the capture subtasks of the 2026-10-16 owner rule were asked as Q8 and refused by the owner (2026-10-29): no capture subtask, he looks himself. Sequencing: every task here needs the rotor meshes of [[task-367-extractor-rotors]] (todo, runs after [[task-368-extractor-rebake]], in progress); the untracked SM_Extractor_Body|Rotor_*.fbx in Art/Meshes/extractor/ are partial task-367 outputs that its rerun replaces, so no subtask here uses them before task-367 is done. Questions Q1 to Q8 are asked once, all in [[task-369-extractor-level-exit]]; each global lists the ones that apply. Q3 applies here; Q8 (no capture) applies to all. Owner answer 2026-10-29 (relayed by the main session, verbatim): "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés." = the plan accepted.
- goal: On a successful extraction the success screen opens at once, the miner and drone disappear, and the extractor plays a helicopter takeoff behind it while the game stays paused, until it leaves the camera view (Q3).
- acceptance: At the contact: success or stats screen in the same frame as today; miner and drone hidden; robots and pickups frozen; the extractor rises vertically 400 uu in 1.5 s after a 0.5 s spin-up, tilts nose down 20 deg and accelerates along its nose until out of the camera view, then is removed; takeoff callable from C++ for the arrival (task-371); Blueprint events at takeoff start and end; tunables provisional on ARRShuttle; existing Extraction and RunFlow tests pass; doc updated; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved: lands when the alert bar is full, 37-50 cells from the miner, 45 s countdown, reaching it ends the level, missing it loses the run, landing clearance on a clear 4 x 3-cell footprint, look = the Rustwing extractor with four separate rotors), [[level-flow]] (step 1 Land, no arrival described), [[intermission]] (stats screen drawn over the paused finished level, cards screen opaque), [[run-structure]], [[destruction-rules]] (approved: only three things break blocks), [[ore-blocks]] (a deposit is never left partly destroyed; start-zone drops thrown to the ground), [[color-code]] (shuttle white + teal), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, nose +X, origin bottom centre, 50,000 triangles), tech [[sys-objectives-shuttle]], [[sys-objectives-run-flow]] (EndRun pauses the game), [[sys-run-transition-fade]] (fade in 0.5 s after the level switch; level 1 and direct PIE have no fade), [[bp-extraction-shuttle]] (white box, glow pulse, chevrons). No gdd node describes the miner leaving or a takeoff: values asked as Q3 (written in extraction by the Q edits). EndRun pauses the world, so the takeoff must tick while paused; the stats screen is drawn over the paused level, so it is visible behind. Needs [[task-369-extractor-level-exit]] first.
- confirmed_by_owner: yes (2026-10-29, "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés.")
- milestone: M2
- next_step: none
- started: 2026-10-29T21:17:31Z
- waiting_for: none
- executor_tokens: input 24, cache write 61558, cache read 271782, output 4547
- executor_usage: runs 1, turns 10, context start 18k peak 40k, resumes 0, 12 min, units 0.13 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Commit b2327290: sys-objectives-shuttle, events and run-flow document StartTakeoff, tunables, events, hide rule; ForwardAcceleration, TakeoffMaxSeconds, TiltSeconds marked provisional.

## Subtasks
1. task-370-01-cpp-extractor-takeoff (vault/reference/tasks/task-370-extractor-takeoff-exit/task-370-01-cpp-extractor-takeoff.md) — cpp: hide miner and drone on extraction, helicopter takeoff of ARRShuttle ticking while paused, StartTakeoff entry and events.
2. task-370-02-doc-extractor-takeoff (vault/reference/tasks/task-370-extractor-takeoff-exit/task-370-02-doc-extractor-takeoff.md) — doc: update sys-objectives-shuttle and sys-objectives-events for the takeoff and hidden miner.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-370-01-cpp-extractor-takeoff]] — step 1, cpp.
- [[task-370-02-doc-extractor-takeoff]] — step 2, doc.
- [[task-369-extractor-level-exit]] — must be done first; holds the questions.
- [[extraction]] — the takeoff rule written by the Q3 edit.
