---
id: task-061-02-bp-minimap
description: "WBP_HUD round 200 px minimap at the top left from the task-061-01 texture and markers."
parent: task-061-minimap
kind: task
status: in-progress
---
# Minimap widget

- goal: the minimap is visible in the HUD.
- agent: bp
- depends_on: task-061-01-minimap
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-hud (vault/reference/tech/systems/sys-hud.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md)
- expected_output: WBP_HUD: a 200 px round minimap at the top left (circular mask material on the task-061-01 texture, centred on the miner with the view radius from the getter), markers for the miner, console (teal), bonus chest (gold), shuttle (white) placed from the getter, no enemies; nothing else in the HUD overlaps it (move the elements that would); docs wbp-hud; PIE: no log error, widget and texture bound. From task-061-01 (URRHUDWidget, HUD|Minimap): GetMinimapTexture(), event OnMinimapTextureReady(Texture) to set the material texture once, GetMinimapState(FRRMinimapState) with MinerUV, ViewRadiusUV, MinerPosition, ViewRadiusCells and console, chest, shuttle positions with their bHas flags; static GetMinimapOffset(Position, Center, Radius, bClampToCircle, bOutOfRange); material sample UV = MinerUV + (WidgetUV - 0.5) × 2 × ViewRadiusUV, circular mask; marker = widget centre + offset × 100 px; nearest filtering.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-061-minimap]] — parent global task.
