---
id: task-094-09-hud-root
description: "WBP_HUD contains only the sub-widgets and their placement, with no logic and no tick; code initialises it only."
parent: task-094-hud-refactor
kind: task
status: todo
---
# WBP_HUD as the placement root

- goal: WBP_HUD contains only the sub-widgets and their placement, with no logic and no tick; code initialises it only.
- agent: bp
- depends_on: task-094-08-hud-minimap
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: final pass: WBP_HUD holds only the canvas and the sub-widget instances (health, XP, weapons, cards, alerts and objectives, minimap and locator, subtitles) with their anchors and z-order; delete every leftover function, variable, event and the Tick graph; check URRHUDWidget (C++) initialises only WBP_HUD and calls the panel binds once on the children; reconcile placements so nothing overlaps (weapons row, subtitles, XP bar, alert bar, banners, minimap); update wbp-hud to a short placement note and the hud-refactor audit notes with the final structure; read every anchor back; compile 0 errors; run nothing else. Owner rules (2026-10-11): WBP_HUD stays the single root that contains all sub-widgets and only places them; each sub-widget holds its own logic, binds to C++ events in Construct and unbinds in Destruct, no polling of unchanged values, a Tick only for a genuinely continuous value (stated in the audit); keep the current look and placement unless the audit says otherwise; no screenshot calls (nobody sees the screen: the owner judges); group the work, commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md and Tools/art/ are someone else's); quirks: real newline in pins, set_pin_value does not dirty the asset (touch a widget property and revert, then check git status shows the uasset changed), write_graph_dsl leaves stray nodes (read back and delete them); a new doc node for the widget in tech/blueprints linked from blueprints-index and wbp-hud updated (remove what moved). 40-turn limit: commit after each working group.
- done_when: WBP_HUD has no logic and no tick, sub-widgets placed with anchors read back, compiled 0 errors, saved, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-094-hud-refactor]] — parent global task.
