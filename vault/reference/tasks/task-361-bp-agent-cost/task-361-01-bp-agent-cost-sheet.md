---
id: task-361-01-bp-agent-cost-sheet
description: "Cpp: Tools/unreal/mcp-sheet.js builds a compact MCP tool sheet from bp agent transcripts of tasks 332-357; run it once and commit the sheet."
parent: task-361-bp-agent-cost
kind: task
status: todo
---
# bp agent cost: MCP tool sheet

- goal: Write `Tools/unreal/mcp-sheet.js` per global Q10, run it once, commit the script and `Tools/unreal/mcp-tool-sheet.md`. Read `vault/scripts/agent-usage.js` for the transcript location and the description-prefix attribution. No editor, no MCP call, no doc node edit.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), global task-361-bp-agent-cost (vault/reference/tasks/task-361-bp-agent-cost.md), `vault/scripts/agent-usage.js`, unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md) (tool inventory and call shape)
- expected_output: script and sheet committed by path; change summary: usage line, number of tools listed, sheet size in tokens (estimate), tools whose shape was not found.
- done_when: the sheet lists the UMG, object, asset and blueprint tools called in tasks 332-357 (including MoveWidget, GetWidgets, GetWidgetDescription) with required arguments; it is under ~4k tokens; committed.
- editor_state: any
- token_ceiling: 0.8 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-361-bp-agent-cost]] — parent global task.
