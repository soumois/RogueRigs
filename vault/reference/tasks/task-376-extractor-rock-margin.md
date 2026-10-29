---
id: task-376-extractor-rock-margin
description: "Global task: At arrival and extraction landing, the rock of the enlarged footprint plus a margin around it is destroyed; landing stays in its band."
parent: tasks-done-index
kind: task
status: done
---
# Extractor: rock destroyed under it and in a margin around it

- request: "Prépare une tâche pour agrandir l'extrateur par 4 ( avec la collision pour finir le niveau ) et détruire la roche autour" then, added to the same request: "Il ne faut pas non plus que la roche soit trop proche du vaisseau" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached; "extrateur" = the extractor, ARRShuttle / BP_ExtractionShuttle). "Prépare une tâche": nodes created, nothing launched before the owner confirms. Two logical points, two globals: [[task-375-extractor-scale-x4]] (bigger extractor, its footprint and its level-ending zone) and [[task-376-extractor-rock-margin]] (rock cleared around it with a margin). No capture, test or bot run requested; the owner refused capture subtasks for the extractor chain (task-369 Q8, 2026-10-29: he looks himself), so none is planned; only the existing test expectations the new values break are updated. Questions Q1 to Q8 are asked once, all in [[task-375-extractor-scale-x4]]; each global lists the ones that apply. This global: "détruire la roche autour" and "Il ne faut pas non plus que la roche soit trop proche du vaisseau". Q4, Q5, Q8 apply. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q4, Q5, Q8 accepted.
- goal: No rock stays under the enlarged extractor or within the margin around it, at arrival and at extraction landing (Q4), and the extraction landing stays in the 37-50 cell band (Q5).
- acceptance: New ARRShuttle tunable for the margin (2 cells, provisional, owner 2026-10-29); both ClearExtractorFootprint calls (PlaceArrivalShuttle and LandShuttle) clear footprint + margin, clamped to the grid, with the task-372 rule (deposits whole, resources on the floor, barrels without explosion, log line); FindShuttleCell picks a cell in the 37-50 band, an open 16 x 12 footprint first, else any reachable floor cell of the band, never the out-of-band open spot; existing test expectations the change breaks updated, Extraction, RunFlow and FineConsoleShuttlePlacement pass; doc updated; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved; extractor in game provisional: board 3 cells = 201 uu from the centre, mesh fits the 4 x 3-cell footprint, landing clearance "lands only where its whole 4 x 3-cell footprint is open floor", takeoff rise 400 uu, rotor wash within 300 uu of the floor), [[level-flow]] (arrival: 4 x 3 footprint 1 cell from the spawn cell, above him on screen), [[destruction-rules]] (approved, breaker 4 = the extractor clears its 4 x 3 footprint, task-372), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, origin bottom centre). Tech: [[sys-objectives-shuttle]] (ARRShuttle FootprintCells (4, 3) in RRShuttle.h; board check 2D distance <= BoardRadius, no collision component: the "collision that ends the level" is this radius; mesh and rotors NoCollision; takeoff removal uses the 8 bounds corners, so it follows the scale), [[sys-objectives-run-flow]] (RRRunFlowConfig ShuttleBoardRadiusCells 3, ArrivalDistanceCells 1 measured to the footprint near row, so the miner stays outside a bigger footprint; FindShuttleCell keeps only spots whose whole footprint is reachable floor, else the fitting spot nearest to the 37-50 band, else any reachable cell), [[bp-extraction-shuttle]] (Mesh scale 1, rotors attached to Mesh so they follow its scale; chevrons hidden when less than 150 uu short of the shuttle centre), [[bp-rr-game-mode]] (RunFlowConfig only sets the classes: radius at the C++ default). Minimap draws the shuttle as a point and the drone only targets its location: unaffected. Not written in the gdd and needed: what "par 4" means, the new board radius, footprint and margin (Q1 to Q4). Conflict: the approved-rule landing clearance (whole footprint open) is rarely possible with a footprint 16 times larger and would push the landing out of the 37-50 band (Q5). Overlap: [[task-373-extractor-rotor-spin]] (in progress, doc step left) and [[task-374-extractor-rotor-wash]] (todo) edit BP_ExtractionShuttle and bp-extraction-shuttle: this plan runs after both (Q8). The margin extends breaker 4 of [[destruction-rules]] (approved) and replaces the landing clearance of [[extraction]] (edits of Q4 and Q5). Needs [[task-375-extractor-scale-x4]] first (footprint 16 x 12).
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: none
- started: 2026-10-29T23:48:11Z
- waiting_for: none
- result: Done. ClearMarginCells, GetClearRect and in-band landing documented in 3 nodes; commit c02cf3d6; lint OK

## Questions
Decided 2026-10-29 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-29.
- Q4 margin of 2 cells around the 16 x 12 footprint, at arrival and extraction landing, no generation change (full text in task-375). Default: yes.
- Q5 landing stays in the 37-50 band, open footprint first, else any reachable floor cell, rock destroyed (full text in task-375). Default: yes.
- Q8 runs after task-375. Default: yes.

## Subtasks
1. task-376-01-cpp-extractor-rock-margin (vault/reference/tasks/task-376-extractor-rock-margin/task-376-01-cpp-extractor-rock-margin.md) — cpp: margin tunable, footprint + margin cleared at both placements, landing kept in the 37-50 band.
2. task-376-02-doc-extractor-rock-margin (vault/reference/tasks/task-376-extractor-rock-margin/task-376-02-doc-extractor-rock-margin.md) — doc: doc: update sys-objectives-shuttle, sys-objectives-run-flow and sys-grid-api.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-376-01-cpp-extractor-rock-margin]] — step 1, cpp.
- [[task-376-02-doc-extractor-rock-margin]] — step 2, doc.
- [[task-375-extractor-scale-x4]] — must be done first; holds the questions.
- [[task-372-extractor-clears-rocks]] — the footprint clear this extends.
- [[destruction-rules]] — breaker 4 extended by the Q4 edit.
