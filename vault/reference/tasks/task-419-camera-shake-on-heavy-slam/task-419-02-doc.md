---
id: task-419-02-doc
description: "Doc: update sys-enemies-heavy and sys-camera-shake for the slam shake"
parent: task-419-camera-shake-on-heavy-slam
kind: task
status: in-progress
---
# Camera shake from the heavy ground slam: doc

- goal: Document the slam shake and its values.
- agent: doc
- depends_on: task-419-01-slam-shake
- inputs: task-419-camera-shake-on-heavy-slam (vault/reference/tasks/task-419-camera-shake-on-heavy-slam.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: sys-enemies-heavy (slam frame, data) and sys-camera-shake (callers, preset) updated, lint clean, committed by path.
- done_when: Both nodes state the slam shake trigger, strength and max distance; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T18:59:22Z
- waiting_for: none
- result: open

## Relations
- [[task-419-camera-shake-on-heavy-slam]] — parent global task.
