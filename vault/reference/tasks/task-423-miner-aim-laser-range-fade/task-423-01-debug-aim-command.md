---
id: task-423-01-debug-aim-command
description: "Cpp: add the rr.Debug.Aim <YawDegrees> console command that fixes the miner aim for visual checks"
parent: task-423-miner-aim-laser-range-fade
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-423-miner-aim-laser-range-fade]] — parent global task.
