---
id: task-313-02-capture-drone-socket
description: "Capture the drone at the socket (cpp): drone at the console socket while hacking, step 02."
parent: task-313-drone-at-console-socket
kind: task
status: done
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
- model: opus
- tokens: input 94, cache write 253716, cache read 3014339, output 33912
- usage: turns 47, context start 11k peak 136k, resumes 1 (message x1), 30 min, units 0.79 M
- runs: 2
- milestone: M2
- result: run 2 (after the Y mirror fix): drone hovers in front of the dark port under the green screen (+Y face), at port height, facing it, red link lands in the port recess between the lamps; sideways miss at most about 10-15 uu toward -X (pixel estimate); antenna head on the mast; Saved/DroneSocket/contact_sheet_drone_socket.png + 9 frames; run 1 had found the drone at the back face

## Relations
- [[task-313-drone-at-console-socket]] — parent global task.
