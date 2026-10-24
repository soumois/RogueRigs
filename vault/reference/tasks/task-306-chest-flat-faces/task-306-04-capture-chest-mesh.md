---
id: task-306-04-capture-chest-mesh
description: "Capture the new chest in game (cpp): bonus chest mesh with flat faces, step 04."
parent: task-306-chest-flat-faces
kind: task
status: done
---
# Capture the new chest in game

- goal: Headless -game run with cheats: frames of the dropped chest from the game camera, compared with the grey-box cube before; contact sheet for the owner.
- agent: cpp
- depends_on: task-306-03-bp-chest-import
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md); files: none
- expected_output: Saved/ChestMesh/ frames and contact sheet, report
- done_when: frames show the new chest at its drop spot; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 86, cache write 149755, cache read 2476334, output 25575
- runs: 1
- milestone: M2
- result: Headless -game capture: new chest at its real drop spot from the game camera and closer, lock face toward the camera, lid and latch in place, flat faces read; before (grey-box cube, task-312 frame) | after sheet Saved/ChestMesh/contact_sheet_chest_before_after.png. Debug tool rr.Debug.Objective now accepts Chest (f37555e5, sys-cheats 5de1b024), to be confirmed by the owner.

## Relations
- [[task-306-chest-flat-faces]] — parent global task.
