---
id: task-036-recycle-relocate
description: "Global task: robots left far behind are moved back ahead of the miner instead of removed, so pressure stays."
parent: tasks-index
kind: task
status: todo
---
# Recycling by relocation

- request: owner 2026-10-06: a robot spawned ahead must not stay stuck far behind when the player turns; when the player moves away, keep the pressure.
- goal: robots far behind come back ahead of the running miner.
- acceptance: (1) off screen and > 1800 uu: seen once → 3 s, never seen → 6 s, then the robot reappears in the spawn band in the front sector with its type and health; never near the camera; (2) build exits 0; tests pass; (3) docs; lint clean; committed.
- gdd_check: GDD enemies-index updated by the Orchestrator (owner 2026-10-06). No open value.
- confirmed_by_owner: yes (2026-10-06)
- milestone: M2
- next_step: task-036-01-relocate
- result: open

## Subtasks
1. task-036-01-relocate — cpp: relocation instead of removal, seen-once and grace timers

## Relations
- [[tasks-index]] — parent index.
- [[task-036-01-relocate]] — step 1, cpp.
