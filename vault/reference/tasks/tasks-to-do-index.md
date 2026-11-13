---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.




- [[task-531-impact-spark-test-fails]] — Automation test RogueRigs.Weapons.ImpactSparkAsset fails (pool not prewarmed, 0 spark requested, 0 played) under -nullrhi and -RenderOffScreen; find whether the test or the impact spark pool / Niagara code is wrong and make it pass without changing the design. Status todo.
