---
id: task-523-02-doc
description: "Doc: sys-audio: the near reduction of the robot impact"
parent: task-523-audio-impact-near-quieter
kind: task
status: done
---
# Impact : un peu moins fort au corps à corps: doc

- goal: sys-audio: the near reduction of the robot impact
- agent: doc
- depends_on: task-523-01-impact-curve
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md)
- expected_output: see done_when
- done_when: A3 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-523
- usage: included in the executor run of task-523
- runs: 1
- milestone: M2
- started: 2026-11-13T14:41:54Z
- waiting_for: none
- result: Commit f4c8d58c: sys-audio gets near_reduction (row fields, GetNearFactor, Impact values Q9, checked factors), the Near fields in data, the (near f) log in debug, relation to task-523. sys-audio-distance-sounds not listed in inputs: left unchanged (its GetFade entry stays true, the near rule is in sys-audio). Lint clean.

## Relations
- [[task-523-audio-impact-near-quieter]] — parent global task.
