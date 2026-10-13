---
id: task-113-capture-zone-visual
description: "Global task: The console and chest hack zones show a sharp zone circle and a growing progress circle."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Capture zone visual (C++ side)

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: The console and chest hack zones show a sharp zone circle and a growing progress circle; the old diffuse circle is gone; the rules are unchanged.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-113-02-capture-zone-visual
- result: open

## Subtasks
1. task-113-01-capture-zone-visual — cpp
2. task-113-02-capture-zone-visual — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-113-01-capture-zone-visual]] — step 1, cpp.
- [[task-113-02-capture-zone-visual]] — step 2, bp.
