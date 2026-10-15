---
id: task-184-01-minimap-opaque
description: "Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included."
parent: task-184-minimap-opaque
kind: task
status: todo
---
# Minimap: fully opaque

- goal: Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included.
- agent: bp
- depends_on: none
- inputs: wbp-minimap-locator (vault/reference/tech/blueprints/ui/wbp-minimap-locator.md), sys-hud-minimap (vault/reference/tech/systems/ui/sys-hud-minimap.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: The disc of WBP_MinimapLocator is fully opaque: M_Minimap outputs opacity 1 inside the circle (circle mask kept, outside stays transparent) whatever the texel alpha, and unseen cells are drawn opaque with an opaque colour (MinimapConfig.UnseenColor on the HUD widget Blueprint, alpha 255; colour per the owner, default black 0,0,0) instead of alpha 0; any widget or image opacity / tint alpha below 1 on MinimapImage raised to 1; wbp-minimap-locator and sys-hud-minimap docs updated (UnseenColor value)
- done_when: M_Minimap compiles and its opacity path reads back 1 inside the circle; UnseenColor and the MinimapImage opacity read back; the HUD Blueprints compile and are saved by explicit path; docs updated; committed by path; the editor stays open; not seen on screen unless the owner checks
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-184-minimap-opaque]] — parent global task.
