---
id: task-428-02-glow-from-spawn-capture
description: "Capture: in-game check of the XP glow in the air, at rest and on magnet"
parent: task-428-xp-glow-from-spawn
kind: task
status: todo
---
# XP pickup glow disc on from spawn: capture

- goal: Run the game (-game) and check the checklist on frames; only if Q5 keeps the capture (if Q5 is answered "no capture", the Executor closes this subtask on the owner word without a run).
- agent: capture
- depends_on: task-428-01-glow-from-spawn-cpp
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), task-428-xp-glow-from-spawn (vault/reference/tasks/task-428-xp-glow-from-spawn.md) (acceptance A1 to A4), debug commands: open (rr.Debug.Shots per agent-capture)
- expected_output: Frames and crops under Saved/Scratch or the capture folder, one verdict per point (seen, not seen, ambiguous) with paths.
- done_when: Checklist reported: C1 an XP nut in mid-air during its pop arc shows its cyan glow disc. C2 a resting XP nut shows its glow disc. C3 a nut flying to the miner has no glow disc (Q1 default) or keeps it (Q1 other choice). C4 a big XP cube in the air shows its magenta glow disc (if Q3 keeps it; report not seen if no big cube dropped). Ambiguous points go to the owner.
- editor_state: closed (packaged -game run from the uproject)
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
- [[task-428-xp-glow-from-spawn]] — parent global task.
