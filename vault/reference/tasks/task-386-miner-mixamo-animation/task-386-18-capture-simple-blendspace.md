---
id: task-386-18-capture-simple-blendspace
description: "Capture: -game frames with MoveX, MoveY log: forward and one side run without T-pose arms"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: capture of the simple blend space

- goal: See in -game the miner run forward and to one side without arms stretched out, with the MoveX, MoveY log.
- agent: capture
- depends_on: task-386-17-simple-blendspace
- inputs: agent-capture, previous captures Saved/Captures/task386_08 and task386_14, rr.Pacing.Run, the debug log of MoveX, MoveY, GroundSpeed (task-386-16)
- expected_output: frames, crops, log lines of MoveX, MoveY and frame times, one line per point
- done_when: C1 forward run seen without T-pose arms; C2 a side run seen; C3 each seen frame matched to logged MoveX, MoveY; missing relative-move command named precisely
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T16:58:00Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.