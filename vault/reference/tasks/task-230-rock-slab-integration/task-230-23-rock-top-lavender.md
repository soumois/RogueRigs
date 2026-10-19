---
id: task-230-23-rock-top-lavender
description: "Rock top reads lavender, not near white: top desaturation reduced to about 25 %."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Rock top reads lavender, not near white: top desaturation reduced to about 25 %

- goal: the top keeps the rock lavender of the color-code (owner 2026-10-19, after the 230-19 captures (Saved/RockSlab/after3/): tops read near white since the texture shows); owner accepted the default: desaturation about 25 % instead of the 55 % of task-230-04, tint and brightness adjusted only if still too pale.
- agent: bp
- depends_on: task-230-22-broken-top-texture
- inputs: vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/agents/bp-mcp-lessons.md, vault/reference/gdd/art-audio/color-code.md; captures Saved/RockSlab/after3/
- expected_output: MI_RockSlab TopDesaturate about 0.25 (and TopBrightness if needed), values documented as owner 2026-10-19.
- done_when: saved; doc updated; committed; look checked in task-230-26.
- editor_state: open
- token_ceiling: 20000
- model: sonnet
- tokens: input 42, cache write 33643, cache read 512573, output 571
- runs: 1
- milestone: M2
- result: Commit c25fe7b3. MI_RockSlab TopDesaturate 0.55 -> 0.25 (owner 2026-10-19), TopBrightness 1.12 -> 0.85; estimated mean top albedo (0.28, 0.26, 0.34), about 1.8x SideColor, lavender hue kept. Look checked in 230-26.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
