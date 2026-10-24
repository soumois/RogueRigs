---
id: task-312-01-telegraph-under-actors
description: "Ground telegraph under actors (cpp): zone colour does not touch the console, step 01."
parent: task-312-zone-colour-off-console
kind: task
status: in-progress
---
# Ground telegraph under actors

- goal: Find why the hack zone decal colours the console (and anything inside the zone) and fix it in the shared ground telegraph path so the disc and circles only show on the ground under actors (for example DisableDecalsOn for the console, chest and drone, or the decal projection); if the fix needs the M_GroundTelegraph asset, stop and report so a bp subtask is added; update sys-ground-telegraph; fix only the tests this change breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Telegraph/RRGroundTelegraphComponent.h/.cpp, Source/RogueRigs/Objectives/RRConsole.cpp, Source/RogueRigs/Drone/RRDrone.cpp
- expected_output: C++ fix, tech doc line, tests green
- done_when: build green; RogueRigs.GroundTelegraph.* and RogueRigs.Console.* pass; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-312-zone-colour-off-console]] — parent global task.
