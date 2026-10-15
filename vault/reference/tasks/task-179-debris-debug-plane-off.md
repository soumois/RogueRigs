---
id: task-179-debris-debug-plane-off
description: "Global task: Remove the red debug square drawn on the floor under each active debris system (NS_RobotDebris collision plane debug draw)."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot debris: hide the red debug plane

- request: J'ai un rectangle rouge de débug au sol, c'est quoi ? (owner screenshot: two red squares on the floor near XP pickups, also on the task-166-05 game shot) (owner, 2026-10-15)
- goal: Remove the red debug square drawn on the floor under each active debris system (NS_RobotDebris collision plane debug draw).
- acceptance: The debug inputs read back off; compiles with no stack issue; saved by explicit path; a -game run with 1 or 2 rr.Debug.Shots right after a chaser death shows no red square on the floor (images described); doc updated; committed by path; the editor stays open; committed.
- gdd_check: enemy-death and burst-rifle checked (2026-10-14): no conflict; the burst rifle has no knockback, only the shotgun does.
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: task-179-01-debris-debug-plane-off
- result: open

## Subtasks
1. task-179-01-debris-debug-plane-off — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-179-01-debris-debug-plane-off]] — step 1, bp.
