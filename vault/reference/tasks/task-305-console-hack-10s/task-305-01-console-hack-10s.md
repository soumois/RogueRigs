---
id: task-305-01-console-hack-10s
description: "Console HackDuration 10 s (cpp): console hack time 10 s, step 01."
parent: task-305-console-hack-10s
kind: task
status: in-progress
---
# Console HackDuration 10 s

- goal: Set the ARRConsole HackDuration default to 10 (header comment history kept), check ARRBonusChest still sets 8 and BP_ObjectiveConsole overrides nothing; update the GDD objective-console hack time (10 s, provisional owner 2026-10-23, was 12 s) and sys-objectives-console-chest; fix only the tests this change breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/RRConsole.h; Source/RogueRigs/Tests/RRRunFlowTests.cpp (asserts 12 s at line ~380)
- expected_output: C++ default changed, GDD and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass (and Integration.ConsoleExtractionWin if it is run); chest 8 s unchanged; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-305-console-hack-10s]] — parent global task.
