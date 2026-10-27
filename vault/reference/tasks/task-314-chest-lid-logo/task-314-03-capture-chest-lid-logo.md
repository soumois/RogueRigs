---
id: task-314-03-capture-chest-lid-logo
description: "Capture the chest with its lid logo in game (cpp): yellow logo on the chest lid, step 03."
parent: task-314-chest-lid-logo
kind: task
status: todo
---
# Capture the chest lid logo in game

- goal: Headless -game run with cheats (rr.Debug.Objective Chest): frames of the dropped chest from the game camera and closer, compared with Saved/ChestMesh/ frames of task-306-04; before | after contact sheet for the owner.
- agent: cpp
- depends_on: task-314-02-bp-chest-lid-logo
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md); files: Saved/ChestMesh/
- expected_output: Saved/ChestLogo/ frames and contact sheet, report
- done_when: frames show the yellow mark on the lid at the drop spot; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-314-chest-lid-logo]] — parent global task.
