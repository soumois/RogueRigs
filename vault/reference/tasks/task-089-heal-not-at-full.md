---
id: task-089-heal-not-at-full
description: "Global task: a heal pickup is not taken at full health and stays on the floor until health drops."
parent: tasks-to-do-index
kind: task
status: todo
---
# Heal pickup not taken at full health

- request: "2x2 pour les minerais et la vie, 8 % d'or, un taux par minerai; ne pas prendre les soins si la vie est pleine" (owner 2026-10-10) (owner, 2026-10-10)
- goal: A heal pickup is not taken when the miner is at full health; it stays on the floor until health is below the maximum.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated (sys-pickups); lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: task-089-01-heal-not-at-full
- result: open

## Subtasks
1. task-089-01-heal-not-at-full — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-089-01-heal-not-at-full]] — step 1, cpp.
