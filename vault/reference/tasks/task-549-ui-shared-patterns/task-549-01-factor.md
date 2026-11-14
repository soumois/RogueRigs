---
id: task-549-01-factor
description: "Cpp: Inventory and factor the repeated UI patterns and build scripts, screens unchanged"
parent: task-549-ui-shared-patterns
kind: task
status: done
---
# UI: generalise the patterns and scripts used again and again: cpp

- goal: Inventory and factor the repeated UI patterns and build scripts, screens unchanged
- agent: cpp
- depends_on: none
- inputs: sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), pause-menu (vault/reference/gdd/run/pause-menu.md), intermission (vault/reference/gdd/run/intermission.md), run-shop (vault/reference/gdd/run/run-shop.md), death-screen (vault/reference/gdd/run/death-screen.md), main-menu (vault/reference/gdd/run/main-menu.md)
- expected_output: see done_when
- done_when: A2, A4 and A5 hold; inventory drafted for A1.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-549
- usage: included in the executor run of task-549
- runs: 1
- milestone: M2
- started: 2026-11-14T04:37:35Z
- waiting_for: none
- result: 685bc5ea. Inventory (tool: node Tools/unreal/mcp-client.js brushes, every textured brush of 14 WBPs grouped by texture) and scratchpad scripts of tasks 540-548. Factored: (1) Tools/unreal/mcp-client.js, the JSON-RPC MCP client each executor rewrote (541, 545, 546, 547, 548) as a library + CLI (list, call, batch, brushes, tree) with the UMG helpers (ref, widget, get/setProps, slotOf, brush, point/stretch layouts, addWidget, addImage, compile, save) and UI presets of the 3+ screen patterns: frame brush T_LevelEnd_Frame (6 brushes, 5 screens, identical), plate button style T_Shop_ButtonContinue (8 screens; sizes 640x163, 416x106, 302x77), title bar T_Shop_TitleBar (5 screens; 210x29, 356x48, pause Box 276x38), bolted bar classes; (2) Tools/capture/ui-screens.js, the screen recipes rebuilt by hand in 540, 545, 546, 547, 548: mainmenu, pause, options, stats, shop, death, runwon at one resolution, --compare gives side-by-side sheets and PSNR; (3) rr.Debug.PressKey moved beside rr.Debug.Cursor/CursorOn/ButtonHover in UI/RRUIDebugCommands.cpp (renamed from RRButtonHoverDebugCommands.cpp; RRScreenEntranceDebugCommands.cpp removed), now compiled out of shipping like them. No widget, asset or gameplay code changed (A5). Widgets kept as they are, with reasons, in the doc subtask. Build: Succeeded, 0 errors, no new warnings; test scope build only (debug command and tools).

## Relations
- [[task-549-ui-shared-patterns]] — parent global task.
