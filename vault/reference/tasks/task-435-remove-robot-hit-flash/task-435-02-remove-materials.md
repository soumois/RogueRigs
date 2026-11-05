---
id: task-435-02-remove-materials
description: "Bp: The robot materials and instances no longer read the flash and the flash function is deleted."
parent: task-435-remove-robot-hit-flash
kind: task
status: todo
---
# Remove the robot hit flash: bp

- goal: The robot materials and instances no longer read the flash and the flash function is deleted.
- agent: bp
- depends_on: task-435-01-remove-trigger
- inputs: bp-robot-hit-flash (vault/reference/tech/blueprints/enemies/bp-robot-hit-flash.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md)
- expected_output: see done_when
- done_when: Masters recompile with no error and baked animation inputs intact; shield, emblem and kamikaze core instances keep their look; the function asset no longer exists and nothing references it.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-435-remove-robot-hit-flash]] — parent global task.
