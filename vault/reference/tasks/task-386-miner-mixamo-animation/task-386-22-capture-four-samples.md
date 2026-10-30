---
id: task-386-22-capture-four-samples
description: "Capture: miner standing with MoveX and MoveY forced to each of the four blend space samples"
parent: task-386-miner-mixamo-animation
kind: task
status: wait-answers
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
- started: 2026-10-30T17:04:45Z
- waiting_for: Forced MoveX/MoveY capture (Saved/Captures/task386_22c/crops2.png) shows the SAME rifle-hold idle-like pose for all 5 values (no run, no T-pose), while real movement (task386_18) shows running legs with T-pose arms: next diagnosis needs a side camera or a pose read-back (which state and sample play)
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] - parent global task.
