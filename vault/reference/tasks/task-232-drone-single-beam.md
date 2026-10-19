---
id: task-232-drone-single-beam
description: "Global task: The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# One drone beam

- request: "Il faudrait juste un rayon, même si on mine plusieurs rochers. ( il vise au milieu )" ; défaut accepté : visé au milieu des points d'impact des blocs creusés, sur la face du mur (owner, 2026-10-19)
- goal: The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number.
- acceptance: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures (tests expecting one beam per block updated); capture sent to the owner; docs updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: task-232-01-drone-single-beam
- result: open

## Subtasks
1. task-232-01-drone-single-beam — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-232-01-drone-single-beam]] — step 1, cpp.
