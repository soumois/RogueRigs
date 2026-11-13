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




- [[task-533-task-create-auto-number]] — task-create.js assigns the global task number itself (highest existing + 1) and takes the index anchor itself, so the planner no longer lists vault/reference/tasks (1062 entries, 31k characters) before creating; task-spec loses that rule. Status in-progress.
- [[task-534-descent-script]] — A vault script prints in one call the ## Children glosses of an index and of its child indexes, with their paths, so reaching a gdd node takes one round trip instead of 2 to 3; _navigation authorises it as a descent by glosses, like path-of.js. Status todo.
- [[task-535-task-status-usage-line]] — task-spec gets one line with the exact syntax of task-status.js for the planner, e.g. putting an existing task back to wait-answers with waiting_for, so it no longer spends cycles guessing or reading the script. Status todo.
- [[task-536-layout-filler-cheaper]] — FRRLayoutGenerator::RunFiller costs less per iteration (reused buffers, squared distances, no recompute when a walk carved nothing) with byte-identical cells for the same params and seed; slow seed timed before and after. Status in-progress.
