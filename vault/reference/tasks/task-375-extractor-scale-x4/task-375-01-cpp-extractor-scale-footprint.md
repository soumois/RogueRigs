---
id: task-375-01-cpp-extractor-scale-footprint
description: "Cpp: ARRShuttle FootprintCells 16 x 12 and ShuttleBoardRadiusCells 12 (804 uu); test expectations updated."
parent: task-375-extractor-scale-x4
kind: task
status: done
---
# Extractor: four times bigger, with its level-ending zone and footprint: cpp

- goal: The extractor footprint and its level-ending board radius match the x4 scale (Q2, Q3).
- agent: cpp
- depends_on: task-374-03-doc-extractor-rotor-wash
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), extraction (vault/reference/gdd/run/extraction.md), level-flow (vault/reference/gdd/run/level-flow.md), task-375-extractor-scale-x4 (vault/reference/tasks/task-375-extractor-scale-x4.md) request, gdd_check and Questions (decided values), Source/RogueRigs/Objectives/RRShuttle.h (FootprintCells), Source/RogueRigs/Objectives/RRRunFlowConfig.h (ShuttleBoardRadiusCells), Source/RogueRigs/Tests/RRExtractionTests.cpp, Source/RogueRigs/Tests/RRFineGridIntegrationTests.cpp
- expected_output: FootprintCells default (16, 12) with its comment; ShuttleBoardRadiusCells default 12 with its comment (x4 of 3, owner 2026-10-29); the tests that assert 4 x 3 and 3 cells expect the new values; no new test; one commit; change summary (with the FindShuttleCell candidate count seen in the tests, for task-376).
- done_when: build succeeds; RogueRigs.Extraction, RogueRigs.RunFlow and Integration.FineConsoleShuttlePlacement pass; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 28, cache write 49182, cache read 1092769, output 7214
- usage: turns 14, context start 58k peak 93k, resumes 0, 5 min, units 0.21 M
- runs: 1
- milestone: M2
- started: 2026-10-29T23:39:47Z
- waiting_for: none
- result: FootprintCells 16x12, ShuttleBoardRadiusCells 12; Extraction, RunFlow, FineConsoleShuttlePlacement pass; commit 8fae3518

## Relations
- [[task-375-extractor-scale-x4]] — parent global task.
