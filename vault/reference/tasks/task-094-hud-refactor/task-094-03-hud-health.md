---
id: task-094-03-hud-health
description: "Health bar and text, game over text and damage vignette move into their own widget bound to the health events."
parent: task-094-hud-refactor
kind: task
status: done
---
# WBP_HealthStatus

- goal: Health bar and text, game over text and damage vignette move into their own widget bound to the health events.
- agent: bp
- depends_on: task-094-02-hud-events
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: create WBP_HealthStatus holding the health bar, health text, GAME OVER text and the damage vignette (with the vignette hold/decay logic) and their logic, driven by ARRMiner OnHealthChanged / OnDamaged and the game-over event (the vignette decay may keep a tick only while it fades, as the audit states); WBP_HUD places it with the same position and z-order as today; remove the moved elements and logic from WBP_HUD and its tick. Owner rules (2026-10-11): WBP_HUD stays the single root that contains all sub-widgets and only places them; each sub-widget holds its own logic, binds to C++ events in Construct and unbinds in Destruct, no polling of unchanged values, a Tick only for a genuinely continuous value (stated in the audit); keep the current look and placement unless the audit says otherwise; no screenshot calls (nobody sees the screen: the owner judges); group the work, commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md and Tools/art/ are someone else's); quirks: real newline in pins, set_pin_value does not dirty the asset (touch a widget property and revert, then check git status shows the uasset changed), write_graph_dsl leaves stray nodes (read back and delete them); a new doc node for the widget in tech/blueprints linked from blueprints-index and wbp-hud updated (remove what moved). 40-turn limit: commit after each working group.
- done_when: compiled 0 errors, saved, WBP_HUD places the widget, moved logic removed, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: WBP_HealthStatus (parent URRHUDPanelWidget) holds health bar/text, GAME OVER and the vignette with a self-stopping fade timer; WBP_HUD places it full-screen at child 0 and lost the moved logic; compiled and saved, not seen on screen. Side effect: C++ KeepVignetteWarm and the vignette debug commands no longer find the vignette (task-094-10). Commits c179c2f, 5c2aeb6, da160c2.

## Relations
- [[task-094-hud-refactor]] — parent global task.
