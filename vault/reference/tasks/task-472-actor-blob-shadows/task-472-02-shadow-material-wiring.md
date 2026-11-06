---
id: task-472-02-shadow-material-wiring
description: "Bp: The purple-grey soft-edged shadow material, assigned to every listed actor with its start values."
parent: task-472-actor-blob-shadows
kind: task
status: done
---
# Light-independent purple-grey shadows under actors: bp

- goal: The purple-grey soft-edged shadow material, assigned to every listed actor with its start values.
- agent: bp
- depends_on: task-472-01-shadow-component
- inputs: visual-style (vault/reference/gdd/art-audio/visual-style.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md)
- expected_output: see done_when
- done_when: A3, A4, A6 hold in a -game frame in and out of the miner light pool; each listed actor shows its shadow; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-472
- usage: included in the executor run of task-472
- runs: 1
- milestone: M2
- started: 2026-11-06T18:09:02Z
- waiting_for: none
- result: Done (0028c7a2). M_BlobShadow rebuilt headless by Tools/unreal/blob_shadow_material.py (MCP editor not needed): Unlit Translucent two sided; Tint -> Emissive; custom node BlobShadowMask = (1 - smoothstep(EdgeStart 0.8, 1, radius)) x Opacity, Opacity = scalar parameter on custom primitive data 0 (default 0.6, written per actor by the component). Tint stored linear (0.0092, 0.0065, 0.0102): the plain sRGB (45, 35, 55) conversion showed about (80, 60, 110) on screen after the fixed exposure and grading (first capture, lavender discs lighter in hue than the floor); after the correction the disc reads about (35, 30, 43) once unblended. MI_BlobShadow keeps no override. Deleting material expressions asserts headless in 5.8, so the task-264 falloff nodes stay unconnected in the graph. Seen in -game frames (Saved/Captures/task472-shadows-b, task472-shadows-barrel): dark purple-grey soft discs under the miner, drone, turrets and robots, in and out of the miner light pool; chest and barrel discs exist but show only a thin rim at SizeScale 1.2.

## Relations
- [[task-472-actor-blob-shadows]] — parent global task.
