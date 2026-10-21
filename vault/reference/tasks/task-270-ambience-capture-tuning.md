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
- result: Done, tuned on 5 capture rounds judged by the Orchestrator (Saved/LookTune/r0-r5, final r0 | r5 side by sides Saved/LookTune/final_270_*.png). Final: PlayerLight height 300 -> 450, radius 650 -> 640, 120 -> 3000 cd, colour (1, 0.62, 0.30) -> (1, 0.75, 0.50); DirectionalLight_0 0.12 -> 1.5 (violet); chaser BodyBrightness 0.7 -> 0.5; sky 0.08, exposure 1.3, bloom unchanged. Seen: warm amber-cream pool (core L 155, half radius 210 px = 0.22 of width), violet rocks readable outside, chasers golden yellow. Still differs from DRGS: lit rock tops burn to a cream-white crescent (diffuse, not specular; the light rock albedo against the dark floor, owner decision), open floor outside the pool L 10-12 vs about 40 (floor albedo, owner decision), miner top and drone white-hot. Revert values in lights-dark-ambient, bp-miner, bp-robot-readability.

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
