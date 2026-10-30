---
id: task-377-01-cpp-extractor-board-box
description: "Cpp: BoardZone box component on ARRShuttle, miner overlap ends the level, radius removed, broken test expectations updated."
parent: task-377-extractor-board-box
kind: task
status: in-progress
---
# Extractor: adjustable box collision for the end of the level: cpp

- goal: The extraction win comes from the miner overlapping an editable box component instead of the 2D radius (Q1 to Q5).
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-tests-console-extraction (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-console-extraction.md), extraction (vault/reference/gdd/run/extraction.md), task-377-extractor-board-box (vault/reference/tasks/task-377-extractor-board-box.md) request, acceptance, gdd_check and Questions (decided values), Source/RogueRigs/Objectives/RRShuttle.h, Source/RogueRigs/Objectives/RRShuttle.cpp (constructor, InitializeShuttle, InitializeArrival, Tick), Source/RogueRigs/Objectives/RRRunFlowConfig.h (ShuttleBoardRadiusCells), Source/RogueRigs/Objectives/RRRunFlowSubsystem.cpp (InitializeShuttle call), Source/RogueRigs/Tests/RRExtractionTests.cpp, Source/RogueRigs/Tests/RRFineGridIntegrationTests.cpp (FineConsoleShuttlePlacement board check)
- expected_output: ARRShuttle: UBoxComponent BoardZone (VisibleAnywhere, BlueprintReadOnly, category Shuttle, attached to ShuttleRoot) with the Q2 default extent and relative location (box bottom on the floor), query-only collision overlapping only ECC_Pawn, overlap events on, bHiddenInGame per Q4, no navigation impact; OnComponentBeginOverlap bound in BeginPlay or PostInitializeComponents, filtered on the miner (Q3), calls Finish(true) under the same guards as the current Tick check (initialized, not finished, not taking off, miner valid and alive); InitializeShuttle also checks a miner already inside; InitializeArrival disables BoardZone collision (Q5); Tick keeps the countdown only; radius removed per Q1 with its config field and callers; header comments updated; existing test expectations the change breaks updated, no new test; one commit; change summary (public API, defaults, removed members, tests touched).
- done_when: build succeeds; RogueRigs.Extraction, RogueRigs.RunFlow and Integration.FineConsoleShuttlePlacement pass; no reference to ShuttleBoardRadiusCells or GetBoardRadius left in Source/ (if Q1 = replace); committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T00:40:20Z
- waiting_for: none
- result: open

## Relations
- [[task-377-extractor-board-box]] — parent global task.
