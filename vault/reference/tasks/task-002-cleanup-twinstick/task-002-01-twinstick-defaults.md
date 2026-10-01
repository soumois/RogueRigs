---
id: task-002-01-twinstick-defaults
description: "Switch L_ArenaM1 and the project defaults to BP_TwinStickGameMode and make L_ArenaM1 the editor and game startup map."
parent: task-002-cleanup-twinstick
kind: task
status: in-progress
---
# TwinStick game mode and default maps

- goal: nothing in the project's defaults or in `L_ArenaM1` points to TopDown or Strategy assets any more.
- agent: bp
- depends_on: none
- inputs: [[unreal-mcp]], [[bp-arena-grid]], `Config/DefaultEngine.ini` (read only, to check the result).
- expected_output: `L_ArenaM1` World Settings game mode override = `/Game/Variant_TwinStick/Blueprints/BP_TwinStickGameMode`. Project Settings (Maps & Modes): Editor Startup Map and Game Default Map = `/Game/RogueRigs/Maps/L_ArenaM1`; Default GameMode = `BP_TwinStickGameMode`. Done through the editor (MCP), so `Config/DefaultEngine.ini` is written by the editor. Map saved. Report every other asset outside `Content/TopDown`, `Content/Variant_Strategy` and `Content/Cursor` that references them (reference viewer or asset tools), without changing it.
- done_when: `Config/DefaultEngine.ini` holds `GameDefaultMap`, `EditorStartupMap` = `L_ArenaM1` and `GlobalDefaultGameMode` = `BP_TwinStickGameMode_C`; the map's override reads back as `BP_TwinStickGameMode`; map saved; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-002-cleanup-twinstick]] — parent global task.
