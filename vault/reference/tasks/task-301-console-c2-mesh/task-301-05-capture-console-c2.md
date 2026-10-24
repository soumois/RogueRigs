---
id: task-301-05-capture-console-c2
description: "Capture the console C2 in game (cpp): console c2 uplink mast mesh, step 05."
parent: task-301-console-c2-mesh
kind: task
status: in-progress
---
# Capture the console C2 in game

- goal: Headless -game run: frames of the dropped console with the C2 mesh from the game camera, compared with the cylinder before; contact sheet for the owner.
- agent: cpp
- depends_on: task-301-04-bp-console-c2-import
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: none
- expected_output: Saved/ConsoleC2/ frames and contact sheet, report
- done_when: frames show the C2 mesh at the console spot; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-301-console-c2-mesh]] — parent global task.
