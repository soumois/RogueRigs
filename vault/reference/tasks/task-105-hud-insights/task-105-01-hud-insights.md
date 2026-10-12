---
id: task-105-01-hud-insights
description: "Unreal Insights measures the HUD cost per frame after the refactor and compares it with the task-068-01 trace."
parent: task-105-hud-insights
kind: task
status: done
---
# HUD cost with Insights after the refactor

- goal: Unreal Insights measures the HUD cost per frame after the refactor and compares it with the task-068-01 trace.
- agent: cpp
- depends_on: none
- inputs: perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-hud-events (vault/reference/tech/systems/sys-hud-events.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: measure only, no gameplay or HUD change. Same scenario and method as task-068-01 (read perf-trace-real-game and the task-068-01 / task-071-01 / task-092-01 / task-093-01 sections of stress-grid-cells for the exact commands, trace channels, the export workaround for paths with spaces, and the baseline numbers): `rr.Stress.Run 250 30 60 1 game` with `rr.Pacing.RunLookahead 0` in editor -game, owner normal PC profile (close nothing, kill nothing, no UnrealEditor.exe running: the owner authorised closing the Orchestrator's editor; verify). One Insights-traced run plus two untraced runs. Report, from the trace: the game-thread cost per frame of the HUD (Slate game-thread side, widget Tick/event cost, the Blueprint tick that used to cost about 0.40 ms and the whole Slate 0.86 ms of task-068-01, the viewport draw game-thread side 0.97 ms, the tickable objects 0.65 ms), the cost of each new panel (WBP_HealthStatus, WBP_XPStatus, WBP_Weapons, WBP_UpgradeChoice, WBP_AlertObjectives, WBP_MinimapLocator, WBP_Subtitles) if the trace shows them by name (Blueprint function scopes), the number of widget Blueprint function calls per frame before/after if it can be counted, and the game-thread avg/p99 of the untraced runs against the latest numbers (task-093-01: 11.6-12.1 ms avg, 17.4 ms p99 at 250 real-game robots). State plainly what is comparable and what is not (the PC load differs from run to run by about 2 ms). Write a dated section in stress-grid-cells and update perf-trace-real-game with the after-refactor ranking of the top game-thread costs; do not commit traces or logs.
- done_when: one traced and two untraced runs done; HUD cost before/after table; top costs ranking updated; results written; lint clean; committed.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 48, cache write 74790, cache read 1096526, output 17544
- runs: 1
- milestone: M2
- result: HUD Blueprint 0.40 -> 0.11 ms per frame (all panel events; WBP_HUD tick scope 0.005); Slate 0.86 -> 0.69; per-panel costs read by name; untraced 12.35-13.72 ms avg, p99 20.3-20.7 (PC load; not attributable to the HUD). Commit 2843106.

## Relations
- [[task-105-hud-insights]] — parent global task.
