---
id: task-230-03-rock-top-texture
description: "Tileable painted rock top texture for the world-UV slab material."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
