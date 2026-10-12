---
id: task-094-07-hud-alerts
description: "The alert bar, icons, horde banner, objective, hack and shuttle texts and the end screen move into their own widget."
parent: task-094-hud-refactor
kind: task
status: in-progress
---
# WBP_AlertObjectives

- goal: The alert bar, icons, horde banner, objective, hack and shuttle texts and the end screen move into their own widget.
- agent: bp
- depends_on: task-094-06-hud-cards
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: create WBP_AlertObjectives with the alert bar and label and event icons, the HORDE INCOMING banner, the objective and hack texts, the shuttle text and the end screen (z 20), driven by OnAlertBarEventsChanged, OnHordeIncoming and the objective, shuttle and run-ended events (no per-tick recomputation of the icon list); same placement and look as today; remove the moved parts and the C++ per-tick RefreshAlertBarEvents dependence from the HUD path if the audit lists it (report if it needs the cpp agent). Owner rules (2026-10-11): WBP_HUD stays the single root that contains all sub-widgets and only places them; each sub-widget holds its own logic, binds to C++ events in Construct and unbinds in Destruct, no polling of unchanged values, a Tick only for a genuinely continuous value (stated in the audit); keep the current look and placement unless the audit says otherwise; no screenshot calls (nobody sees the screen: the owner judges); group the work, commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md and Tools/art/ are someone else's); quirks: real newline in pins, set_pin_value does not dirty the asset (touch a widget property and revert, then check git status shows the uasset changed), write_graph_dsl leaves stray nodes (read back and delete them); a new doc node for the widget in tech/blueprints linked from blueprints-index and wbp-hud updated (remove what moved). 40-turn limit: commit after each working group.
- done_when: compiled 0 errors, saved, WBP_HUD places the widget, moved logic removed, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-094-hud-refactor]] — parent global task.
