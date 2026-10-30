---
id: task-377-extractor-board-box
description: "Global task: The level-ending zone of the extractor becomes a box component the owner resizes and moves in BP_ExtractionShuttle, replacing the 12-cell radius."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor: adjustable box collision for the end of the level

- request: "Pour la collision qui détecte la fin du niveau ( sur l'extracteur ), est-ce que je pourrais avoir une box collision que je pourrais ajuster" (owner, 2026-10-30, verbatim, French, relayed by the main session; no file or image attached; "extracteur" = the extractor, ARRShuttle / BP_ExtractionShuttle). One logical point, one global. Today there is no collision: ARRShuttle::Tick ends the level when the miner is within a 2D distance BoardRadius of the actor centre (FRRRunFlowConfig ShuttleBoardRadiusCells 12 = 804 uu, task-375). No capture, test or bot run requested; the owner refused capture subtasks for the extractor chain (task-369 Q8: he looks himself), so none is planned; only the existing test expectations the change breaks are updated. Questions Q1 to Q6 below, each with a default. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- goal: The end of the level is detected by a box component on the extractor whose size, position and rotation the owner edits in the BP_ExtractionShuttle Details panel and viewport (Q1 to Q6).
- acceptance: ARRShuttle has a UBoxComponent BoardZone (VisibleAnywhere, BlueprintReadOnly, attached to ShuttleRoot, not to Mesh, so the Mesh x4 scale does not multiply it), default extent per Q2, query-only, overlapping the Pawn channel only, overlap events on, hidden in game (Q4); the run is won when the miner (not the drone, Q3) overlaps it: begin-overlap event plus one check at InitializeShuttle for a miner already inside, same Finish(true) path, countdown and dead-miner freeze unchanged; inactive (no collision) on the arrival extractor (Q5); the 12-cell radius removed (Q1: ShuttleBoardRadiusCells, the InitializeShuttle radius parameter, BoardRadius, GetBoardRadius) with its callers; test expectations the change breaks updated (Extraction defaults, OverlapWin, Integration.FineConsoleShuttlePlacement board check), no new test; RogueRigs.Extraction, RogueRigs.RunFlow and Integration.FineConsoleShuttlePlacement pass; no Blueprint edit (Q6); doc updated; gdd extraction updated; lint clean; committed by path.
- gdd_check: read 2026-10-30: [[extraction]] (approved; extractor in game provisional: "walking within 12 cells (804 uu) of its centre validates the level, no button"; arrival extractor has no board check; footprint 16 x 12 cells = 1072 x 804 uu, hull about 786 x 804 x 344 uu). Tech: [[sys-objectives-shuttle]] and Source/RogueRigs/Objectives/RRShuttle.cpp (Tick: FVector::DistSquared2D(miner, actor) <= BoardRadius^2 -> Finish(true); root ShuttleRoot, Mesh scale 4 in the Blueprint, Mesh and rotors NoCollision; InitializeArrival sets BoardRadius 0 and never runs the check; actor origin on the floor at the footprint centre), [[sys-objectives-run-flow]] (RRRunFlowConfig ShuttleBoardRadiusCells 12, passed as cells x cell size by RRRunFlowSubsystem.cpp InitializeShuttle; only caller), [[bp-extraction-shuttle]] (no collision component; chevrons hidden within 600 uu of the shuttle centre, independent of the radius, unchanged), [[sys-tests-console-extraction]] (Extraction tests: radius default 12, OverlapWin teleports the miner 50 uu from the centre), Source/RogueRigs/Tests/RRFineGridIntegrationTests.cpp (counts board cells with GetBoardRadius). Miner: capsule, Pawn object type (RRMiner.cpp, projectiles set to overlap). Conflict: none; the gdd zone shape changes from a circle to a box (Q1, Q2, gdd edit). Open values needed: the box default size and height (Q2), what counts as touching (Q3), visibility (Q4). Scope: M2 like tasks 375 and 376. Minimap point and drone targeting unaffected. Overlap: none in progress (task-314 todo, unrelated).
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- next_step: task-377-01-cpp-extractor-board-box
- started: 2026-10-30T00:40:20Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 La box remplace le rayon actuel de 12 cases (804 uu) : le réglage ShuttleBoardRadiusCells disparaît de BP_RRGameMode, seule la box compte. Autre choix : garder le rayon en plus (la fin se déclenche avec l'un ou l'autre). Default: la box remplace le rayon.
- Q2 Taille de départ de la box : un carré de 1608 x 1608 uu centré sur l'extracteur (804 uu de chaque côté, le carré autour du cercle actuel, donc rien ne change avant que tu l'ajustes), 400 uu de haut depuis le sol. Autre choix : ajustée à l'emprise de l'extracteur, 1072 x 804 uu. Tu règles ensuite taille, position et rotation dans BP_ExtractionShuttle (composant BoardZone). Default: carré 1608 x 1608, hauteur 400.
- Q3 Ce qui déclenche la fin : la capsule du mineur qui touche la box (overlap Unreal, événement, pas de test à chaque frame) ; le drone ne compte pas. Autre choix : le centre du mineur à l'intérieur de la box. Default: la capsule du mineur qui touche la box.
- Q4 Visibilité : la box se voit en fil de fer dans l'éditeur (viewport du Blueprint et du niveau), invisible en jeu. Default: invisible en jeu.
- Q5 L'extracteur d'arrivée (début de niveau, qui décolle) : box inactive, comme aujourd'hui où il n'a pas de détection. Default: inactive.
- Q6 Pas de sous-tâche Blueprint : la box apparaît toute seule dans BP_ExtractionShuttle avec la taille de Q2, et c'est toi qui l'ajustes ; pas de capture ni de nouveau test (seuls les tests existants cassés par le changement sont mis à jour). Default: oui.

## Subtasks
1. task-377-01-cpp-extractor-board-box (vault/reference/tasks/task-377-extractor-board-box/task-377-01-cpp-extractor-board-box.md) — cpp: BoardZone box component on ARRShuttle, miner overlap ends the level, radius removed, broken test expectations updated.
2. task-377-02-doc-extractor-board-box (vault/reference/tasks/task-377-extractor-board-box/task-377-02-doc-extractor-board-box.md) — doc: update sys-objectives-shuttle, sys-objectives-run-flow, bp-extraction-shuttle, sys-tests-console-extraction and sys-tests-dig-ore.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-377-01-cpp-extractor-board-box]] — step 1, cpp.
- [[task-377-02-doc-extractor-board-box]] — step 2, doc.
- [[extraction]] — gdd rule whose level-ending zone this changes.
- [[task-375-extractor-scale-x4]] — set the 12-cell radius this box replaces.
- [[task-376-extractor-rock-margin]] — previous extractor task.
