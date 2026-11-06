---
id: task-372-extractor-clears-rocks
description: "Global task: When the extractor is set on the ground, blocks under its footprint are destroyed and their ore or heal pickups stay on the floor."
parent: tasks-done-index
kind: task
status: done
---
# Extractor: blocks under its footprint are destroyed, resources stay

- request: "Ok on va préparer les tâches d'intégration de l'extracteur dans le jeu : * Pour l'instant, il suffit de s'approcher de lui pour valider le niveau. * Lorsqu'on s'approche suffisamment l'écran de fin valide, le perso disparait et on voit l'extracteur décoller ( attention cela décolle comme un hélicoptère ) en fond et il sort de l'écran * Lorsqu'on arrive sur une map, lorsque le fade est fini, le joueur est déjà en dehors de l'extracteur et il décole directement. * Si des rochers bloquent la position de l'extracteur au sol, ils sont détruit. ( les ressources reste au sol s'il y en avait ) * Il faut faire tourner les hélices * Il faut une particule de souffle sous les 4 hélices" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached) Point 4 of 6: "Si des rochers bloquent la position de l'extracteur au sol, ils sont détruit. ( les ressources reste au sol s'il y en avait )". "préparer les tâches": nodes created, nothing launched before the owner confirms. No capture, test or bot run was requested; the capture subtasks of the 2026-10-16 owner rule were asked as Q8 and refused by the owner (2026-10-29): no capture subtask, he looks himself. Sequencing: every task here needs the rotor meshes of [[task-367-extractor-rotors]] (todo, runs after [[task-368-extractor-rebake]], in progress); the untracked SM_Extractor_Body|Rotor_*.fbx in Art/Meshes/extractor/ are partial task-367 outputs that its rerun replaces, so no subtask here uses them before task-367 is done. Questions Q1 to Q8 are asked once, all in [[task-369-extractor-level-exit]]; each global lists the ones that apply. Q5 applies here; Q8 (no capture) applies to all. Owner answer 2026-10-29 (relayed by the main session, verbatim): "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés." = the plan accepted.
- goal: Every ground placement of the extractor removes the blocks of its footprint before it is seen; resources of broken deposits or heal veins lie on the floor (Q5).
- acceptance: At arrival and at extraction landing, no block remains in the 4 x 3-cell footprint; a deposit or heal vein touching it breaks whole and its chunks or heal pickups rest on the floor of their cells, pickable; barrels removed without explosion; new terrain damage source Extractor logged in the run log; a log line per placement with the counts by type (0 accepted when the footprint was open, reported); doc updated; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved: lands when the alert bar is full, 37-50 cells from the miner, 45 s countdown, reaching it ends the level, missing it loses the run, landing clearance on a clear 4 x 3-cell footprint, look = the Rustwing extractor with four separate rotors), [[level-flow]] (step 1 Land, no arrival described), [[intermission]] (stats screen drawn over the paused finished level, cards screen opaque), [[run-structure]], [[destruction-rules]] (approved: only three things break blocks), [[ore-blocks]] (a deposit is never left partly destroyed; start-zone drops thrown to the ground), [[color-code]] (shuttle white + teal), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, nose +X, origin bottom centre, 50,000 triangles), tech [[sys-objectives-shuttle]], [[sys-objectives-run-flow]] (EndRun pauses the game), [[sys-run-transition-fade]] (fade in 0.5 s after the level switch; level 1 and direct PIE have no fade), [[bp-extraction-shuttle]] (white box, glow pulse, chevrons). Conflict: destruction-rules (approved) allows only three block breakers; the extractor is a fourth, written by the Q5 edit. Extraction landing clearance kept (approved), so in practice this acts at arrival. ore-blocks already allows start-zone drops on the ground. Needs [[task-371-extractor-arrival]] first.
- confirmed_by_owner: yes (2026-10-29, "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés.")
- milestone: M2
- next_step: none
- started: 2026-10-29T21:42:09Z
- waiting_for: none
- executor_tokens: input 42, cache write 67557, cache read 558730, output 7918
- executor_usage: runs 1, turns 17, context start 18k peak 46k, resumes 3, 18 min, units 0.18 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Done: 10 doc nodes updated citing commits de889932 and 0e85c9a0 (doc commit 7598c940); lint clean.

## Subtasks
1. task-372-01-cpp-extractor-footprint-clear (vault/reference/tasks/task-372-extractor-clears-rocks/task-372-01-cpp-extractor-footprint-clear.md) — cpp: grid footprint clear with source Extractor at each extractor ground placement, drops at rest, barrels without explosion.
2. task-372-02-doc-extractor-footprint-clear (vault/reference/tasks/task-372-extractor-clears-rocks/task-372-02-doc-extractor-footprint-clear.md) — doc: update sys-grid-api, sys-grid, sys-grid-deposits, run-log-schema and sys-objectives-shuttle.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-372-01-cpp-extractor-footprint-clear]] — step 1, cpp.
- [[task-372-02-doc-extractor-footprint-clear]] — step 2, doc.
- [[task-369-extractor-level-exit]] — holds the questions.
- [[task-371-extractor-arrival]] — must be done first: the arrival placement cleared here.
- [[destruction-rules]] — the fourth breaker written by the Q5 edit.
