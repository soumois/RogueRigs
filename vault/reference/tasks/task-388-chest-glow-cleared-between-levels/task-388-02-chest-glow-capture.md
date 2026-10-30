---
id: task-388-02-chest-glow-capture
description: "Capture: -game frames: chest opened on level 1, nothing of it on level 2"
parent: task-388-chest-glow-cleared-between-levels
kind: task
status: todo
---
# Bonus chest glow removed between levels: capture

- goal: Check on frames that the chest glow no longer stays between levels.
- agent: capture
- depends_on: task-388-01-clear-chest-effects
- inputs: agent-capture (vault/reference/agents/agent-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), task-388-01-clear-chest-effects (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-01-clear-chest-effects.md) change summary (commit in the subtask result), debug commands: rr.Cheat.AlertSpeed, rr.Cheat.CompleteHack, rr.Run.EndLevel, rr.Run.Stats.Next, rr.Run.Continue, rr.Debug.Shots
- expected_output: Frames and crops under Saved/Captures/task-388/, report per point seen, not seen or ambiguous with the frame path.
- done_when: C1 level 1: the opened chest glows yellow inside, rays or sparks visible (opening unchanged). C2 level 2 right after Continue (first 2 s): no yellow glow, light, ray or spark at the screen spot of the old chest nor anywhere. C3 level 2 a few seconds later: same, nothing of the chest. Every point reported seen, not seen or ambiguous; ambiguous points go to the owner.
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
- [[task-388-chest-glow-cleared-between-levels]] — parent global task.
