---
id: task-140-01-xp-chip-glow-violet
description: "Give the big XP chip the nut glow level and the lilac-violet colour of the owner sample."
parent: task-140-xp-chip-glow-violet
kind: task
status: done
---
# XP chip: glow level and lilac colour

- goal: Give the big XP chip the nut glow level and the lilac-violet colour of the owner sample.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: M_XPChip_core_bright: GlowAmount 25, Color and GlowColor (0.8, 0.25, 1.0); M_XPChip_core_dark: GlowAmount 12, Color and GlowColor (0.45, 0.12, 0.6); PulseAmount unchanged (0.4); both saved; read back; the owner sample is an 80 x 83 screenshot of a lilac hexagon whose bright part is sRGB (233, 155, 245) and whose darker centre is (112, 41, 124), seen through bloom, so the GlowColor values are a first guess for the owner to judge; color-code (vault/reference/gdd/art-audio/color-code.md) gets one sentence: the big XP chip core glows lilac-violet, GlowColor about (0.8, 0.25, 1.0), provisional, owner 2026-10-13; NS_PickupSparkleBig, NS_XPTrailBig and MI_PickupXPBig stay magenta and are NOT changed (report them as a mismatch to decide). Editor OPEN (the owner is using it; keep it open, save only the assets you name), MCP up, no CaptureEditorImage, Screenshot or CaptureViewport; read every value back; update the doc node bp-pickup-xp; commit by name; the three XP tasks run one after the other, never together.
- done_when: GlowAmount and colour read back on both instances, assets saved, git shows them modified, doc node and color-code updated
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 16, cache write 18657, cache read 97402, output 188
- runs: 1
- milestone: M2
- result: M_XPChip_core_bright GlowAmount 25 colour (0.8, 0.25, 1.0); core_dark 12 and (0.45, 0.12, 0.6); lilac guess from the owner sample; Niagara sparkle and trail stay magenta (to decide).

## Relations
- [[task-140-xp-chip-glow-violet]] — parent global task.
