---
id: task-423-01-debug-aim-command
description: "Cpp: add the rr.Debug.Aim <YawDegrees> console command that fixes the miner aim for visual checks"
parent: task-423-miner-aim-laser-range-fade
kind: task
status: done
---
# Miner aim laser: visible maximum range and fade-out at its end: cpp

- goal: Add a debug console command `rr.Debug.Aim <YawDegrees>` (with the other visual-check commands in Source/RogueRigs/Debug/RRVisualCheckCommands.cpp) that fixes the player miner aim direction to that world yaw until `rr.Debug.Aim off` (or an empty argument), overriding mouse and stick aim; no gameplay change when unused, no laser change in this subtask (the before frames must show the current laser). Change summary for the doc subtask: command, how the override is held, files.
- agent: cpp
- depends_on: none
- inputs: task-423-miner-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: The console command, C++ builds, a commit `task-423-01-debug-aim-command: ...`, a change summary.
- done_when: C++ builds; `node Tools/capture/commands.js` lists rr.Debug.Aim with its help text; without the command the aim behaves as before; existing tests pass (the known failures of sys-aim-indicator `unverified` excepted), no new test; committed by path.
- editor_state: Unreal editor closed (C++ build)
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 34, cache write 75647, cache read 893212, output 10062
- usage: turns 17, context start 12k peak 76k, resumes 0, 12 min, units 0.23 M
- runs: 1
- milestone: M3
- started: 2026-11-03T02:02:14Z
- waiting_for: none
- result: rr.Debug.Aim <YawDegrees|off> added (ARRMiner debug aim override, commit b12776b6); build ok, 276 tests pass

## Relations
- [[task-423-miner-aim-laser-range-fade]] — parent global task.
