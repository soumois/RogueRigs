---
id: task-419-01-slam-shake
description: "Cpp: slam shake preset played at the heavy on the slam frame with distance falloff"
parent: task-419-camera-shake-on-heavy-slam
kind: task
status: todo
---
# Camera shake from the heavy ground slam: cpp

- goal: Add a slam shake preset and play it from ARRHeavy on the slam frame at its feet through the task-416 located call.
- agent: cpp
- depends_on: none
- inputs: task-419-camera-shake-on-heavy-slam (vault/reference/tasks/task-419-camera-shake-on-heavy-slam.md), task-416-camera-shake-mixing-falloff (vault/reference/tasks/task-416-camera-shake-mixing-falloff.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: C++: slam preset class (8 uu, 0.3 s, 25 Hz), EditDefaultsOnly preset class and max-distance factor on ARRHeavy, the call in PerformSlam; values provisional owner 2026-11-02; commit task-419-01-slam-shake; change summary for the doc subtask. Uncommitted edits of task-415 (RRAimIndicatorComponent, three test files, Config/DefaultEditor.ini) are not this task's: do not touch or commit them.
- done_when: Editor build green; PerformSlam calls the located shake once per slam at the feet with 8 uu and 722 uu max distance (2 x AttackRange); no call in Telegraph; full RogueRigs automation suite with no new failure (heavy tests included); committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-419-camera-shake-on-heavy-slam]] — parent global task.
