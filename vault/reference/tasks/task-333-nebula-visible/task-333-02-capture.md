---
id: task-333-02-capture
description: "One -game capture near an arena edge showing the retuned nebula at normal brightness, with luma numbers against the floor, sheet for the owner."
parent: task-333-nebula-visible
kind: task
status: wait-answers
---
# Nebula: capture for the owner

- goal: the owner sees the retuned nebula in game, and the Executor knows whether its peak is in the N1 band and still below the floor beside the edge. Runs only if the owner kept N5; otherwise the Executor cancels this subtask.
- agent: cpp
- depends_on: task-333-01-nebula-tune
- inputs: the global `request`, `acceptance` and `gdd_check` (vault/reference/tasks/task-333-nebula-visible.md); the task-333-01 change summary; the task-332-03 capture node for the method and the previous numbers (vault/reference/tasks/task-332-space-background/task-332-03-capture.md); agent-cpp (vault/reference/agents/agent-cpp.md, capture_script and visual_check); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, map and spawn).
- expected_output: one `-game` run through `Tools/capture/capture.js` in L_ArenaM1, miner moved near the same east edge as task-332-03 so void fills at least a third of the frame, `rr.Stress.Peace 1`, two shots about 3 s apart; `Saved/Captures/<Name>/` with the PNGs and contact_sheet.png; a short description (nebula seen without boost or not, cloud coverage, colours); numbers: nebula peak and mean void sRGB luma, floor strip mean luma beside the edge, brightest star luma. No code or asset change; no other run.
- done_when: the PNGs exist and are described; the numbers are given; nebula peak in the N1 band and below the floor strip mean (else reported as a defect with the measured ratio, not fixed here); the contact sheet path is given for the owner.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T22:00:29Z
- waiting_for: owner decision S1 of task-335; capture PNGs and contact sheet exist in Saved/Captures/task333_nebula, numbers not measured
- result: open

## Relations
- [[task-333-nebula-visible]] — parent global task.
