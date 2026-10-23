---
id: task-304-01-console-marked-rocks-range
description: "Drop picker marked-rock range (cpp): console marked rocks between 10 and 40, step 01."
parent: task-304-console-marked-rocks-range
kind: task
status: done
---
# Drop picker marked-rock range

- goal: ARRConsole gets MinMarkedCells 10 and MaxMarkedCells 40 (EditDefaultsOnly, provisional owner 2026-10-23); the console drop picker counts FindMarkableCells per candidate and accepts only counts in range; if none fits, the candidate with the nearest count, logged as a warning; the bonus chest (empty MarkedCellTypes) is not filtered; GDD objective-console and sys-objectives docs; fix only tests the change breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), level-flow (vault/reference/gdd/run/level-flow.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/
- expected_output: C++ change, GDD and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 36, cache write 80469, cache read 573541, output 10694
- runs: 1
- milestone: M2
- result: ARRConsole MinMarkedCells 10 / MaxMarkedCells 40 (EditDefaultsOnly, provisional owner 2026-10-23); console picker keeps spots in range, else nearest count with a warning; chest unfiltered; GDD objective-console, sys-objectives-console-chest, sys-objectives-run-flow, bp-objective-console updated; build green; Console.* and RunFlow.* 7/7 pass, no test changed; code 0a357aea, docs 3134b212

## Relations
- [[task-304-console-marked-rocks-range]] — parent global task.
