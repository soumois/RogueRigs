---
id: task-181-perf-chaser-meshy-death
description: "Global task: stress-test the Meshy chaser animation and its death puff and debris against the M1 tables."
parent: tasks-done-index
kind: task
status: done
---
# Performance test: Meshy chaser, animation and death effects

- request: Prépare un test de performance maintenant que j'ai inclus le chasseur, animation et particule à sa mort (owner, 2026-10-15)
- goal: Measure the frame cost of the Meshy chaser (animated material) and its death effects (puff, debris) with the stress tool, compared with the M1 tables.
- acceptance: All scenario steps ran and their CSV/log are under Saved/Stress; tables and comparison written in the new perf node; build passes with the editor closed; existing stress tests still pass; report sent to the owner first; committed by path.
- gdd_check: enemy-death (puff and debris, pools of 40) and the M1 stress baselines read (2026-10-14); no conflict; owner chose the skip counter yes, Insights only if needed.
- confirmed_by_owner: yes (2026-10-14), after the RREnemy include task
- milestone: M3
- next_step: none
- result: Done. Pool play/skip counters in Stress step and CSV; S0 50-600 avg 7.99-11.13 ms, S2 50-400 8.32-10.55 (faster than M1, game thread -32 to -51 %); new draw/GPU +5.6 us per robot (trace S0 600: Lumen card captures likely); S7 chasers 50-200 max 27-28 ms (M1 49.6), cap 40 holds; game caps 100/200/300 avg 9.73/10.75/12.28, p99 12.3/15.4/18.2; debris skipped 1/30/45 % of deaths; GC spikes 39-47 ms as in M1; report perf-stress-chaser-meshy; commits c719274c, bc42ec48

## Subtasks
1. task-181-01-perf-chaser-meshy-death — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-181-01-perf-chaser-meshy-death]] — step 1, cpp.
