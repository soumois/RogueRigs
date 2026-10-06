---
id: task-028-02-bp-shield-heavy
description: "BP_ShieldChaser with a pale blue-white translucent shell that breaks, BP_Heavy ring at 400 uu, spawner weights, PIE."
parent: task-028-balance
kind: task
status: todo
---
# Shield chaser and heavy visuals

- goal: the new enemy and radius are visible.
- agent: bp
- depends_on: task-028-01-heavy-shield-values
- inputs: [[bp-chaser]], [[bp-heavy]], [[bp-enemy-spawner]], task-028-01 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: BP_ShieldChaser (BP_Chaser look + translucent pale blue-white shell, hidden on OnShieldBroken with a short burst); BP_Heavy telegraph shows the slam area on the ground during the telegraph (ring at the real radius); spawner data for shield chasers; docs. PIE check. Added after task-027 (fixes found in task-027-03 screenshots): (1) BP_Kamikaze EventTick: CoreLight multiplier 60000 → about 10000 (flare blows out); (2) M_DamageNumber or T_DamageDigits: dark outline or drop shadow so digits read over yellow chasers; (3) NS_DamageNumbers: Initialize Particle Lifetime Mode Direct, Lifetime 0.7 (or a user parameter C++ can read); (4) BP_Miner: green heal ring (BP_HealRing) on the level-up heal too, from an existing level-up event; update docs bp-kamikaze, bp-damage-numbers, bp-miner.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 130000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-028-balance]] — parent global task.
