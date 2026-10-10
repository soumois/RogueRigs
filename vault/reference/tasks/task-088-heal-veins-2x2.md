---
id: task-088-heal-veins-2x2
description: "Global task: Heal veins are 2 x 2 in the layout and in code: 12 veins, 15 HP per pickup, 60 HP per vein."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heal veins 2 x 2 (layout)

- request: "2x2 pour les minerais et la vie, 8 % d'or, un taux par minerai; ne pas prendre les soins si la vie est pleine" (owner 2026-10-10) (owner, 2026-10-10)
- goal: Heal veins are 2 x 2 in the layout and in code: 12 veins, 15 HP per pickup, 60 HP per vein.
- acceptance: validate_layout.js exits 0; 12 heal veins of 2 x 2 (48 cells) counted by script; layout notes updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: task-088-02-heal-veins-2x2
- result: open

## Subtasks
1. task-088-01-heal-veins-2x2 — level-design
2. task-088-02-heal-veins-2x2 — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-088-01-heal-veins-2x2]] — step 1, level-design.
- [[task-088-02-heal-veins-2x2]] — step 2, cpp.
