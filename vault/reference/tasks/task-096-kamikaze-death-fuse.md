---
id: task-096-kamikaze-death-fuse
description: "Global task: a kamikaze killed by the player goes through the 0.65 s preparation and releases XP at the explosion."
parent: tasks-done-index
kind: task
status: done
---
# Kamikaze death fuse

- request: "rapport de test du 2026-10-11" (owner) (owner, 2026-10-11)
- goal: A kamikaze killed by the player goes through the same 0.65 s preparation as a proximity trigger and releases its XP only at the explosion.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: none
- result: Done. Kamikaze killed by the player side fuses 0.65 s then explodes, XP only after the explosion; killed by an explosion stays instant (rule: lethal effect class recorded by the health set); slot held during the fuse; 228/228 tests. Commit 636ab2b.

## Subtasks
1. task-096-01-kamikaze-death-fuse — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-096-01-kamikaze-death-fuse]] — step 1, cpp.
