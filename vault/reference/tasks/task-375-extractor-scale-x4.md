---
id: task-375-extractor-scale-x4
description: "Global task: Scale the extractor x4 (body and rotors), its level-ending board radius and its footprint in cells; chevrons follow."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor: four times bigger, with its level-ending zone and footprint

- request: "Prépare une tâche pour agrandir l'extrateur par 4 ( avec la collision pour finir le niveau ) et détruire la roche autour" then, added to the same request: "Il ne faut pas non plus que la roche soit trop proche du vaisseau" (owner, 2026-10-29, verbatim, French, relayed by the main session; no file or image attached; "extrateur" = the extractor, ARRShuttle / BP_ExtractionShuttle). "Prépare une tâche": nodes created, nothing launched before the owner confirms. Two logical points, two globals: [[task-375-extractor-scale-x4]] (bigger extractor, its footprint and its level-ending zone) and [[task-376-extractor-rock-margin]] (rock cleared around it with a margin). No capture, test or bot run requested; the owner refused capture subtasks for the extractor chain (task-369 Q8, 2026-10-29: he looks himself), so none is planned; only the existing test expectations the new values break are updated. Questions Q1 to Q8 are asked once, all in [[task-375-extractor-scale-x4]]; each global lists the ones that apply. This global: "agrandir l'extrateur par 4 ( avec la collision pour finir le niveau )". Q1, Q2, Q3, Q6, Q7, Q8 apply. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8 accepted.
- goal: The extractor is drawn 4 times bigger (Q1), the level ends within the scaled board radius (Q2), it occupies a 16 x 12-cell footprint for its landing and arrival placement (Q3), the chevrons never sit under the hull (Q6).
- acceptance: BP_ExtractionShuttle Mesh relative scale 4 (rotors follow, attached to Mesh; rotor wash components of task-374 still under the rotors); ARRShuttle FootprintCells (16, 12); RRRunFlowConfig ShuttleBoardRadiusCells 12 (804 uu) and the level ends when the miner is within it; arrival footprint still 1 cell from the spawn cell, the miner outside the hull; chevron hide distance 600 uu; takeoff values unchanged (Q7); existing test expectations on 4 x 3 and 3 cells updated, Extraction, RunFlow and FineConsoleShuttlePlacement pass; doc updated; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (approved; extractor in game provisional: board 3 cells = 201 uu from the centre, mesh fits the 4 x 3-cell footprint, landing clearance "lands only where its whole 4 x 3-cell footprint is open floor", takeoff rise 400 uu, rotor wash within 300 uu of the floor), [[level-flow]] (arrival: 4 x 3 footprint 1 cell from the spawn cell, above him on screen), [[destruction-rules]] (approved, breaker 4 = the extractor clears its 4 x 3 footprint, task-372), [[art-prompts-extractor]] (mesh 196.6 x 201.0 x 86.1 cm, origin bottom centre). Tech: [[sys-objectives-shuttle]] (ARRShuttle FootprintCells (4, 3) in RRShuttle.h; board check 2D distance <= BoardRadius, no collision component: the "collision that ends the level" is this radius; mesh and rotors NoCollision; takeoff removal uses the 8 bounds corners, so it follows the scale), [[sys-objectives-run-flow]] (RRRunFlowConfig ShuttleBoardRadiusCells 3, ArrivalDistanceCells 1 measured to the footprint near row, so the miner stays outside a bigger footprint; FindShuttleCell keeps only spots whose whole footprint is reachable floor, else the fitting spot nearest to the 37-50 band, else any reachable cell), [[bp-extraction-shuttle]] (Mesh scale 1, rotors attached to Mesh so they follow its scale; chevrons hidden when less than 150 uu short of the shuttle centre), [[bp-rr-game-mode]] (RunFlowConfig only sets the classes: radius at the C++ default). Minimap draws the shuttle as a point and the drone only targets its location: unaffected. Not written in the gdd and needed: what "par 4" means, the new board radius, footprint and margin (Q1 to Q4). Conflict: the approved-rule landing clearance (whole footprint open) is rarely possible with a footprint 16 times larger and would push the landing out of the 37-50 band (Q5). Overlap: [[task-373-extractor-rotor-spin]] (in progress, doc step left) and [[task-374-extractor-rotor-wash]] (todo) edit BP_ExtractionShuttle and bp-extraction-shuttle: this plan runs after both (Q8).
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: task-375-03-doc-extractor-scale-x4
- started: 2026-10-29T23:39:47Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 "agrandir par 4": scale 4 on each axis of the extractor body and its rotors (about 786 x 804 x 344 uu, the hull covers about 12 x 12 cells), or scale 2 per axis (4 times the floor area). Default: scale 4 on each axis.
- Q2 zone that ends the level ("la collision pour finir le niveau", the board radius around the centre): x4 too, 12 cells = 804 uu from the centre (about 400 uu beyond the hull), or the old 100 uu beyond the hull (about 7.5 cells = 500 uu). Default: 12 cells (804 uu).
- Q3 extractor footprint in cells (used for the landing spot, the arrival placement and the rocks destroyed under it): 4 x 3 x4 = 16 x 12 cells (the hull fills about 12 x 12 of it). Default: 16 x 12 cells.
- Q4 "pas trop proche": the rock in a margin of 2 cells (134 uu) all around the 16 x 12 footprint is also destroyed (zone 20 x 16 cells), at both placements (arrival and extraction landing), with the same rule as task-372 (deposits break whole, resources stay on the floor, barrels removed without explosion); level generation unchanged (the extraction spot is chosen during the level, not at generation). Default: yes, 2 cells, both placements, no generation change.
- Q5 landing spot (conflict with the landing clearance rule of extraction): the extractor keeps landing in the 37-50 cell band, on a spot whose 16 x 12 footprint is open floor when one exists there, otherwise on any reachable floor cell of the band, and the rock of footprint + margin is destroyed; the old fallback that leaves the band to find an open spot is dropped. Default: yes, the band first, rock destroyed.
- Q6 floor chevrons (countdown arrows towards the extractor): their hide distance goes x4 too (hidden when less than 600 uu short of the centre instead of 150) so none is drawn under the bigger hull; their size and spacing stay. Default: yes.
- Q7 takeoff and rotor wash: takeoff values unchanged (rise 400 uu, slightly more than the new hull height; removal once out of view follows the size by itself); the rotor wash of task-374 is attached under the rotors and grows x4 with them. Default: yes, unchanged.
- Q8 order: these two tasks run after task-373 and task-374 are done (same Blueprint and doc nodes), task-375 then task-376; no capture, test or bot run added. Default: yes.

## Subtasks
1. task-375-01-cpp-extractor-scale-footprint (vault/reference/tasks/task-375-extractor-scale-x4/task-375-01-cpp-extractor-scale-footprint.md) — cpp: ARRShuttle FootprintCells 16 x 12 and ShuttleBoardRadiusCells 12 (804 uu); test expectations updated.
2. task-375-02-bp-shuttle-scale-x4 (vault/reference/tasks/task-375-extractor-scale-x4/task-375-02-bp-shuttle-scale-x4.md) — bp: BP_ExtractionShuttle: Mesh scale 4 (rotors and wash follow), chevron hide distance 150 -> 600 uu.
3. task-375-03-doc-extractor-scale-x4 (vault/reference/tasks/task-375-extractor-scale-x4/task-375-03-doc-extractor-scale-x4.md) — doc: doc: update sys-objectives-shuttle, sys-objectives-run-flow and bp-extraction-shuttle.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-375-01-cpp-extractor-scale-footprint]] — step 1, cpp.
- [[task-375-02-bp-shuttle-scale-x4]] — step 2, bp.
- [[task-375-03-doc-extractor-scale-x4]] — step 3, doc.
- [[task-376-extractor-rock-margin]] — runs after it: the rock margin around the enlarged footprint.
- [[task-374-extractor-rotor-wash]] — must be done first: same Blueprint (Q8).
- [[task-373-extractor-rotor-spin]] — must be done first: its doc step edits the same doc nodes (Q8).
- [[extraction]] — gdd rule the Q1-Q3 edits update.
