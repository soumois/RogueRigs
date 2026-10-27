---
id: agent-bp
description: "BP agent: through Unreal MCP, builds the editor layer on C++ (Blueprints, widgets, materials, Niagara, asset assignment, map placement) and returns a change summary."
parent: agents-index
kind: spec
status: provisional
---
# BP agent

- role: build the thin editor layer on top of C++ through Unreal MCP: derived Blueprints, default values, asset assignment and import, placement in maps, UMG widgets, materials, Niagara systems.
- invoked_by: orchestrator
- trigger: a subtask that needs the editor: a Blueprint, widget, material, Niagara system, map change, asset import or assignment.
- reads: its subtask node; [[bp-mcp-lessons]], [[unreal-mcp]], [[unreal-mcp-quirks]], [[cpp-bp-split]], [[editor-workflow]], [[blueprints-index]]; the `sys-*` node of the C++ class it derives from; the gdd nodes the subtask lists.
- produces: Blueprint assets and map changes in the editor, saved and committed by explicit path; a change summary. It does not write `bp-*` doc nodes: the doc subtask does, from that summary ([[agent-doc]]); the Blueprint doc rule of [[_authoring]] is met when it closes. Exception: a subtask node planned before task-325 (tasks 308, 309, 314) that names a doc node in its own output is followed as written.
- output_format: change summary: assets created or changed (paths), commit hash, properties set, actors placed, decided values used, doc nodes to create or update, MCP tools used, and any tool that failed.
- definition_of_done: assets saved and committed; the subtask's checks pass in the editor (for example, the map opens and shows the expected actors); change summary returned.
- must_not: write C++; put gameplay logic in a Blueprint graph ([[cpp-bp-split]]); edit layout files or gdd nodes; leave unsaved assets; call a viewport or editor capture tool.
- needs: the editor running with the MCP server started.
- known_limits: [[bp-mcp-lessons]] (turn limit, captures, PIE, cross-Blueprint calls).
- definition_file: `.claude/agents/bp.md` (80 turns since 2026-10-14; 40 before).
- model: sonnet
- token_ceiling: 1.0 M units (provisional, owner 2026-10-27)

## Relations
- [[agents-index]] — parent index.
- [[agent-executor]] — who delegates to it.
- [[bp-mcp-lessons]] — its known limits and working rules.
- [[agent-doc]] — writes the `bp-*` nodes from its change summary.
