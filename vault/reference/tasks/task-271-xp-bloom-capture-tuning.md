---
id: task-271-xp-bloom-capture-tuning
description: "Global task: XP pickups read as small bright cyan dots (about 8 to 14 px at 1080p, small halo) on real -game screenshots judged by the Orchestrator."
parent: tasks-done-index
kind: task
status: done
---
# XP glow tuned on screenshots

- request: XP too bloomy (big glowing blobs hiding the hex nut); this time screenshots checked by the Orchestrator (owner, 2026-10-21)
- goal: XP pickups read as small bright cyan dots (about 8 to 14 px at 1080p, small halo) on real -game screenshots judged by the Orchestrator.
- acceptance: Captures exist, XP size measured in px; no game process left; Orchestrator judged them; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done without change: XP pickups read as cyan hex nuts with a small halo, no blob (captures Saved/LookTune/final_271_xp_r0_r5.png, final_271_xp_zoom3_r5.png). Still differs from the 8-14 px target: the nut itself is 18-34 px on screen (30 cm mesh); shrinking it (scale about 0.45) is an owner decision. Glow values stay those of task-269 (bp-pickup-xp).

## Subtasks
1. task-271-01-xp-bloom-capture-tuning — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-271-01-xp-bloom-capture-tuning]] — step 1, cpp.
