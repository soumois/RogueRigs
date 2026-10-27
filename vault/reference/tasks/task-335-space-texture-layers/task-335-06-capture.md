---
id: task-335-06-capture
description: "One -game capture near the east arena edge showing the texture layers, luma numbers, comparison with the task-333 capture, sheet for the owner."
parent: task-335-space-texture-layers
kind: task
status: done
---
# Space layers: capture for the owner

- goal: the owner sees the new layered background in game next to the task-333 look, and the Executor knows whether the clouds sit in the B1 band. Runs only if the owner kept S9; otherwise the Executor cancels this subtask.
- agent: cpp
- depends_on: task-335-04-space-layers, task-335-05-void-mask-gain
- inputs: the global `request`, `acceptance` and `gdd_check` (vault/reference/tasks/task-335-space-texture-layers.md); the task-335-03, 04 and 05 change summaries; the task-333-02 capture node for the method (vault/reference/tasks/task-333-nebula-visible/task-333-02-capture.md) and its images `Saved/Captures/task333_nebula/`; agent-cpp (vault/reference/agents/agent-cpp.md, capture_script and visual_check); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md).
- expected_output: one `-game` run through `Tools/capture/capture.js` in L_ArenaM1, miner at the same east edge as task-333-02, `rr.Stress.Peace 1`, two shots about 3 s apart; `Saved/Captures/task335_space/` with the PNGs and contact_sheet.png, plus a side-by-side with `Saved/Captures/task333_nebula/contact_sheet.png`; a short description (cloud structure, both layers distinct, visible tiling or not, drift between the shots, colours); numbers: cloud peak and mean void sRGB luma, floor strip mean beside the edge, brightest star. No code or asset change; no other run.
- done_when: the PNGs exist and are described; the numbers are given; cloud peak in the B1 band (70 to 100 sRGB luma) and void mean below the floor strip mean (else reported as a defect with the ratio, not fixed here); the sheet paths are given for the owner.
- editor_state: any
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 26, cache write 33928, cache read 278016, output 9244
- usage: turns 13, context start 11k peak 34k, resumes 0, 2 min, units 0.12 M
- runs: 1
- milestone: M3
- started: 2026-10-27T22:53:36Z
- waiting_for: none
- result: Capture done (Saved/Captures/task335_space/, contact_sheet.png, side_by_side_task333_vs_task335.png). DEFECTS reported, not fixed: cloud peak p99 180 / max 232 (B1 band 70-100: Intensity x about 0.2 to 0.28 needed); void mean 45.5 above floor strip 34.6-37.3. Layers: dense saturated clouds, no tiling, no plane edge, drift ~2px/3s; deep layer not distinguishable. Mask-gain FarBrightness reads 0.06.

## Relations
- [[task-335-space-texture-layers]] — parent global task.
