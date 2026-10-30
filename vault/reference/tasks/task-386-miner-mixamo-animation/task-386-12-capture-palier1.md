---
id: task-386-12-capture-palier1
description: "Capture: -game frames of the miner standing still, the T-pose must be gone"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: capture of step 1

- goal: Check by eye in a -game run that the miner now plays the rifle-aiming idle.
- agent: capture
- depends_on: task-386-11-abp-palier1-idle
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), previous T-pose frames Saved/Captures/MinerAnim2/idle_00.png and crops-idle.png, reference palette Art/Source/miner-meshy/texture_0.png, debug commands of agent-capture
- expected_output: frames and crops where agent-capture stores them, one line per point with the frame path
- done_when: C1 the miner is not in T-pose or A-pose (arms and hands hold a rifle-aiming pose, idle breathing visible across at least two frames some seconds apart); C2 feet touch the floor; C3 the Meshy mesh is shown, not Quinn. If a point is not seen, say so; the Executor then diagnoses this step only
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T06:28:10Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
