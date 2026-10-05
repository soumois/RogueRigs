---
id: orchestrator-lessons
description: "What the crew learned from task-013 to task-029: bp agent and Unreal MCP limits, visual checks, measuring, owner preferences. Open before delegating."
parent: agents-index
kind: note
---
# Orchestrator lessons

## Blueprint agent and Unreal MCP
- 40-turn limit: most bp subtasks needed 2 to 4 runs. Ask it to work in groups and commit after each.
- Never let it call CaptureEditorImage, Screenshot or CaptureViewport: the base64 floods its context.
- It cannot create calls from one Blueprint to another Blueprint's functions, nor Enhanced Input assets. Put every hook as a BlueprintImplementableEvent on the actor that shows it.
- MCP cannot set console variables: drive PIE with the property route on `/Script/RogueRigs.Default__RRCheatSettings` ([[sys-cheats]]). Reset persistent values after.
- If the MCP tools are not exposed to a subagent, JSON-RPC over HTTP to the endpoint works. More quirks: [[unreal-mcp]].
- A PIE check that reads a visibility flag is not a render check (the heavy's ring was hidden in game; an integration test caught it).

## Visual checks
- Nobody sees the screen. Ask the cpp agent to run `-game`, take shots with `rr.Debug.Shots`, open the PNGs and describe them. This found the Niagara damage-number defects (task-025-03).

## Measuring
- Close other GPU-heavy programs first: a background game made the 025 runs look twice slower.
- Stress procedures and history: [[stress-test-m1]], [[stress-opt-m1]], [[stress-grid-cells]].

## Owner (Pierre)
- Write to him in French; never correct his spelling.
- Ask all questions at once, each with a proposed default he can accept with "ok".
- He launches whole lots and lets provisional values be set (written in the GDD as `provisional, owner <date>`); report decisions he must confirm.
- He wants a stress-test report as soon as it ends, before the rest.
- He tracks cost: input, cache and output tokens per subtask ([[cost-log]]).

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-runbook]] — the commands.
- [[agent-bp]] — the Blueprint agent these limits apply to.
