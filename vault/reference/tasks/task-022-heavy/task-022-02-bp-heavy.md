---
id: task-022-02-bp-heavy
description: "BP_Heavy grey-box (steel blue, hazard stripes, 2× visual), telegraph glow and tilt, shockwave visual, spawner weights 65/12/15/8 and cap 5, PIE."
parent: task-022-heavy
kind: task
status: done
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
- model: sonnet
- tokens: input 174, cache write 348908, cache read 9269314, output 59211
- runs: 4
- milestone: M3
- result: BP_Heavy (steel-blue cylinder 2× chaser, 4 yellow/black stripes, red eyes, yellow plate, NoCollision parts, capsule untouched; ShockRing disc): OnTelegraphStart glow MI_GB_HeavyGlow + 14° tilt, OnSlam ring to Radius/50 over 0.24 s, OnRecover resets. New MI_GB_Heavy, MI_GB_HazardBlack, MI_GB_HeavyGlow. BP_EnemySpawner defaults 65/12/15/8 with heavy MaxAlive 5. PIE: 5 heavies alive (cap), telegraph → slam → recover → chase sampled on a teleported heavy; slam damage on a vulnerable miner not checked (C++ tests cover it); placed spawner in the open editor showed old weights (session-only, saved map has no override: verify in 022-03); no game log error. Docs bp-heavy (new), bp-enemy-spawner, blueprints-index. Commits 8b37df6, a54034c.

## Relations
- [[task-022-heavy]] — parent global task.
