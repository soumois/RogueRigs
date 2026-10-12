---
id: task-106-01-wear-mask-texture
description: "One shared grayscale wear mask texture exists, tileable, ready to be multiplied on flat colours for edge wear, scratches and light grime."
parent: task-106-wear-mask-texture
kind: task
status: in-progress
---
# Shared wear mask texture

- goal: One shared grayscale wear mask texture exists, tileable, ready to be multiplied on flat colours for edge wear, scratches and light grime.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Blender route, no Meshy, no credit: a script Tools/art/blender/wear_mask.py (bpy + numpy, deterministic fixed seeds, no screenshot) that writes Art/Textures/T_WearMask.png: 1024 x 1024 grayscale, tileable in both directions, white = clean, darker = worn. Content: irregular patches (chips and scuffs) of low-frequency noise, a few fine scratches, light grime; large flat-faceted style: hard-edged shapes, no photographic noise, no soft gradients. Also T_WearMask_preview.png is NOT needed. Write Art/Textures/T_WearMask.report.json (size, seed, tileable check numeric: left/right and top/bottom edge difference under a small threshold, mean grey value, fraction of dark pixels). Then open T_WearMask.png once with the Read tool to describe it. Nothing applied to any asset yet (task-101-05 applies it to the barrel). Commit by name: the script, T_WearMask.png and its report.
- done_when: T_WearMask.png is 1024 x 1024 grayscale, tileable (numeric edge check in the report), hard-edged patchy look (described after opening it), deterministic script; the report exists; nothing applied to an asset; committed by name.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-106-wear-mask-texture]] — parent global task.
