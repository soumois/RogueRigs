---
id: task-087-card-desc-test
description: "Global task: The existing test Integration.WeaponCardEquipsShotgun matches the card flavor line now shown on stat cards."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Card description test update

- request: "fix existing test expectations a requested change breaks" (task-071-01 found the test failing: CardDesc1/2 collapsed on a stat card; the flavor line of tasks 080/084 is now shown there) (owner, 2026-10-10)
- goal: The existing test Integration.WeaponCardEquipsShotgun matches the card flavor line now shown on stat cards.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless (204/204); sys-automation-tests updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: task-087-01-card-desc-test
- result: open

## Subtasks
1. task-087-01-card-desc-test — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-087-01-card-desc-test]] — step 1, cpp.
