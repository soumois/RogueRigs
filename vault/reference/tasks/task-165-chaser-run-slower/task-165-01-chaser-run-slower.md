---
id: task-165-01-chaser-run-slower
description: "Slow the chaser run animation by 30 percent (playback rate 0.7), as a tunable material parameter."
parent: task-165-chaser-run-slower
kind: task
status: in-progress
---
# Chaser: slower run animation

- goal: Slow the chaser run animation by 30 percent (playback rate 0.7), as a tunable material parameter.
- agent: bp
- depends_on: none
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In M_Chaser_Meshy_Anim a scalar parameter RunRate (default 0.7, editable in the material instance or the material) that scales the playback of the run clip only: frames per second of the run clip = 30 x RunRate (the run loop then lasts 0.762 s instead of 0.533 s); the attack clip keeps 30 fps; implemented by passing RunRate into the Custom node ChaserAnimWPO (a new input) and using t_run = max(Time - Start, 0) x 30 x RunRate for the run branch only; the doc bp-chaser updated with the parameter and its meaning
- done_when: The material recompiles without error; the code and the new input read back; the parameter default 0.7 read back; saved and committed by name; docs updated; the editor stays open; the owner is told how to tune it (RunRate in the material) and that the attack clip is unchanged
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-165-chaser-run-slower]] — parent global task.
