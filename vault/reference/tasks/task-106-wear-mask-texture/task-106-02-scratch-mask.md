---
id: task-106-02-scratch-mask
description: "The shared mask becomes a scratch-only mask: a few thin hard-edged scratch lines, no blobs (owner 2026-10-11)."
parent: task-106-wear-mask-texture
kind: task
status: done
---
# The shared mask becomes a scratch-only mask

- goal: The shared mask becomes a scratch-only mask: a few thin hard-edged scratch lines, no blobs (owner 2026-10-11).
- agent: 3d
- depends_on: task-106-01-wear-mask-texture
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md); files Tools/art/blender/wear_mask.py, Art/Textures/T_WearMask.png, Art/Textures/T_WearMask.report.json
- expected_output: Rewrite Tools/art/blender/wear_mask.py (same file names, replaces the previous PNG; the old blob version stays in git history): 1024 x 1024 grayscale, tileable, deterministic fixed seed; white = clean; ONLY thin hard-edged scratch lines (1 to 3 px wide, straight or slightly bent, 5 to 30 % of the width long, mostly at shallow angles, a few small clusters like blows on a lid), about 25 to 40 lines, dark grey (about 0.35 to 0.5); NO blobs, NO grime, NO scuffs, NO soft gradients; fraction of dark pixels about 1 to 2 %. Update T_WearMask.report.json (size, seed, number of lines, tileable numbers, dark fraction); open the PNG once with Read and describe it. Commit by name.
- done_when: T_WearMask.png is scratch lines only on white, tileable (numeric check), dark fraction 1 to 2 %, deterministic, report updated, committed by name.
- editor_state: any
- token_ceiling: 30000
- model: sonnet
- tokens: input 14, cache write 17989, cache read 100704, output 792
- runs: 1
- milestone: M2
- result: Mask rewritten as scratch lines only: 37 lines (26 isolated, 3 clusters), 1.44 % dark, tileable (edge diffs 0.0018 and 0.0047), seed 108; commits 62db100, 81e0b80. Shared wear mask delivered; applied to the barrel in task-101-06.

## Relations
- [[task-106-wear-mask-texture]] — parent global task.
