---
id: task-270-ambience-capture-tuning
description: "Global task: The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Ambience tuned on screenshots

- request: XP too bloomy, light too wide, no dark ambience around the player; this time screenshots checked by the Orchestrator (owner, 2026-10-21)
- goal: The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots.
- acceptance: Final values applied in the editor and read back; after captures at the same frames as the baseline judged by the Orchestrator against the DRGS reference and sent to the owner; docs updated with before and after values; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: task-270-02-ambience-sweep
- result: open

## Subtasks
1. task-270-01-ambience-capture-tuning — cpp, baseline captures and first runtime sweep
2. task-270-02-ambience-sweep — cpp, second runtime sweep
3. task-270-03-ambience-apply — bp, values set in the editor
4. task-270-04-ambience-after-capture — cpp, after captures

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-270-01-ambience-capture-tuning]] — step 1, cpp.
- [[task-270-02-ambience-sweep]] — step 2, cpp.
- [[task-270-03-ambience-apply]] — step 3, bp.
- [[task-270-04-ambience-after-capture]] — step 4, cpp.
