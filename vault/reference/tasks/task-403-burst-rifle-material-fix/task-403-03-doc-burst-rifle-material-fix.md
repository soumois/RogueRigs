---
id: task-403-03-doc-burst-rifle-material-fix
description: "Doc: update bp-burst-rifle and burst-rifle-mesh with the cause and the fix"
parent: task-403-burst-rifle-material-fix
kind: task
status: done
---
# Burst rifle material fix: colours readable in game: doc

- goal: The doc nodes state the material cause, the fix and the capture outcome; the open colour observation is resolved.
- agent: doc
- depends_on: task-403-01-burst-rifle-material-diagnose-fix, task-403-02-burst-rifle-colours-capture
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md) (only if the bp change summary reports an editor or MCP quirk), Global request, gdd_check and Questions: `vault/reference/tasks/task-403-burst-rifle-material-fix.md`
- expected_output: Updated doc nodes, lint clean, committed by path.
- done_when: bp-burst-rifle and burst-rifle-mesh state the cause, the changed assets and settings, the compile result and the capture outcome; the "renders flat cream-white" open observation is replaced; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 26116, cache read 89674, output 3466
- usage: turns 5, context start 12k peak 30k, resumes 0, 0 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-10-31T04:36:53Z
- waiting_for: none
- result: Doc nodes updated (bp-burst-rifle, burst-rifle-mesh, unreal-mcp-quirks), commit 71a658bb; cause, fix, compile and capture outcome recorded.

## Relations
- [[task-403-burst-rifle-material-fix]] — parent global task.
