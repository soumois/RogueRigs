---
id: task-401-04-hands-fit-capture
description: "Capture: game capture: both hands on the rifle, idle and four directions"
parent: task-401-miner-hands-fit-rifle
kind: task
status: done
---
# Miner hands fitted to the burst rifle: capture

- goal: A -game capture checks the hands on the rifle against C1 to C4.
- agent: capture
- depends_on: task-401-03-abp-left-hand-ik
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Look before: the frames of task-400-06 (rifle in hand without IK); debug command `rr.Debug.MinerAnimForce "x y"`
- expected_output: Report C1..C4 seen, not seen or ambiguous with frame or crop paths; frames sent to the owner.
- done_when: C1 standing still, the right hand is on the pistol grip and the left hand on the handguard; C2 running forward, both hands stay on the rifle; C3 running backward, left and right (MinerAnimForce), both hands stay on the rifle; C4 no arm stretched straight, crossed through the body or in T-pose. Each point reported; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 22, cache write 36023, cache read 665790, output 4855
- usage: turns 10, context start 58k peak 80k, resumes 2 (message x2), 2 min, units 0.14 M
- runs: 1
- milestone: M2
- started: 2026-10-31T05:53:42Z
- waiting_for: none
- result: Capture 7 frames in Saved/Captures/HandsFit401: C1 C2 C3 ambiguous (right hand occluded by head from top-down camera; left hand at front of rifle, in left run slightly below/ahead of barrel front), C4 seen with caveats (no T-pose, crossed or splayed arm; elbows hidden). Ambiguous points go to the owner.

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
