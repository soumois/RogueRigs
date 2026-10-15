---
id: task-181-perf-chaser-meshy-death
description: "Global task: stress-test the Meshy chaser animation and its death puff and debris against the M1 tables."
parent: tasks-to-do-index
kind: task
status: todo
---
# Performance test: Meshy chaser, animation and death effects

- request: Prépare un test de performance maintenant que j'ai inclus le chasseur, animation et particule à sa mort (owner, 2026-10-15)
- goal: Measure the frame cost of the Meshy chaser (animated material) and its death effects (puff, debris) with the stress tool, compared with the M1 tables.
- acceptance: All scenario steps ran and their CSV/log are under Saved/Stress; tables and comparison written in the new perf node; build passes with the editor closed; existing stress tests still pass; report sent to the owner first; committed by path.
- gdd_check: enemy-death (puff and debris, pools of 40) and the M1 stress baselines read (2026-10-14); no conflict; owner chose the skip counter yes, Insights only if needed.
- confirmed_by_owner: yes (2026-10-14), after the RREnemy include task
- milestone: M3
- next_step: task-181-01-perf-chaser-meshy-death
- result: open

## Subtasks
1. task-181-01-perf-chaser-meshy-death — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-181-01-perf-chaser-meshy-death]] — step 1, cpp.
