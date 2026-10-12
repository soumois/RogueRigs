---
id: task-105-hud-insights
description: "Global task: Unreal Insights measures the HUD cost per frame after the refactor and compares it with the task-068-01 trace."
parent: tasks-done-index
kind: task
status: done
---
# HUD cost with Insights after the refactor

- request: "Oui, lance la mesure Insights" after the HUD refactor (owner 2026-10-11) (owner, 2026-10-12)
- goal: Unreal Insights measures the HUD cost per frame after the refactor and compares it with the task-068-01 trace.
- acceptance: one traced and two untraced runs done; HUD cost before/after table; top costs ranking updated; results written; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: none
- result: Done. HUD Blueprint 0.40 -> 0.11 ms per frame (all panel events; WBP_HUD tick scope 0.005); Slate 0.86 -> 0.69; per-panel costs read by name; untraced 12.35-13.72 ms avg, p99 20.3-20.7 (PC load; not attributable to the HUD). Commit 2843106.

## Subtasks
1. task-105-01-hud-insights — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-105-01-hud-insights]] — step 1, cpp.
