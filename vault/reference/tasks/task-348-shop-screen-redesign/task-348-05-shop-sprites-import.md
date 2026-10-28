---
id: task-348-05-shop-sprites-import
description: "Bp: import the ten shop screen sprites from Art/UI/Shop as UI textures in /Game/RogueRigs/UI/Shop."
parent: task-348-shop-screen-redesign
kind: task
status: in-progress
---
# Shop screen redesign: import the sprites

- goal: Import the retained PNGs of task-348-01 to 03 (`Art/UI/Shop/T_Shop_*.png`) into `/Game/RogueRigs/UI/Shop/` as UI textures (texture group UI, no mipmaps, sRGB, compression for UI with alpha), names kept.
- agent: bp
- depends_on: task-348-03-shop-card-sprites
- inputs: bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), the "Sprites" section of art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md)
- expected_output: ten texture assets saved; change summary (paths, settings, sizes); one commit.
- done_when: every texture exists, settings read back, sizes match the PNGs; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T16:42:16Z
- waiting_for: none
- result: open

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
