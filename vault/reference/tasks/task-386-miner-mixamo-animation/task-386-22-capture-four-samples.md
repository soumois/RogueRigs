---
id: task-386-22-capture-four-samples
description: "Capture: miner standing with MoveX and MoveY forced to each of the four blend space samples"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: capture of each sample

- goal: See which blend space sample shows T-pose arms: force (1,0), (-1,0), (0,-1), (0,1) and (0.5,0).
- agent: capture
- depends_on: task-386-21-force-move-cvar
- inputs: agent-capture, cvar rr.Debug.MinerAnimForce of task-386-21, Saved/Captures/task386_18/crops.png
- expected_output: report
- done_when: one crop per forced value, arms judged T-pose or rifle hold
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T17:09:40Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] - parent global task.
