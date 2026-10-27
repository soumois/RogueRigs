---
id: task-335-07-intensity-retune
description: "Retune Intensity of MI_Space_Clouds, and MI_Space_Deep if needed, from the task-335-06 numbers: cloud peak 70 to 100, void mean below floor strips."
parent: task-335-space-texture-layers
kind: task
status: todo
---
# Space layers: intensity retune from the capture

- goal: from the measured capture, the cloud peak lands in the B1 band (70 to 100 sRGB luma) and the void mean stays below the floor strips beside the edge; the deep layer stays distinguishable from the clouds' dark gaps if a change of its Intensity can do it.
- agent: bp
- depends_on: task-335-06-capture
- inputs: the global `request` (owner answer of 2026-10-27 asking for this retune) and `feedback_2` with B1 (vault/reference/tasks/task-335-space-texture-layers.md); the task-335-06 result with the numbers (vault/reference/tasks/task-335-space-texture-layers/task-335-06-capture.md: clouds p99 180, max 232; void mean 45.5; floor strips 34.6 to 37.3; deep layer not distinguishable; clouds near-black share 14.2 percent); the task-335-03 result (vault/reference/tasks/task-335-space-texture-layers/task-335-03-space-texture-material.md: MI_Space_Clouds Intensity 2.7, Contrast 1.6; MI_Space_Deep Intensity 0.7, Contrast 1.3); images `Saved/Captures/task335_space/`; bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: new Intensity on MI_Space_Clouds, set from the measured ratio (target peak about 85: convert sRGB to linear, state the arithmetic; the capture estimate is x0.2 to 0.28, about 0.55 to 0.75); MI_Space_Deep Intensity changed only if the arithmetic shows the deep layer then reads below the cloud gaps (state the reasoning either way). Intensity only: no change of Contrast, Saturation, TileSize, PanVelocity, textures, M_SpaceTexture graph, BP_SpaceBackground or the mask. Change summary for the doc subtask: old and new values, arithmetic, commit hash. No vault doc node.
- done_when: the instances are saved with no log error; their parameters read back; only MI_Space_Clouds (and MI_Space_Deep if changed) show in `git status`; committed by explicit path.
- editor_state: open
- token_ceiling: 1.0 M units
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
