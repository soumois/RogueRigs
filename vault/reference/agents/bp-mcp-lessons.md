---
id: bp-mcp-lessons
description: "bp agent limits and Unreal MCP working rules: turns, captures, PIE, cross-Blueprint calls, saving. Read by bp and before delegating to it."
parent: agents-index
kind: note
---
# bp agent and Unreal MCP lessons

Learned on tasks 014 to 182. MCP tool facts (DSL, widgets, Niagara, import): [[unreal-mcp-quirks]].

## Turns and commits
- About 45% of bp subtasks needed 2 to 6 runs, most because of the turn limit. Work in groups, save and commit after each group, and always end with a report, even partial.
- Resumed with `SendMessage`, the agent checks `git log` and `git status` first: unsaved work may live only in the open editor.
- Commit by explicit path (`git commit -- <paths>`), never `git add vault`: another session may have staged files.

## Context
- Never call `CaptureEditorImage`, `Screenshot` or `CaptureViewport`: the base64 floods the context, and the permission classifier denies captures.
- Nobody sees the screen: report values read back, and say plainly what is unverified. A PIE check that reads a visibility flag is not a render check (the heavy's ring was hidden in game).

## Editor state
- The owner may close the editor or run PIE while the agent works: check the editor process and `IsPIERunning` before starting, and before every save. Asset tools (save, exists) fail while PIE runs: stop PIE first.
- Up to 3 bp agents can work in parallel on one editor, on different assets.

## Design limits
- MCP cannot create a call from one Blueprint to another Blueprint's function, nor Enhanced Input assets: put each hook as a BlueprintImplementableEvent on the actor that shows it.
- MCP cannot set console variables: drive PIE through `/Script/RogueRigs.Default__RRCheatSettings` ([[sys-cheats]]); reset persistent values after.

## PIE probes
- PIE can stop ticking after long idle gaps between MCP calls: restart PIE.
- Batch probes in one Node JSON-RPC script (4 connections at most); short events are missed by slow polls. If the MCP tools are not exposed to the agent, JSON-RPC over HTTP to `http://127.0.0.1:8000/mcp` works.

## Relations
- [[agents-index]] — parent index.
- [[agent-bp]] — the agent these limits apply to.
- [[unreal-mcp-quirks]] — tool-level facts and workarounds.
- [[orchestrator-lessons]] — owner rules and planning lessons.
