---
id: task-417-01-hit-shake
description: "Cpp: hit shake preset played from the miner on each damage taken"
parent: task-417-camera-shake-on-damage
kind: task
status: in-progress
---
# Camera shake when the miner takes damage: cpp

- goal: Add a hit shake preset class and play it from ARRMiner on each OnDamageReceived with damage > 0 (lethal included), through the task-416 shared shake.
- agent: cpp
- depends_on: none
- inputs: task-417-camera-shake-on-damage (vault/reference/tasks/task-417-camera-shake-on-damage.md), task-416-camera-shake-mixing-falloff (vault/reference/tasks/task-416-camera-shake-mixing-falloff.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md), miner (vault/reference/gdd/player/miner.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: C++: a hit preset class in Source/RogueRigs/Camera/ (5 uu, 0.2 s, 25 Hz, provisional owner 2026-11-02), the call in the miner damage handler with the preset class as an EditDefaultsOnly property; commit task-417-01-hit-shake; change summary for the doc subtask. Uncommitted edits of task-415 (RRAimIndicatorComponent, three test files, Config/DefaultEditor.ini) are not this task's: do not touch or commit them.
- done_when: Editor build green; the miner damage handler plays the hit preset for damage > 0 including the lethal hit, nothing when no damage is consumed; the preset values match Q4; full RogueRigs automation suite with no new failure; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T18:23:52Z
- waiting_for: none
- result: open

## Relations
- [[task-417-camera-shake-on-damage]] — parent global task.
