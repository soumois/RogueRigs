---
id: task-335-08-recapture
description: "Second -game capture at the same east edge after the intensity retune: numbers, contact sheet, side-by-side with the task-335-06 capture."
parent: task-335-space-texture-layers
kind: task
status: todo
---
# Space layers: capture after the retune

- goal: the owner sees the retuned layers in game next to the first capture, and the Executor knows whether the cloud peak is in the B1 band and the void mean below the floor strips.
- agent: cpp
- depends_on: task-335-07-intensity-retune
- inputs: the global `request`, `acceptance` and `feedback_2` (vault/reference/tasks/task-335-space-texture-layers.md); the task-335-07 change summary; the task-335-06 node for the method and the previous numbers (vault/reference/tasks/task-335-space-texture-layers/task-335-06-capture.md) and its images `Saved/Captures/task335_space/`; agent-cpp (vault/reference/agents/agent-cpp.md, capture_script and visual_check); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md).
- expected_output: one `-game` run through `Tools/capture/capture.js` in L_ArenaM1, miner at the same east edge as task-335-06, `rr.Stress.Peace 1`, two shots about 3 s apart; `Saved/Captures/task335_space_retune/` with the PNGs and contact_sheet.png, plus a side-by-side with `Saved/Captures/task335_space/contact_sheet.png`; a short description (cloud structure, deep layer distinct or not, colours); numbers measured the same way as task-335-06: cloud p99 and max, void mean, floor strip means beside the edge, brightest star, clouds near-black share. No code or asset change; no other run.
- done_when: the PNGs exist and are described; the numbers are given next to the task-335-06 numbers; cloud peak in 70 to 100 and void mean below the floor strips (else reported as a defect with the ratio, not fixed here); the sheet paths are given for the owner.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
