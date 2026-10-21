---
id: task-270-ambience-capture-tuning
description: "Global task: The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots."
parent: tasks-done-index
kind: task
status: done
---
# Ambience tuned on screenshots

- request: XP too bloomy, light too wide, no dark ambience around the player; this time screenshots checked by the Orchestrator (owner, 2026-10-21)
- goal: The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots.
- acceptance: Final values applied in the editor and read back; after captures at the same frames as the baseline judged by the Orchestrator against the DRGS reference and sent to the owner; docs updated with before and after values; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. Frames Saved/LookTune/r5/ and final side by sides Saved/LookTune/final_270_{A_idle,B_wall,D_robots_xp,E_xp}.png (r0 | r5); radius sweep 620/640/660 on D. Seen at 640: warm amber-cream pool core L 155, half radius 210 px (0.22 of width, target 0.18), violet rock tops readable (L 39), chasers golden yellow and identifiable, XP small cyan. Still off: lit rock tops a cream-white crescent (L 242), floor outside the pool very dark (L 10-12 vs about 40 in DRGS), miner top and drone white-hot. 640 kept.

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
10. task-270-10-ambience-radius-r5 — bp, round 5 radius
11. task-270-11-ambience-capture-r5 — cpp, round 5 captures

## Relations
- [[tasks-done-index]] — parent index.
- [[task-270-01-ambience-capture-tuning]] — step 1, cpp.
- [[task-270-02-ambience-sweep]] — step 2, cpp.
- [[task-270-03-ambience-apply]] — step 3, bp.
- [[task-270-04-ambience-after-capture]] — step 4, cpp.
- [[task-270-05-ambience-apply-r3]] — step 5, bp.
- [[task-270-06-ambience-capture-r3]] — step 6, cpp.
- [[task-270-07-ambience-light-r4]] — step 7, bp.
- [[task-270-08-chaser-brightness-r4]] — step 8, bp.
- [[task-270-09-ambience-capture-r4]] — step 9, cpp.
- [[task-270-10-ambience-radius-r5]] — step 10, bp.
- [[task-270-11-ambience-capture-r5]] — step 11, cpp.
