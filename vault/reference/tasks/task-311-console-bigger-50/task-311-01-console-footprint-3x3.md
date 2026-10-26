---
id: task-311-01-console-footprint-3x3
description: "Console footprint 3 x 3 (cpp): console 50 % bigger, step 01."
parent: task-311-console-bigger-50
kind: task
status: done
---
# Console footprint 3 x 3

- goal: ARRConsole FootprintCells default 2 x 2 -> 3 x 3 (header comment history kept; an odd size puts the actor at the anchor cell centre, check the zone centre and marking still use the footprint centre); ARRBonusChest sets FootprintCells 2 x 2 explicitly in its constructor (only that line, the chest files are otherwise owned by tasks 306-310); update the GDD objective-console landing clearance (3 x 3, provisional owner 2026-10-24, was 2 x 2) and sys-objectives-console-chest; fix only the tests this change breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-console-extraction (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-console-extraction.md); files: Source/RogueRigs/Objectives/RRConsole.h (FootprintCells line ~266), RRConsole.cpp, RRBonusChest.cpp, RRLandingFootprint.h; tests asserting 2 x 2: Source/RogueRigs/Tests/RRRunFlowTests.cpp (~301), RRFineGridIntegrationTests.cpp (~430)
- expected_output: C++ default changed, chest kept 2 x 2, GDD and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.*, RogueRigs.RunFlow.*, the bonus chest tests (if present) and Integration.ConsoleExtractionWin pass; the console footprint is 3 x 3 and the chest 2 x 2; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 42, cache write 38423, cache read 556193, output 8911
- usage: turns 21, context start 11k peak 38k, resumes 0, 10 min, units 0.15 M
- runs: 1
- milestone: M2
- result: ARRConsole FootprintCells 2 x 2 -> 3 x 3 (RRConsole.h, commit 5f7a8010): actor and zone centre at the anchor cell centre; ARRBonusChest sets 2 x 2 explicitly (one constructor line). Tests fixed: RunFlow footprint count and marking offset from the footprint centre (RRRunFlowTests.cpp), FineConsoleShuttlePlacement 3 x 3 (RRFineGridIntegrationTests.cpp). GDD objective-console and sys-objectives-console-chest updated (commit f7875a91). Build exit 0; 22/22 tests pass (Console.*, RunFlow.*, HUD.*, Integration.FineConsoleShuttlePlacement, Integration.ConsoleExtractionWin).

## Relations
- [[task-311-console-bigger-50]] — parent global task.
