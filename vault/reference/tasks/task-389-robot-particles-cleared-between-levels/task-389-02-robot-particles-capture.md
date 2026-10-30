---
id: task-389-02-robot-particles-capture
description: "Capture: -game frames: robots killed just before extraction, no particles on level 2"
parent: task-389-robot-particles-cleared-between-levels
kind: task
status: todo
---
# Robot particles removed between levels: capture

- goal: Check on frames that robot particles of the previous level no longer show.
- agent: capture
- depends_on: task-389-01-clear-robot-effects
- inputs: agent-capture (vault/reference/agents/agent-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), task-389-01-clear-robot-effects (vault/reference/tasks/task-389-robot-particles-cleared-between-levels/task-389-01-clear-robot-effects.md) change summary, debug commands: rr.Debug.SpawnNear, rr.Debug.KillRobots, rr.Run.EndLevel, rr.Run.Stats.Next, rr.Run.Continue, rr.Debug.Shots
- expected_output: Frames and crops under Saved/Captures/task-389/, report per point.
- done_when: C1 level 1: death puffs or debris visible right after the kills (effects unchanged). C2 level 2, first 2 s after Continue: no puff, smoke or debris of robots anywhere on screen. Every point reported seen, not seen or ambiguous; ambiguous points go to the owner.
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
- [[task-389-robot-particles-cleared-between-levels]] — parent global task.
