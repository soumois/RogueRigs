---
id: agent-bp
description: "BP agent: through Unreal MCP, builds the editor layer on C++ (Blueprints, widgets, materials, Niagara, asset assignment, map placement) and documents every Blueprint."
parent: agents-index
kind: spec
status: provisional
---
# BP agent

- role: build the thin editor layer on top of C++ through Unreal MCP: derived Blueprints, default values, asset assignment and import, placement in maps, UMG widgets, materials, Niagara systems.
- invoked_by: orchestrator
- trigger: a subtask that needs the editor: a Blueprint, widget, material, Niagara system, map change, asset import or assignment.
- reads: its subtask node; [[bp-mcp-lessons]], [[unreal-mcp]], [[unreal-mcp-quirks]], [[cpp-bp-split]], [[editor-workflow]], [[blueprints-index]]; the `sys-*` node of the C++ class it derives from; the gdd nodes the subtask lists.
- produces: Blueprint assets and map changes in the editor; one `bp-<name>` doc node per Blueprint created or changed (template `blueprint.md`); a short report.
- output_format: report listing assets created or changed (paths), properties set, actors placed, MCP tools used, and any tool that failed.
- definition_of_done: assets saved; each Blueprint's doc node matches it; the subtask's checks pass in the editor (for example, the map opens and shows the expected actors); lint clean; committed by explicit path.
- must_not: write C++; put gameplay logic in a Blueprint graph ([[cpp-bp-split]]); edit layout files or gdd nodes; leave unsaved assets; call a viewport or editor capture tool.
- needs: the editor running with the MCP server started.
- known_limits: [[bp-mcp-lessons]] (turn limit, captures, PIE, cross-Blueprint calls).
- definition_file: `.claude/agents/bp.md` (80 turns since 2026-10-14; 40 before).
- model: sonnet
- token_ceiling: 40k per task (provisional)

## Relations
- [[agents-index]] — parent index.
- [[agent-executor]] — who delegates to it.
- [[bp-mcp-lessons]] — its known limits and working rules.
