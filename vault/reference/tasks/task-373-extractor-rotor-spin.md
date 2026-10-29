---
id: task-373-extractor-rotor-spin
description: "Global task: The four extractor rotors spin on their axes, opposite diagonal pairs, slower on the ground, faster in flight."
parent: tasks-done-index
kind: task
status: done
---
# Extractor: the four rotors spin

- request: "Ok on va préparer les tâches d'intégration de l'extracteur dans le jeu : * Pour l'instant, il suffit de s'approcher de lui pour valider le niveau. * Lorsqu'on s'approche suffisamment l'écran de fin valide, le perso disparait et on voit l'extracteur décoller ( attention cela décolle comme un hélicoptère ) en fond et il sort de l'écran * Lorsqu'on arrive sur une map, lorsque le fade est fini, le joueur est déjà en dehors de l'extracteur et il décole directement. * Si des rochers bloquent la position de l'extracteur au sol, ils sont détruit. ( les ressources reste au sol s'il y en avait ) * Il faut faire tourner les hélices * Il faut une particule de souffle sous les 4 hélices" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached) Point 5 of 6: "Il faut faire tourner les hélices". "préparer les tâches": nodes created, nothing launched before the owner confirms. No capture, test or bot run was requested; the capture subtasks of the 2026-10-16 owner rule were asked as Q8 and refused by the owner (2026-10-29): no capture subtask, he looks himself. Sequencing: every task here needs the rotor meshes of [[task-367-extractor-rotors]] (todo, runs after [[task-368-extractor-rebake]], in progress); the untracked SM_Extractor_Body|Rotor_*.fbx in Art/Meshes/extractor/ are partial task-367 outputs that its rerun replaces, so no subtask here uses them before task-367 is done. Questions Q1 to Q8 are asked once, all in [[task-369-extractor-level-exit]]; each global lists the ones that apply. Q6 applies here; Q8 (no capture) applies to all. Owner answer 2026-10-29 (relayed by the main session, verbatim): "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés." = the plan accepted.
- goal: The rotors of every extractor spin per Q6 on the ground and during takeoff, also while the game is paused for the end screen, and expose their speed to Blueprints for the wash.
- acceptance: Each rotor turns on its own axis; FL and RR clockwise seen from above, FR and RL counter-clockwise; 3 rev/s on the ground, 5 rev/s in flight, 0.5 s ramp; spinning during the paused end-screen takeoff; pure getter of the speed fraction and a Blueprint event when it changes; doc updated; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved: lands when the alert bar is full, 37-50 cells from the miner, 45 s countdown, reaching it ends the level, missing it loses the run, landing clearance on a clear 4 x 3-cell footprint, look = the Rustwing extractor with four separate rotors), [[level-flow]] (step 1 Land, no arrival described), [[intermission]] (stats screen drawn over the paused finished level, cards screen opaque), [[run-structure]], [[destruction-rules]] (approved: only three things break blocks), [[ore-blocks]] (a deposit is never left partly destroyed; start-zone drops thrown to the ground), [[color-code]] (shuttle white + teal), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, nose +X, origin bottom centre, 50,000 triangles), tech [[sys-objectives-shuttle]], [[sys-objectives-run-flow]] (EndRun pauses the game), [[sys-run-transition-fade]] (fade in 0.5 s after the level switch; level 1 and direct PIE have no fade), [[bp-extraction-shuttle]] (white box, glow pulse, chevrons). No gdd value for rotor speed or direction: asked as Q6 (written in extraction by the edit). Needs [[task-370-extractor-takeoff-exit]] first (flight state).
- confirmed_by_owner: yes (2026-10-29, "Pas de vérification visuel pour le point 8, prépare les tâches avec les informations dedans, je les lancerais déjà que le mesh est prés.")
- milestone: M2
- next_step: none
- started: 2026-10-29T21:59:48Z
- waiting_for: none
- result: Done. Docs updated (commit d12b02a6): sys-objectives-shuttle and bp-extraction-shuttle; GetRotorSpeedAlpha defined as rev/s over FlightRevPerSecond (0.6 ground, 1 flight).

## Subtasks
1. task-373-01-cpp-extractor-rotor-spin (vault/reference/tasks/task-373-extractor-rotor-spin/task-373-01-cpp-extractor-rotor-spin.md) — cpp: spin the four rotor components per Q6, ground and flight speeds, ramp, ticking while paused; speed fraction getter and event.
2. task-373-02-doc-extractor-rotor-spin (vault/reference/tasks/task-373-extractor-rotor-spin/task-373-02-doc-extractor-rotor-spin.md) — doc: update sys-objectives-shuttle and bp-extraction-shuttle for the rotor spin.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-373-01-cpp-extractor-rotor-spin]] — step 1, cpp.
- [[task-373-02-doc-extractor-rotor-spin]] — step 2, doc.
- [[task-369-extractor-level-exit]] — holds the questions; makes the rotor components.
- [[task-370-extractor-takeoff-exit]] — must be done first: the flight state the speed follows.
