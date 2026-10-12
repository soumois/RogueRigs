---
id: task-103-02-hud-events-only
description: "WBP_MinimapLocator reacts to the new minimap and locator events; its always-on timer and texture polling are removed."
parent: task-103-hud-events-only
kind: task
status: in-progress
---
# Minimap and locator widget on events

- goal: the minimap and locator panel redraws only when an event says something changed; the 0.016 s timer and the texture polling are gone.
- agent: bp
- depends_on: task-103-01-hud-events-only
- inputs: wbp-minimap-locator (vault/reference/tech/blueprints/wbp-minimap-locator.md), hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), sys-hud-events (vault/reference/tech/systems/sys-hud-events.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: in WBP_MinimapLocator, bind the new C++ delegates from task-103-01 (OnMinimapTextureReady: build the dynamic material once, no polling; OnMinimapViewChanged: redraw the minimap view and the markers; OnLocatorTargetChanged plus the movement-change signal: update the arrow and the distance) and remove the always-running 0.016 s timer and the GetMinimapTexture polling loop; the arrow may keep a per-frame update only if the C++ audit of task-103-01 says it must (state it); idle miner = nothing runs. Same look and placement. Read the new delegate names and signatures from sys-hud-events and sys-hud-minimap. Rules: compile 0 errors, save (dirty-asset trick, check git status shows the uasset changed), update wbp-minimap-locator and wbp-hud notes, no screenshot calls, MCP via JSON-RPC to http://127.0.0.1:8000/mcp with small outputs (truncate in the script), a few large write_graph_dsl batches, stray events deleted by connection count; commit after the change by name (never git add -A; untracked .claude/agents/3d.md, art.md, Tools/art/ and Art/ belong to someone else). 40-turn limit.
- done_when: compiled 0 errors, saved, no timer or polling left in the widget except a justified continuous update, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-103-hud-events-only]] — parent global task.
