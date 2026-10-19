---
id: task-231-dig-shake-reduced
description: "Global task: smaller dug block shake (8 uu horizontal, 3 uu vertical), drawn on every rock piece."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Dig shake reduced

- request: "Il faudrait réduire le tremblement, maintenant qu'on a amélioré le placeholder" ; amplitude proposée 8 uu horizontal, 3 uu vertical : "Essayons ça" (owner, 2026-10-19)
- goal: The dug block shake is smaller now that the rock look is final: 8 uu horizontal, 3 uu vertical, and it draws on every piece.
- acceptance: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; shake visible in a -game capture and sent to the owner; docs updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: task-231-01-dig-shake-reduced
- result: open

## Subtasks
1. task-231-01-dig-shake-reduced — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-231-01-dig-shake-reduced]] — step 1, cpp.
