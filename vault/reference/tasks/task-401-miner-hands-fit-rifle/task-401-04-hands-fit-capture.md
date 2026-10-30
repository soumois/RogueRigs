---
id: task-401-04-hands-fit-capture
description: "Capture: game capture: both hands on the rifle, idle and four directions"
parent: task-401-miner-hands-fit-rifle
kind: task
status: todo
---
# Miner hands fitted to the burst rifle: capture

- goal: A -game capture checks the hands on the rifle against C1 to C4.
- agent: capture
- depends_on: task-401-03-abp-left-hand-ik
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Look before: the frames of task-400-05 (rifle in hand without IK); debug command `rr.Debug.MinerAnimForce "x y"`
- expected_output: Report C1..C4 seen, not seen or ambiguous with frame or crop paths; frames sent to the owner.
- done_when: C1 standing still, the right hand is on the pistol grip and the left hand on the handguard; C2 running forward, both hands stay on the rifle; C3 running backward, left and right (MinerAnimForce), both hands stay on the rifle; C4 no arm stretched straight, crossed through the body or in T-pose. Each point reported; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
