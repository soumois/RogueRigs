---
id: unreal-mcp-epic-plugin
description: "What Rogue Rigs takes from Epic's Unreal MCP plugin for Claude Code: safety rules, Live Coding compile, proxy, custom toolsets. Open before a bp subtask."
parent: unreal-mcp
kind: doc
---
# Epic's Unreal MCP plugin for Claude Code

Source: `EpicGames/unreal-engine-skills-for-claude-code-plugin` (v3.1.1, MIT). Not installed (owner 2026-10-11): its skill triggers on every agent that names a `BP_` or `SM_` asset, and it gives in-editor Agent Skills (stored as `.uasset`) precedence over everything, a second source of truth beside this vault.

## Rules adopted
- Every MCP result is read before the next call; anything that is not an explicit success stops the subtask.
- Mutations on the same asset or editor state run one after the other; parallel calls only for independent reads.
- Save and commit before any bulk change, and save again after it.

## Tools worth using
- `LiveCodingToolset.CompileLiveCoding`: compiles from the open editor, blocks until the compile ends and returns MSVC diagnostics.
- Proxy `unreal_mcp_proxy` (engine `Plugins/Experimental/ModelContextProtocol/Extras/Proxy`): keeps the Claude Code connection open while the editor restarts. Installed with `unreal_mcp_proxy.exe --mcp-json "<path to .mcp.json>" install`.

## Custom toolsets
The plugin's `create-toolset` skill describes how to add AI-callable Python toolsets to the editor. It is the route to fill the gaps listed in [[unreal-mcp]] and [[unreal-mcp-quirks]] (calling a UFunction, console variables, level creation, Enhanced Input assets, re-import over an asset, redirector fixup, fixed FBX import settings). Copy the skill into `.claude/skills/` only when a task builds such a toolset.

## Relations
- [[unreal-mcp]] — parent: setup, inventory and limits.
- [[unreal-mcp-quirks]] — the workarounds a custom toolset would replace.
