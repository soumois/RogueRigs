---
id: task-<number>-<step>-<slug>
description: "<one sentence: the goal of this subtask>"
parent: task-<number>-<slug>
kind: task
status: todo
---
# <Subtask title>

- goal: <one outcome, one agent>
- agent: <agent id>
- depends_on: <subtask ids, or none>
- inputs: <vault node ids and files>
- expected_output: <files, format>
- done_when: <checks the orchestrator runs before marking done>
- editor_state: closed | open | any
- token_ceiling: <number>
- milestone: M1 | M2 | M3
- result: <filled when done: what changed, commit, tokens used>

## Relations
- [[task-<number>-<slug>]] — parent global task.
