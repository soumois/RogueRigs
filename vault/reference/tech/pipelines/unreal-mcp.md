---
id: unreal-mcp
description: "Epic's Unreal MCP plugin (UE 5.8, experimental): how to start it, how Claude Code connects, known toolsets, limits, and the tool inventory status."
parent: pipelines-index
kind: doc
---
# Unreal MCP

- Plugins enabled in the project: **Unreal MCP** (Epic, experimental) and **All Toolsets** (aggregator of every toolset plugin).
- The server runs inside the editor process: the editor must be open ([[editor-workflow]]).
- Start: Editor Preferences → Model Context Protocol → Auto Start Server, or the console command `ModelContextProtocol.StartServer`.
- Endpoint: `http://127.0.0.1:8000/mcp` (HTTP with Server-Sent Events; port and path set in Editor Preferences). Loopback only, no authentication.
- Claude Code connects through the project file `.mcp.json` (server name `unreal`).

## Known toolsets
Named in Epic's documentation: `ActorTools`, `SceneTools`, `MaterialInstanceTools`, `ObjectTools`. Exposed functionality includes spawning actors, lighting, material instances, inspecting Slate widgets, running automation tests.

## Tool inventory
- Full tool list: open. The first BP subtask lists the tools the server actually exposes and records them here.
- Whether Blueprint classes can be created and edited through MCP: open, checked by that inventory. If not, the fallback is decided with the owner.

## Limits
- Experimental: features incomplete, APIs and formats may change.
- Each session starts cold: the agent discovers the project one tool call at a time, so subtasks must name the exact assets and classes involved.
- New tools need an editor restart (Live Coding does not propagate them).

Source: https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor

## Relations
- [[pipelines-index]] — parent index.
- [[agent-bp]] — the agent that uses it.
