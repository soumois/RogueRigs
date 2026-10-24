---
id: task-313-01-drone-socket-hover
description: "Drone hover point at the socket (cpp): drone at the console socket while hacking, step 01."
parent: task-313-drone-at-console-socket
kind: task
status: todo
---
# Drone hover point at the socket

- goal: During ARRDrone hack of a console, move the drone to a hover point in front of the round data socket (front console block of SM_ConsoleBase, at its height, facing it) and aim the hack link at the socket; the point comes from the scaled mesh (a socket on SM_ConsoleBase read by name, or an offset constant scaled by the Mesh scale, implementation choice reported); blocked point: existing rule (back toward the miner); leaving the zone unchanged; update sys-drone and sys-objectives-console-chest; fix only the tests this change breaks.
- agent: cpp
- depends_on: task-311-03-capture-console-bigger
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), drone (vault/reference/gdd/player/drone.md), objective-console (vault/reference/gdd/run/objective-console.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Drone/RRDrone.h/.cpp, Source/RogueRigs/Objectives/RRConsole.h/.cpp; mesh reference: Art/Source/console-meshy/owner/console-c2-turnaround.webp
- expected_output: C++ hover point and aim, tech doc lines, tests green
- done_when: build green; RogueRigs.Drone.* and RogueRigs.Console.* pass; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-313-drone-at-console-socket]] — parent global task.
