---
id: task-304-console-marked-rocks-range
description: "Global task: The console lands only where its zone holds between 10 and 40 markable blocks, with both bounds editable in Blueprint defaults."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Console marked rocks between 10 and 40

- request: "Il faut aussi un nombre de dalle minimum à miner (10) et un nombre maximal (40). Qu'on ait un contrôle là dessus." (owner, 2026-10-23)
- goal: The console lands only where its zone holds between 10 and 40 markable blocks, with both bounds editable in Blueprint defaults.
- acceptance: drop picker keeps only spots whose zone holds MinMarkedCells 10 to MaxMarkedCells 40 markable blocks; no fitting spot: the spot with the nearest count, logged as a warning; both EditDefaultsOnly on the console; the bonus chest (no marked rocks) unaffected; GDD and tech doc; committed.
- gdd_check: objective-console read (2026-10-23): no marked-rock count rule before; owner default: 10..40 (provisional owner 2026-10-23), replaces the plain marked-rock preference; fallback nearest count with a warning.
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-304-01-console-marked-rocks-range
- result: open

## Subtasks
1. task-304-01-console-marked-rocks-range — cpp: Drop picker marked-rock range

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-304-01-console-marked-rocks-range]] — step 01, cpp.
