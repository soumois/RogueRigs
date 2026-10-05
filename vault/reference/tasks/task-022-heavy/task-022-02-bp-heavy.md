---
id: task-022-02-bp-heavy
description: "BP_Heavy grey-box (steel blue, hazard stripes, 2× visual), telegraph glow and tilt, shockwave visual, spawner weights 65/12/15/8 and cap 5, PIE."
parent: task-022-heavy
kind: task
status: todo
---
# Heavy Blueprint

- goal: the heavy is visible and readable in PIE.
- agent: bp
- depends_on: task-022-01-heavy-cpp
- inputs: [[enemy-heavy]], [[color-code]], [[readability-rules]], [[bp-chaser]], [[bp-shooter]], [[bp-enemy-spawner]], [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]].
- expected_output: `BP_Heavy` from the C++ class: steel-blue body with yellow and black hazard stripes, red eyes, yellow logo plate, about twice the chaser visually (capsule left as C++ sets it); telegraph visual (tilt and glow) on OnTelegraphStart, a short shockwave ring on OnSlam using its radius; XP pickup class; `BP_EnemySpawner` weights chaser 65 / kamikaze 12 / shooter 15 / heavy 8 and heavy cap 5; new doc `bp-heavy`, [[bp-enemy-spawner]] updated. PIE with the cheat property route: heavies spawn, telegraph then slam, pass corridors; no log error.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-022-heavy]] — parent global task.
