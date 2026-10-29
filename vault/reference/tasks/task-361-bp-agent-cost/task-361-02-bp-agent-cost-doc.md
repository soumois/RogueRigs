---
id: task-361-02-bp-agent-cost-doc
description: "Doc: batching, sheet-first and single-widget rules in the bp definition and agent-bp; bp-mcp-lessons and unreal-mcp link the generated tool sheet."
parent: task-361-bp-agent-cost
kind: task
status: done
---
# bp agent cost: doc

- goal: Per global Q11, update `.claude/agents/bp.md` step 4, agent-bp `reads`, bp-mcp-lessons (short "Tool sheet" section) and unreal-mcp (Tool inventory points to the sheet and its regeneration command).
- agent: doc
- depends_on: task-361-01-bp-agent-cost-sheet
- inputs: `.claude/agents/bp.md`, agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), `Tools/unreal/mcp-tool-sheet.md`, _authoring (vault/_authoring.md)
- expected_output: the listed files updated, one commit.
- done_when: bp.md and agent-bp state the three rules and the same sheet path; bp-mcp-lessons and unreal-mcp link it with its regeneration command; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 8, cache write 20366, cache read 61426, output 2833
- usage: turns 4, context start 11k peak 25k, resumes 0, 0 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-10-29T14:30:17Z
- waiting_for: none
- result: bp.md step 4 rules, agent-bp reads and mcp_rules, bp-mcp-lessons Tool sheet section, unreal-mcp link; commit 23915732

## Relations
- [[task-361-bp-agent-cost]] — parent global task.
