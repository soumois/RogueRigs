---
id: task-021-02-bp-shooter
description: "BP_Shooter grey-box (acid green, elbow cannon), red laser on aim, red shot visual, spawner weights 70/15/15, PIE check."
parent: task-021-shooter
kind: task
status: in-progress
---
# Shooter Blueprint

- goal: the shooter is visible and readable in PIE.
- agent: bp
- depends_on: task-021-01-shooter-cpp
- inputs: [[enemy-shooter]], [[color-code]], [[readability-rules]], [[bp-chaser]], [[bp-enemy-spawner]], [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]].
- expected_output: `BP_Shooter` from the C++ class: acid-green body, elbow arm cannon, red eyes, yellow logo plate (rigid primitives); laser beam shown between OnAimStart and OnAimEnd; red projectile Blueprint or material; `BP_EnemySpawner` weights chaser 70 / kamikaze 15 / shooter 15; new doc `bp-shooter`, [[bp-enemy-spawner]] updated. PIE with the cheat property route: shooters appear, keep distance, laser then shot, shots stop on rock; no log error.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-021-shooter]] — parent global task.
