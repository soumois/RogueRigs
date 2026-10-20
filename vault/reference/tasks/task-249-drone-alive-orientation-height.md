---
id: task-249-drone-alive-orientation-height
description: "Global task: The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures."
parent: tasks-done-index
kind: task
status: done
---
# Drone orientation and height variation

- request: give the drone some variation in orientation and height, as if someone commanded it (owner, 2026-10-20)
- goal: The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures.
- acceptance: Build passes; existing drone tests pass; one position check (headless, no capture): drone height stays inside 130-190 uu above the floor and its yaw faces the dig target during Dig; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Height 130-190 uu (two-frequency bob, higher when an enemy is within 700 uu), yaw toward point of interest (dig/hack target, enemy, deposit/objective, miner) refreshed at 4 Hz, roll/pitch from acceleration (max 15 deg), idle gestures every 4-8 s. Build OK; DroneAlive check: 870 Orbit frames, height 146.0-170.1 uu, 0 outside 130-190; Dig yaw error at most 8.9 deg after settling (tolerance 20). HealVein still fails (known). Alert rise, look priorities and gesture types untested; not seen in play.

## Subtasks
1. task-249-01-drone-alive-orientation-height — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-249-01-drone-alive-orientation-height]] — step 1, cpp.
