---
id: task-313-02-capture-drone-socket
description: "Capture the drone at the socket (cpp): drone at the console socket while hacking, step 02."
parent: task-313-drone-at-console-socket
kind: task
status: todo
---
# Capture the drone at the socket

- goal: Headless -game run: frames of the console hack with the drone hovering at the socket and the link aimed at it, at the play zoom and closer, contact sheet for the owner.
- agent: cpp
- depends_on: task-313-01-drone-socket-hover
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), drone (vault/reference/gdd/player/drone.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: none
- expected_output: Saved/DroneSocket/ frames and contact sheet, report
- done_when: drone seen in front of the socket, at its height, facing it, link on the socket; sheet written; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-313-drone-at-console-socket]] — parent global task.
