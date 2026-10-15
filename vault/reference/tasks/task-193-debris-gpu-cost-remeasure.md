---
id: task-193-debris-gpu-cost-remeasure
description: "Global task: Remeasure the GPU debris with wall collision with the PC in its usual state, against task-182-03, frame for frame."
parent: tasks-done-index
kind: task
status: done
---
# Robot debris GPU and walls: remeasure

- request: A puis C: refaire la mesure de la 191-02 avec le PC dans son état habituel (owner, 2026-10-15) (owner, 2026-10-15)
- goal: Remeasure the GPU debris with wall collision with the PC in its usual state, against task-182-03, frame for frame.
- acceptance: Control step at the usual speed; runs done; section written; report sent to the owner first; committed by path; committed.
- gdd_check: enemy-death read (2026-10-15): no conflict (measure and pool fix only).
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: none
- result: Done. Control passed after Armoury Crate Performance (8.65 ms, GPU 7.63); GPU debris with walls vs 182-03: game 200 frame 11.08 -> 14.15, game 300 12.96 -> 15.74 (p99 21.4, above the 181 limits), S7 200 max 25.9 -> 54.1; plays/skips real (S7 80/120); trace: render thread about 68 us per active GPU debris system (dynamic primitive upload, one pass per system), GPU +1.3-1.5 ms incl. global distance field 0.9 ms; first-use hitch gone in editor -game; section in perf-stress-chaser-meshy; commits 65a1e7aa, d1e06c83

## Subtasks
1. task-193-01-debris-gpu-cost-remeasure — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-193-01-debris-gpu-cost-remeasure]] — step 1, cpp.
