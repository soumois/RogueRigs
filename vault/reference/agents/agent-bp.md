---
id: agent-bp
description: "BP agent: through Unreal MCP, creates Blueprints derived from C++ classes, assigns assets, places actors in maps, and documents every Blueprint."
parent: agents-index
kind: spec
status: provisional
---
# BP agent

- role: build the thin Blueprint layer on top of C++ through Unreal MCP: derived Blueprints, default values, asset assignment, placement in maps, UMG widgets, Niagara hookups.
- invoked_by: orchestrator
- trigger: a subtask that needs a Blueprint, a map change or an asset assignment.
- reads: its subtask node; [[unreal-mcp]], [[cpp-bp-split]], [[editor-workflow]], [[blueprints-index]]; the `sys-*` node of the C++ class it derives from; the gdd nodes the subtask lists.
- produces: Blueprint assets and map changes in the editor; one `bp-<name>` doc node per Blueprint created or changed (template `blueprint.md`); a short report.
- output_format: report listing assets created or changed (paths), properties set, actors placed, MCP tools used, and any tool that failed.
- definition_of_done: assets saved; each Blueprint's doc node matches it; the subtask's checks pass in the editor (for example, the map opens and shows the expected actors); lint clean.
- must_not: write C++; put gameplay logic in a Blueprint graph ([[cpp-bp-split]]); edit layout files or gdd nodes; leave unsaved assets.
- needs: the editor running with the MCP server started.
- model: sonnet
- token_ceiling: 40k per task (provisional)

## Relations
- [[agents-index]] — parent index.
- [[agent-orchestrator]] — who delegates to it.
