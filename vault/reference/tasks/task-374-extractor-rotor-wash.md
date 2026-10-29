---
id: task-374-extractor-rotor-wash
description: "Global task: A rotor-wash particle effect plays under each of the four extractor rotors, following the rotor speed."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Extractor: rotor-wash effect under the four rotors

- request: "Ok on va préparer les tâches d'intégration de l'extracteur dans le jeu : * Pour l'instant, il suffit de s'approcher de lui pour valider le niveau. * Lorsqu'on s'approche suffisamment l'écran de fin valide, le perso disparait et on voit l'extracteur décoller ( attention cela décolle comme un hélicoptère ) en fond et il sort de l'écran * Lorsqu'on arrive sur une map, lorsque le fade est fini, le joueur est déjà en dehors de l'extracteur et il décole directement. * Si des rochers bloquent la position de l'extracteur au sol, ils sont détruit. ( les ressources reste au sol s'il y en avait ) * Il faut faire tourner les hélices * Il faut une particule de souffle sous les 4 hélices" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached) Point 6 of 6: "Il faut une particule de souffle sous les 4 hélices". "préparer les tâches": nodes created, nothing launched before the owner confirms. No capture, test or bot run was requested; the capture subtasks come from the owner rule of 2026-10-16 (orchestrator-lessons: a task that changes what the player sees ends with a -game capture) and are asked as Q8. Sequencing: every task here needs the rotor meshes of [[task-367-extractor-rotors]] (todo, runs after [[task-368-extractor-rebake]], in progress); the untracked SM_Extractor_Body|Rotor_*.fbx in Art/Meshes/extractor/ are partial task-367 outputs that its rerun replaces, so no subtask here uses them before task-367 is done. Questions Q1 to Q8 are asked once, all in [[task-369-extractor-level-exit]]; each global lists the ones that apply. Q7 and Q8 apply here.
- goal: A Niagara rotor wash (Q7) plays under each rotor of every extractor, its intensity following the rotor speed and its ground ring fading with height.
- acceptance: NS_ExtractorRotorWash: downward translucent streaks and a floor dust ring, user parameters Intensity (0-1) and floor height, ring only within 300 uu of the floor, pale grey-beige at low opacity; four instances under the rotors of BP_ExtractionShuttle, Intensity bound to OnRotorSpeedChanged (no tick polling), floor height set once at spawn; robots and pickups stay readable; doc nodes created and updated; capture per Q8; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved: lands when the alert bar is full, 37-50 cells from the miner, 45 s countdown, reaching it ends the level, missing it loses the run, landing clearance on a clear 4 x 3-cell footprint, look = the Rustwing extractor with four separate rotors), [[level-flow]] (step 1 Land, no arrival described), [[intermission]] (stats screen drawn over the paused finished level, cards screen opaque), [[run-structure]], [[destruction-rules]] (approved: only three things break blocks), [[ore-blocks]] (a deposit is never left partly destroyed; start-zone drops thrown to the ground), [[color-code]] (shuttle white + teal), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, nose +X, origin bottom centre, 50,000 triangles), tech [[sys-objectives-shuttle]], [[sys-objectives-run-flow]] (EndRun pauses the game), [[sys-run-transition-fade]] (fade in 0.5 s after the level switch; level 1 and direct PIE have no fade), [[bp-extraction-shuttle]] (white box, glow pulse, chevrons). [[readability-rules]] and [[color-code]]: no code colour used by the dust; no gdd value for the wash: asked as Q7. Precedent effect [[bp-drone-thruster]]. Needs [[task-373-extractor-rotor-spin]] first (speed event).
- confirmed_by_owner: no (plan and questions handed back 2026-10-29)
- milestone: M2
- next_step: task-374-01-bp-ns-extractor-rotor-wash
- started: open
- waiting_for: owner confirmation of the plan and answers to Q1 to Q8 (asked in task-369)
- result: open

## Subtasks
1. task-374-01-bp-ns-extractor-rotor-wash (vault/reference/tasks/task-374-extractor-rotor-wash/task-374-01-bp-ns-extractor-rotor-wash.md) — bp: create NS_ExtractorRotorWash (downward streaks, floor dust ring, Intensity and floor height parameters).
2. task-374-02-bp-shuttle-rotor-wash-wiring (vault/reference/tasks/task-374-extractor-rotor-wash/task-374-02-bp-shuttle-rotor-wash-wiring.md) — bp: BP_ExtractionShuttle: four wash components under the rotors, Intensity from OnRotorSpeedChanged, floor height at spawn.
3. task-374-03-doc-extractor-rotor-wash (vault/reference/tasks/task-374-extractor-rotor-wash/task-374-03-doc-extractor-rotor-wash.md) — doc: create bp-extractor-rotor-wash, update blueprints-fx-index and bp-extraction-shuttle.
4. task-374-04-capture-extractor-rotor-wash (vault/reference/tasks/task-374-extractor-rotor-wash/task-374-04-capture-extractor-rotor-wash.md) — capture: -game capture of the wash on the ground and during takeoff (Q8).

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-374-01-bp-ns-extractor-rotor-wash]] — step 1, bp.
- [[task-374-02-bp-shuttle-rotor-wash-wiring]] — step 2, bp.
- [[task-374-03-doc-extractor-rotor-wash]] — step 3, doc.
- [[task-374-04-capture-extractor-rotor-wash]] — step 4, capture.
- [[task-369-extractor-level-exit]] — holds the questions.
- [[task-373-extractor-rotor-spin]] — must be done first: the speed event the wash follows.
