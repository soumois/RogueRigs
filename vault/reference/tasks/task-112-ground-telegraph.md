---
id: task-112-ground-telegraph
description: "Global task: A generic ground signal component draws a zone circle and a growing circle, and dies with its owner."
parent: tasks-to-do-index
kind: task
status: todo
---
# Ground telegraph component (C++)

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: A generic ground signal component draws a zone circle and a growing circle on the floor, dies with its owner, and ignores robots, the miner and player weapons.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-112-01-ground-telegraph
- result: open

## Subtasks
1. task-112-01-ground-telegraph — cpp
2. task-112-02-ground-telegraph — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-112-01-ground-telegraph]] — step 1, cpp.
- [[task-112-02-ground-telegraph]] — step 2, bp.
