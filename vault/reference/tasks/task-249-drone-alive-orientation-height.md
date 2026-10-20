---
id: task-249-drone-alive-orientation-height
description: "Global task: The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Drone orientation and height variation

- request: give the drone some variation in orientation and height, as if someone commanded it (owner, 2026-10-20)
- goal: The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures.
- acceptance: Build passes; existing drone tests pass; one position check (headless, no capture): drone height stays inside 130-190 uu above the floor and its yaw faces the dig target during Dig; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-249-01-drone-alive-orientation-height
- result: open

## Subtasks
1. task-249-01-drone-alive-orientation-height — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-249-01-drone-alive-orientation-height]] — step 1, cpp.
