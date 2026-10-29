---
id: task-309-06-capture-chest-opening
description: "Capture the chest opening (cpp): bonus chest opening, latch, slam, light, ring, chips and sparks, step 06."
parent: task-309-chest-opening
kind: task
status: in-progress
---
# Capture the chest opening

- goal: Headless -game run with cheats: frames of the latch swinging, the lid overshoot, hold and settle, the ground ring, the inner light and turning rays, the three chips popping and flying, the floating sparks, a camera offset across the shake frames, and the open chest from the game camera and closer: lid turning about its hinge pins, up and backward, no hinge-block stub on the body back wall, latch hasp not dangling, no dark recess behind the latch (owner 2026-10-24); contact sheet for the owner, set beside frames of the reference video.
- agent: cpp
- depends_on: task-309-05-bp-chest-open-wiring
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md); files: Art/Source/chest/reference/egor-chest-juice.mp4 (owner reference)
- expected_output: Saved/ChestOpening/ frames and contact sheet, report
- done_when: latch, lid slam, ring, light and rays, chips and sparks visible in order; open pose as in the goal (pivot on the pins, no stub, no dangling hasp, no dark recess); sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- runs: open
- milestone: M2
- started: 2026-10-29T19:13:10Z
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
