---
id: task-295-separation-radius-kamikaze-heavy
description: "Global task: The crowd separation radius of the kamikaze and the heavy grows by 30 %; capsules unchanged."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Kamikaze and heavy separation radius +30 %

- request: "Agrandir la zone d'exclusion autour du kamikaze et du gros robot de 30%" (owner, 2026-10-23)
- goal: URRRobotSeparationSubsystem uses a per-class separation radius = body radius x 1.3 for ARRKamikaze and ARRHeavy (1.0 for the others); capsules, BodyRadius getters and collisions unchanged (provisional, owner 2026-10-23).
- acceptance: separation radius read back 1.3 x body radius on kamikaze and heavy, 1.0 elsewhere; existing tests pass; committed.
- gdd_check: enemy-kamikaze and enemy-heavy read (2026-10-23): crowd spacing line added (provisional, owner 2026-10-23). Reading kept: the zone is the robot-robot separation, not the kamikaze arm range nor the slam radius. No capture: a spacing rule.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-295-01-cpp-separation-radius
- result: open

## Subtasks
1. task-295-01-cpp-separation-radius — cpp: Per-class separation radius

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-295-01-cpp-separation-radius]] — step 01, cpp.
