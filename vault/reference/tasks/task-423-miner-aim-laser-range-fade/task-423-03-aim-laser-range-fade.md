---
id: task-423-03-aim-laser-range-fade
description: "Cpp: MaxLength 600 uu and a fixed 150 uu tip fade at every end of the aim laser"
parent: task-423-miner-aim-laser-range-fade
kind: task
status: in-progress
---
# Miner aim laser: visible maximum range and fade-out at its end: cpp

- goal: In URRAimIndicatorComponent: MaxLength 600 uu; replace the fixed TipFadeFrac push by a fade length `TipFadeLength` 150 uu: each refresh, when the laser length changes, push TipFadeFrac = min(TipFadeLength / length, 0.5) to the dynamic material instance, so the last 150 uu fade at the maximum range, at a rock block and at a robot alike (last half below 300 uu). Values per the global Questions (Q2, Q3, Q4 as decided). Stop rule, colour, core, glow, origin, height and hiding unchanged; M_AimLaser asset and BP_Miner not edited. Change summary for the doc subtask.
- agent: cpp
- depends_on: task-423-02-capture-before
- inputs: task-423-miner-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), bp-aim-laser-material (vault/reference/tech/blueprints/fx/bp-aim-laser-material.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: The component change, C++ builds, a commit `task-423-03-aim-laser-range-fade: ...`, a change summary.
- done_when: C++ builds; GetMaxLength returns 600; for a laser length L the pushed TipFadeFrac is min(150 / L, 0.5) (read from code); the material parameter is not pushed every tick when L is unchanged; existing tests pass (known failures excepted), no new test; committed by path.
- editor_state: Unreal editor closed (C++ build)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-03T02:20:42Z
- waiting_for: none
- result: open

## Relations
- [[task-423-miner-aim-laser-range-fade]] — parent global task.
