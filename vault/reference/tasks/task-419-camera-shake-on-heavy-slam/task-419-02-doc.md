---
id: task-419-02-doc
description: "Doc: update sys-enemies-heavy and sys-camera-shake for the slam shake"
parent: task-419-camera-shake-on-heavy-slam
kind: task
status: done
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
- model: sonnet
- tokens: input 8, cache write 30858, cache read 66768, output 4105
- usage: turns 4, context start 11k peak 31k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M3
- started: 2026-11-02T18:59:22Z
- waiting_for: none
- result: Done 2a540c48: sys-enemies-heavy and sys-camera-shake document the slam shake (8 uu, 0.3 s, 25 Hz, 722 uu max, falloff 0.5 at max).

## Relations
- [[task-419-camera-shake-on-heavy-slam]] — parent global task.
