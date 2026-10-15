---
id: task-183-04-ore-counter-widget
description: "BP: import the ore icon, new WBP_OreCounter panel next to the radar bound to the C++ ore event, secured ore on the success screen, docs."
parent: task-183-ore-counter
kind: task
status: todo
---
# Ore counter widget

- goal: import the validated ore icon as a UI texture; create `WBP_OreCounter` (parent `URRHUDPanelWidget`), icon plus carried count, bound to the C++ ore event from task-183-01 (no tick, no polling); place it in `WBP_HUD` at the top, right of the minimap disc, as an inline resource counter (icon then number, no plate), like a top-left gold counter beside a round minimap, but larger than that reference (owner 2026-10-14: "ça doit plus être comme l'or en haut à gauche, mais tu peux le faire plus grand"); the validated render (`vault/assets/ore-hud-render.png`) gives the icon, not the placement; the success screen in `WBP_AlertObjectives` shows the secured ore; doc node `wbp-ore-counter` created, `wbp-hud` and `wbp-alert-objectives` updated.
- agent: bp
- depends_on: task-183-01-ore-counter-logic, task-183-03-ore-hud-icon
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-minimap-locator (vault/reference/tech/blueprints/ui/wbp-minimap-locator.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md); the validated icon and render in `vault/assets/` (names given at delegation).
- expected_output: texture asset, `WBP_OreCounter`, `WBP_HUD` and `WBP_AlertObjectives` changes, saved by explicit path; doc nodes.
- done_when: widgets compile with warnings as errors; placement read back (no overlap with the minimap or health bar); in PIE `rr.Cheat.GrantOre 10` then extraction shows 10 next to the radar and 10 secured on the success screen (screenshot through the cpp agent if the editor cannot capture); docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-183-ore-counter]] — parent global task.
