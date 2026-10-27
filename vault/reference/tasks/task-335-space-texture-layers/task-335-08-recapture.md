---
id: task-335-08-recapture
description: "Second -game capture at the same east edge after the intensity retune: numbers, contact sheet, side-by-side with the task-335-06 capture."
parent: task-335-space-texture-layers
kind: task
status: done
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
- model: opus
- tokens: input 30, cache write 30805, cache read 274827, output 7322
- usage: turns 14, context start 10k peak 31k, resumes 1 (message x1), 2 min, units 0.10 M
- runs: 1
- milestone: M3
- started: 2026-10-27T23:12:13Z
- waiting_for: none
- result: Recapture done (Saved/Captures/task335_space_retune/). Void mean 20.5 below floor strips 33.7-39.0 (pass). Cloud p99 116 vs band 70-100: ACCEPTANCE DEVIATION CHOSEN BY THE OWNER (2026-10-27 'Ok pour ça, passe à la 334'), kept on purpose, no second retune. Intensity MI_Space_Clouds 0.6, MI_Space_Deep 0.7. Deep layer still not distinguishable. Void near-black share 19.2 percent.

## Relations
- [[task-335-space-texture-layers]] — parent global task.
