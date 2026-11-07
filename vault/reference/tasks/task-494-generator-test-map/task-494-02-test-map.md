---
id: task-494-02-test-map
description: "Bp: The test map asset using the test mode, starting on a generated layout."
parent: task-494-generator-test-map
kind: task
status: done
---
# Enemy-free test map with a command to generate and reload a new level: bp

- goal: The test map asset using the test mode, starting on a generated layout.
- agent: bp
- depends_on: task-494-01-test-mode-command
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: A1 of the global holds when the map is opened in -game; map saved; its bp doc node exists; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-494
- usage: included in the executor run of task-494
- runs: 1
- milestone: M2
- started: 2026-11-07T08:36:35Z
- waiting_for: none
- result: Done (commit 81025641). BP_GenTestGameMode (parent ARRGenTestGameMode): DefaultPawnClass BP_Miner_C, PlayerControllerClass BP_RRPlayerController_C, HiddenHUDWidgetNames [AlertObjectives]. L_GenTest = duplicate of L_ArenaM1 (lights, PPV, space background, asteroid field, PlayerStart, BP_ArenaGrid same transform), BP_EnemySpawner removed, grid LayoutName emptied, World Settings DefaultGameMode BP_GenTestGameMode_C; saved through MCP (JSON-RPC script, mcp__unreal tools not exposed). A1 in -game (Saved/Captures/task494_run1, task494_seedline3): generated 240 x 240 layout built by the loader (cell counts match), miner and drone at the spawn cell, minimap drawn, rifle firing (shot impact effects), rr.Debug.FuseBarrel lit a barrel fuse with its telegraph; HUD alert bar hidden (log). Not driven by a script: walking, aiming by mouse, digging by hand and a heal pickup (no input or pickup debug command; pacing bot refuses without a spawner): owner check. bp doc node bp-gen-test-game-mode written here (done_when asks for it). Not in MapsToCook (dev map).

## Relations
- [[task-494-generator-test-map]] — parent global task.
