---
id: task-386-22-capture-four-samples
description: "Capture: miner standing with MoveX and MoveY forced to each of the four blend space samples"
parent: task-386-miner-mixamo-animation
kind: task
status: done
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
- model: sonnet
- tokens: input 38, cache write 41719, cache read 509189, output 9642
- usage: turns 17, context start 14k peak 49k, resumes 2 (message x2), 4 min, units 0.15 M
- runs: 3
- milestone: M2
- started: 2026-10-30T17:09:40Z
- waiting_for: none
- result: inconclusive: forced values show the same idle-like rifle pose for all five (task386_22c/crops2.png); cvar needs exec files for quoting

## Relations
- [[task-386-miner-mixamo-animation]] - parent global task.
