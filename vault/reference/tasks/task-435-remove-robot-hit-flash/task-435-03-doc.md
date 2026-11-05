---
id: task-435-03-doc
description: "Doc: Remove the flash from the docs that describe it."
parent: task-435-remove-robot-hit-flash
kind: task
status: done
---
# Remove the robot hit flash: doc

- goal: Remove the flash from the docs that describe it.
- agent: doc
- depends_on: task-435-02-remove-materials
- inputs: bp-robot-hit-flash (vault/reference/tech/blueprints/enemies/bp-robot-hit-flash.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md), bp-kamikaze (vault/reference/tech/blueprints/enemies/bp-kamikaze.md)
- expected_output: see done_when
- done_when: No doc describes the flash as current; bp-robot-hit-flash removed or marked history with its index line updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-435
- usage: included in the executor run of task-435
- runs: 1
- milestone: M2
- started: 2026-11-05T17:35:19Z
- waiting_for: none
- result: bp-robot-hit-flash and its index line deleted (history in git and task-433); hit flash lines removed from sys-enemies-base, sys-damage, sys-tests-enemies, bp-robot-meshy-material, bp-robot-eye-material, bp-shield-chaser-v2; bp-kamikaze had no mention. Lint clean.

## Relations
- [[task-435-remove-robot-hit-flash]] — parent global task.
