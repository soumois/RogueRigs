---
id: task-361-bp-agent-cost
description: "Global task: Cut bp agent cost: batch independent MCP calls, a compact tool sheet so toolset discovery is skipped, describe single widgets rather than trees."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# bp agent cost: batching, MCP tool sheet, single-widget reads

- request: owner, 2026-10-29, in French, verbatim: "Fait de 1 à 3 et est-ce que tu penses qu'un modéle sonnet suffirait pour les captures ? Peut être aussi prévoir un agent pour ça ? Par exemple, on lui donne une liste de chose à vérifier, mais il n'a pas besoin d'avoir tout le contexte. Je suppose qu'actuellement, l'agent BP ou CPP qui vérifie les captures et celui qui a créé ce qu'il doit vérifier". "1 à 3" = the three levers proposed by the main session the same day; this task is lever 3 (bp agent, 48 % of worker cost, avg 32 turns, peak 86k; target about -15 % of bp cost). Context relayed by the main session (cost analysis of 2026-10-29, `node vault/scripts/agent-usage.js --from 280` and subagent transcripts, weighted M units): tasks 332-357 cost ~49 M; workers 27.6 (bp 13.3, cpp capture subtasks 4.24 over 18 runs, 3d 3.6, art 2.4, doc 1.24), executor 6.3, planning ~9.7 (~20 %), main sessions ~5.4. Lever 4 (main session per request) is not requested. Lever 3 as proposed: independent MCP calls sent one per turn (task-356-01: 7 consecutive MoveWidget turns); every run redoes list_toolsets + describe_toolset (~9k tokens, ~8 % of bp cost); GetWidgets / GetWidgetDescription dumps (13-23k tokens each) stay in context to the end. Proposal: batching rule in the bp agent definition, a compact cheat sheet of the toolsets, tools and argument shapes it uses so discovery is skipped, guidance to describe single widgets rather than whole trees when possible. Owner answer 2026-10-29 (relayed by the main session, verbatim): "OK, tout par défaut" = the plan and every default of Q1 to Q12 accepted (owner intent for the rule edits of `_navigation` and `orchestration-protocol` steps 3 and 4 given through Q1, Q2, Q7).
- goal: The bp agent sends independent MCP calls together, reads a compact sheet instead of discovering toolsets, and requests single-widget descriptions when the widget is known.
- acceptance: `Tools/unreal/mcp-sheet.js` writes `Tools/unreal/mcp-tool-sheet.md` (one line per tool the bp agent used in tasks 332-357: toolset, tool, required arguments and types) from the transcripts, no editor needed; bp definition and agent-bp state the batching, sheet-first and single-widget rules; bp-mcp-lessons links the sheet; lint clean; committed. No bp run (none requested).
- gdd_check: crew tooling, no gdd node involved (Planner, 2026-10-29). Read: agent-bp (bp already runs on Sonnet: lever 3 is about units), `.claude/agents/bp.md`, bp-mcp-lessons, unreal-mcp (tool inventory: toolset names only, no argument shapes). Every bp run calls describe_toolset, so the transcripts already hold the argument shapes: the sheet can be generated from them without the editor (Q10). A hand-written sheet in a vault node would exceed the 400-word limit: the generated file lives in Tools/unreal and a short section of bp-mcp-lessons links it (Q11).
- confirmed_by_owner: yes (2026-10-29, "OK, tout par défaut" to Q1-Q12 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-361-02-bp-agent-cost-doc
- started: 2026-10-29T14:27:34Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "OK, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-29.
- Q10 Sheet source: `node Tools/unreal/mcp-sheet.js` reads the bp subagent transcripts (the location agent-usage.js already uses), keeps the tools actually called in tasks 332-357 and their argument shapes from the describe_toolset answers in those transcripts, and writes `Tools/unreal/mcp-tool-sheet.md`; no editor, no MCP call. Rerun it after an engine or plugin update. Default: yes.
- Q11 Placement: the generated sheet stays in `Tools/unreal/`, linked from a short "Tool sheet" section of bp-mcp-lessons and from agent-bp `reads`; bp.md step 4 gains: independent MCP calls in one message, no list_toolsets / describe_toolset for a tool on the sheet, GetWidgetDescription of the named widget rather than GetWidgets of the whole tree when the name is known. Default: yes.

## Subtasks
1. task-361-01-bp-agent-cost-sheet (vault/reference/tasks/task-361-bp-agent-cost/task-361-01-bp-agent-cost-sheet.md) — cpp: Tools/unreal/mcp-sheet.js and the generated mcp-tool-sheet.md from bp transcripts; change summary.
2. task-361-02-bp-agent-cost-doc (vault/reference/tasks/task-361-bp-agent-cost/task-361-02-bp-agent-cost-doc.md) — doc: bp.md step 4 rules, agent-bp reads, bp-mcp-lessons Tool sheet section, unreal-mcp link.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-361-01-bp-agent-cost-sheet]] — step 1, cpp.
- [[task-361-02-bp-agent-cost-doc]] — step 2, doc.
- [[task-358-planner-cost]] — lever 1, same request.
