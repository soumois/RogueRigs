---
id: task-423-05-doc
description: "Doc: update sys-aim-indicator, bp-aim-laser-material and tool-capture for the range, fade and rr.Debug.Aim"
parent: task-423-miner-aim-laser-range-fade
kind: task
status: todo
---
# Miner aim laser: visible maximum range and fade-out at its end: doc

- goal: Update the doc nodes from the change summaries of task-423-01 and task-423-03: sys-aim-indicator (MaxLength, TipFadeLength and the per-length TipFadeFrac push, values provisional owner 2026-11-03, rr.Debug.Aim), bp-aim-laser-material (TipFadeFrac now driven by the component per laser length), tool-capture (rr.Debug.Aim for aim-dependent captures).
- agent: doc
- depends_on: task-423-01-debug-aim-command, task-423-03-aim-laser-range-fade, task-423-04-capture-after
- inputs: task-423-miner-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), bp-aim-laser-material (vault/reference/tech/blueprints/fx/bp-aim-laser-material.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: The three doc nodes updated, lint clean, a commit `task-423-05-doc: ...`.
- done_when: sys-aim-indicator, bp-aim-laser-material and tool-capture state the new values and command as built; no gdd node edited; lint clean; committed by path.
- editor_state: any (no editor needed)
- token_ceiling: 0.3 M units
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
