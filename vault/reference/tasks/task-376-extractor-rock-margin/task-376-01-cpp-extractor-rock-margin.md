---
id: task-376-01-cpp-extractor-rock-margin
description: "Cpp: margin tunable, footprint + margin cleared at both placements, landing kept in the 37-50 band."
parent: task-376-extractor-rock-margin
kind: task
status: in-progress
---
# Extractor: rock destroyed under it and in a margin around it: cpp

- goal: Both extractor placements clear footprint + margin and the landing stays in its band (Q4, Q5).
- agent: cpp
- depends_on: task-375-03-doc-extractor-scale-x4
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), extraction (vault/reference/gdd/run/extraction.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), task-376-extractor-rock-margin (vault/reference/tasks/task-376-extractor-rock-margin.md) request, gdd_check and Questions (decided values), Source/RogueRigs/Objectives/RRRunFlowSubsystem.cpp (FindShuttleCell, LandShuttle, PlaceArrivalShuttle), Source/RogueRigs/Objectives/RRShuttle.h, Source/RogueRigs/Grid/RRArenaGrid.cpp (ClearExtractorFootprint)
- expected_output: ARRShuttle EditDefaultsOnly ClearMarginCells 2 with getter; LandShuttle and PlaceArrivalShuttle pass MinCell - margin and Size + 2 x margin (clamped to the grid) to ClearExtractorFootprint, log context unchanged; FindShuttleCell: in-band open-footprint candidates first, else in-band reachable floor cells, else the old fallbacks only when the band has no reachable cell; test expectations the change breaks updated, no new test; one commit; change summary.
- done_when: build succeeds; RogueRigs.Extraction, RogueRigs.RunFlow and Integration.FineConsoleShuttlePlacement pass; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T23:48:11Z
- waiting_for: none
- result: open

## Relations
- [[task-376-extractor-rock-margin]] — parent global task.
