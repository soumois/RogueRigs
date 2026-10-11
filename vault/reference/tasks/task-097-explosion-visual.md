---
id: task-097-explosion-visual
description: "Global task: The visible explosion is about 5 % larger than its collision radius and holds its final state 0.1 s before fading."
parent: tasks-done-index
kind: task
status: done
---
# Explosion visual size and hold

- request: "rapport de test du 2026-10-11" (owner) (owner, 2026-10-11)
- goal: The visible explosion is about 5 % larger than its collision radius and holds its final state 0.1 s before fading.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; C++ part done and the Blueprint part listed if needed; docs sys-explosions updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: none
- result: Done. Every explosion FX is 1.05 x the collision radius and holds 0.1 s at full size before a 0.25 s fade (VisualRadiusScale, VisualHoldSeconds, FadeSeconds); collision and damage unchanged; no Blueprint change; 229/229 tests. Commit 8dfaaa2.

## Subtasks
1. task-097-01-explosion-visual — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-097-01-explosion-visual]] — step 1, cpp.
