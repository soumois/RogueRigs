---
id: task-386-24-capture-sample-test
description: "Capture: real run with the blend space samples all set to one clip, side camera if available"
parent: task-386-miner-mixamo-animation
kind: task
status: cancelled
---
# Miner animated with the owner Mixamo clips: capture of the sample test

- goal: Show whether the arms hold the rifle during a real run when every sample is the same clip.
- agent: capture
- depends_on: task-386-23-run-clip-asset-check
- inputs: agent-capture, task386_18 command line (rr.Pacing.Run), a side camera if rr.Debug.Camera or similar exists
- expected_output: report
- done_when: frames and crops of the running miner, arms judged T-pose or rifle hold, per test
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T17:11:55Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] - parent global task.
