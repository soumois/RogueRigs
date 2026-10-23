---
id: task-295-separation-radius-kamikaze-heavy
description: "Global task: The crowd separation radius of the kamikaze and the heavy grows by 30 %; capsules unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Kamikaze and heavy separation radius +30 %

- request: "Agrandir la zone d'exclusion autour du kamikaze et du gros robot de 30%" (owner, 2026-10-23)
- goal: URRRobotSeparationSubsystem uses a per-class separation radius = body radius x 1.3 for ARRKamikaze and ARRHeavy (1.0 for the others); capsules, BodyRadius getters and collisions unchanged (provisional, owner 2026-10-23).
- acceptance: separation radius read back 1.3 x body radius on kamikaze and heavy, 1.0 elsewhere; existing tests pass; committed.
- gdd_check: enemy-kamikaze and enemy-heavy read (2026-10-23): crowd spacing line added (provisional, owner 2026-10-23). Reading kept: the zone is the robot-robot separation, not the kamikaze arm range nor the slam radius. No capture: a spacing rule.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. SeparationRadiusScale on ARREnemy (default 1.0), 1.3 on kamikaze and heavy; separation radius kamikaze 37.4 -> 48.6 uu, heavy 48 -> 62.4 uu, chaser 24, shield chaser 24, shooter 34 unchanged; capsules and GetBodyRadius unchanged; build clean, 38 enemy tests pass, no test changed; commits 7569c5a5, 63f60eff

## Subtasks
1. task-295-01-cpp-separation-radius — cpp: Per-class separation radius

## Relations
- [[tasks-done-index]] — parent index.
- [[task-295-01-cpp-separation-radius]] — step 01, cpp.
