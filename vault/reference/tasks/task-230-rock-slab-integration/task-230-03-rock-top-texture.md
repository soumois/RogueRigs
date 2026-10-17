---
id: task-230-03-rock-top-texture
description: "Tileable painted rock top texture for the world-UV slab material."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Tileable painted rock top texture for the world-UV slab material

- goal: Tileable painted rock top texture for the world-UV slab material.
- agent: art
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md, vault/reference/gdd/art-audio/art-style-guide.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/gdd/art-audio/readability-rules.md, vault/reference/gdd/art-audio/art-prompts-rock.md, vault/reference/tech/pipelines/kie-image-workflow.md, vault/reference/agents/art-3d-lessons.md; look target Art/Source/rock-look/20261017-131704-rock-look-2.png
- expected_output: 2 candidates max, seamless tileable square texture, violet-tinted light grey (color-code), subtle stone tone, no cracks (intact rock is smooth), readable under the near-vertical camera; files in `Art/Source/rock-top-texture/`, chosen file copied to `Art/Textures/T_RockTop.png` only after the owner picks.
- done_when: candidates sent to the owner; tiling check (2 x 2 tile preview) shows no seam; owner pick recorded.
- editor_state: any
- token_ceiling: 50000
- model: sonnet
- tokens: input 26, cache write 48222, cache read 324815, output 1459
- runs: 1
- milestone: M2
- result: 2 tileable candidates (commit 2af6cde3, 12 credits, make_tileable.js half-shift cross-fade, 3x3 previews seamless), prompt node art-prompts-rock-top-texture; owner 2026-10-17 picks proposal 2, copied to Art/Textures/T_RockTop.png; slightly more violet than the reference, to correct in the material

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
