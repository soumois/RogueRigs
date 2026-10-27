---
id: task-332-03-capture
description: "One -game capture near an arena edge showing the space background layers, with brightness numbers against the floor, sheet for the owner."
parent: task-332-space-background
kind: task
status: todo
---
# Space background: capture for the owner

- goal: the owner sees the new background in game, and the Executor knows whether it stays darker than the floor beside it (Q4). Runs only if the owner kept Q9; otherwise the Executor cancels this subtask.
- agent: cpp
- depends_on: task-332-02-space-layers-actor
- inputs: the global `request`, `acceptance` and `gdd_check` (vault/reference/tasks/task-332-space-background.md); the task-332-02 change summary (depths, placement); agent-cpp (vault/reference/agents/agent-cpp.md, capture_script and visual_check); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, map and spawn).
- expected_output: one `-game` run through `Tools/capture/capture.js` in L_ArenaM1 with the miner moved near an arena edge so void fills at least a third of the frame, `rr.Stress.Peace 1`, two shots about 3 s apart (to see twinkle and flow change); `Saved/Captures/<Name>/` with the PNGs and contact_sheet.png; a short description of what is seen (layers distinguishable, plane edge or not, tiling or not); numbers: mean sRGB luma of the void area and of the floor strip beside the edge, brightest star luma. No code change; no other run.
- done_when: the PNGs exist and are described; the void mean luma is below the floor strip mean luma (else reported as a defect, not fixed here); no plane edge visible (else reported); the contact sheet path is given for the owner.
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
- [[task-332-space-background]] — parent global task.
