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
- next_step: task-270-08-chaser-brightness-r4
- result: open

## Subtasks
1. task-270-01-ambience-capture-tuning — cpp, baseline captures and first runtime sweep
2. task-270-02-ambience-sweep — cpp, second runtime sweep
3. task-270-03-ambience-apply — bp, values set in the editor
4. task-270-04-ambience-after-capture — cpp, after captures
5. task-270-05-ambience-apply-r3 — bp, round 3 values
6. task-270-06-ambience-capture-r3 — cpp, round 3 captures
7. task-270-07-ambience-light-r4 — bp, round 4 light
8. task-270-08-chaser-brightness-r4 — bp, round 4 chaser brightness
9. task-270-09-ambience-capture-r4 — cpp, round 4 captures

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-270-01-ambience-capture-tuning]] — step 1, cpp.
- [[task-270-02-ambience-sweep]] — step 2, cpp.
- [[task-270-03-ambience-apply]] — step 3, bp.
- [[task-270-04-ambience-after-capture]] — step 4, cpp.
- [[task-270-05-ambience-apply-r3]] — step 5, bp.
- [[task-270-06-ambience-capture-r3]] — step 6, cpp.
- [[task-270-07-ambience-light-r4]] — step 7, bp.
- [[task-270-08-chaser-brightness-r4]] — step 8, bp.
- [[task-270-09-ambience-capture-r4]] — step 9, cpp.
