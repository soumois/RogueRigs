---
id: task-361-01-bp-agent-cost-sheet
description: "Cpp: Tools/unreal/mcp-sheet.js builds a compact MCP tool sheet from bp agent transcripts of tasks 332-357; run it once and commit the sheet."
parent: task-361-bp-agent-cost
kind: task
status: done
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
- model: opus
- tokens: input 40, cache write 67125, cache read 917423, output 12629
- usage: turns 20, context start 12k peak 67k, resumes 0, 3 min, units 0.24 M
- runs: 1
- milestone: M2
- started: 2026-10-29T14:27:34Z
- waiting_for: none
- result: mcp-sheet.js + mcp-tool-sheet.md, 43 tools, ~1.1k tokens, no missing shapes; 14 bp runs (342-357, none on disk for 332-341); commit 8fe46492

## Relations
- [[task-361-bp-agent-cost]] — parent global task.
