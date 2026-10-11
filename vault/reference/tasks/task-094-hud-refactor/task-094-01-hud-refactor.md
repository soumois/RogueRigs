---
id: task-094-01-hud-refactor
description: "Audit of WBP_HUD: per feature, its widgets, tick reads, driving events, placement, and the proposed sub-widget list."
parent: task-094-hud-refactor
kind: task
status: in-progress
---
# HUD refactor: audit

- goal: The HUD is split into one widget per logic, all placed by WBP_HUD, which only lays them out; widgets react to events, tick only for continuous values.
- agent: bp
- depends_on: none
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), sys-subtitles (vault/reference/tech/systems/sys-subtitles.md), sys-weapons (vault/reference/tech/systems/sys-weapons.md), sys-level-up (vault/reference/tech/systems/sys-level-up.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: AUDIT ONLY, no change to any asset (step 1 of the HUD refactor, owner rules 2026-10-11): (a) WBP_HUD is one big widget holding every HUD element and all the logic, and it checks everything every tick; target: one widget per logic (the existing WBP_Subtitles is the model), WBP_HUD only the root that contains all the sub-widgets and places them visually (so code initialises one widget), each sub-widget holds its own logic and connects to gameplay EVENTS (when firing, when XP is gained, when damage is taken, level up, ammo changed, weapon changed, alert raised...), and only genuinely continuous values stay in a tick (for example the reload progress, which changes every frame; the locator arrow; the minimap if it must). Produce one inventory note in vault/reference/tech/blueprints (id hud-refactor-audit, kind note/doc, linked from blueprints-index; split in two nodes if it exceeds the size limit) built by reading the saved WBP_HUD asset through the editor MCP (editor is open; use JSON-RPC over HTTP to http://127.0.0.1:8000/mcp from a Node script, see unreal-mcp-quirks; read-only calls: list graphs, read each graph DSL, list widgets and their slots) and the C++ URRHUDWidget (Source/RogueRigs/UI/RRHUDWidget.h/.cpp, events and Bind functions). For every HUD feature (health bar and text, game over, damage vignette, XP bar and level, weapon slots / ammo bar / ammo blocks / reload bar / turret charges / locked slots, level-up cards (name, value, description, flavor), alert bar and icons, horde banner, shuttle text, objective and hack texts, minimap and its markers, locator arrow, any other): list (1) its widgets, (2) which function or event updates it, (3) whether it is driven from the Tick graph and what it reads each tick (which C++ values, which getters), (4) the gameplay event that could drive it instead (existing C++ delegate or event if there is one: name it; otherwise "missing: <proposed event name and payload>"), (5) whether it genuinely needs a per-frame update and why, (6) its current placement (anchor, offset, size) so the split keeps the look, and (7) which sub-widget it should belong to. Propose the list of sub-widgets (target set: health and game over with vignette, XP and level, weapons, level-up cards, alerts and objectives, minimap and locator, subtitles already done) and the list of missing C++ events with their payloads (this list drives the next cpp subtask 094-02). Measure what the Tick graph costs if cheap (stat or Insights not required: count nodes and per-tick calls is enough). Owner wish noted: the weapon info row (slot names, ammo, LOCKED) should be centred on the screen bottom as a whole (not left): note its current placement and propose the centred placement in the weapons sub-widget. No screenshot calls. No asset change, no C++ change.
- done_when: the audit note exists with the table per feature, the proposed sub-widget list and the missing-events list with payloads; no asset changed (git status shows no uasset change); lint clean; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-094-hud-refactor]] — parent global task.
