---
id: task-314-03-capture-chest-lid-logo
description: "Capture the chest with its lid logo in game (cpp): yellow logo on the chest lid, step 03."
parent: task-314-chest-lid-logo
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-314
- usage: included in the executor run of task-314
- runs: 1
- milestone: M2
- started: 2026-11-05T23:14:26Z
- result: One capture.js launch (L_ArenaM1, 1600x900, same chest-drop recipe as task-306-04 run5): frames in Saved/Captures/ChestLogo/. Seen: the yellow mark centred on the closed lid top at the drop spot, in the game-camera frame (cl_game_00, small but readable) and closer (cl_close_00, cl_low_00; zoom crop_close.png). Before|after sheet Saved/Captures/ChestLogo/before_after_full.png (before = Saved/ChestMesh/cm_after_drop2_*). Not seen: the mark on the open lid (open frames look at the lid underside); it is part of the lid mesh so it moves with it by construction. The hack beam lands on the mark in these frames. Lid-top shading streaks from the Meshy source remain (not in scope). Nothing else changed.

## Relations
- [[task-314-chest-lid-logo]] — parent global task.
