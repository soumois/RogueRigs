---
id: task-028-02-bp-shield-heavy
description: "BP_ShieldChaser with a pale blue-white translucent shell that breaks, BP_Heavy ring at 400 uu, spawner weights, PIE."
parent: task-028-balance
kind: task
status: done
---
# Shield chaser and heavy visuals

- goal: the new enemy and radius are visible.
- agent: bp
- depends_on: task-028-01-heavy-shield-values
- inputs: [[bp-chaser]], [[bp-heavy]], [[bp-enemy-spawner]], task-028-01 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: BP_ShieldChaser (BP_Chaser look + translucent pale blue-white shell, hidden on OnShieldBroken with a short burst); BP_Heavy telegraph shows the slam area on the ground during the telegraph (ring at the real radius); spawner data for shield chasers; docs. PIE check. Added after task-027 (fixes found in task-027-03 screenshots): (1) BP_Kamikaze EventTick: CoreLight multiplier 60000 → about 10000 (flare blows out); (2) M_DamageNumber or T_DamageDigits: dark outline or drop shadow so digits read over yellow chasers; (3) NS_DamageNumbers: Initialize Particle Lifetime Mode Direct, Lifetime 0.7 (or a user parameter C++ can read); (4) BP_Miner: green heal ring (BP_HealRing) on the level-up heal too, from an existing level-up event; update docs bp-kamikaze, bp-damage-numbers, bp-miner. From task-028-01: BP_ShieldChaser parent ARRShieldChaser (/Script/RogueRigs.RRShieldChaser), NOT a child of BP_Chaser; copy BP_Chaser look (body mesh, eye, XPPickupClass), keep the stats row; shell hidden on OnShieldBroken with a burst (GetShieldFraction may fade it). BP_EnemySpawner WeightedEnemyClasses: add BP_ShieldChaser_C, Weight 0, MaxAlive 0 (director only; DirectorConfig stays at C++ defaults). BP_Heavy ring scale must stay radius / 50 (8 at 400 uu).
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 130000
- model: sonnet
- tokens: input 154, cache write 131557, cache read 6469476, output 45509
- runs: 2
- milestone: M2
- result: BP_Kamikaze CoreLight × 10000; BP_ShieldChaser (duplicate of BP_Chaser reparented to ARRShieldChaser, ShieldShell + MI_GB_ShieldShell, pop and hide on OnShieldBroken); BP_EnemySpawner fifth entry BP_ShieldChaser_C weight 0; BP_Heavy ShockRing from telegraph at radius / 50 (8 at 400), MI_GB_HeavyTelegraph; BP_Miner spawns BP_HealRing on level-up; NS_DamageNumbers lifetime Direct 0.7; T_DamageDigitsOutline (3 px black outline) in M_DamageNumber, old T_DamageDigits deleted. PIE: ring scale 8, shield chasers after 60 s with visible shell, no BP runtime error. Placed spawner instance showed stale four-entry weights in the editor: checked in 028-03. Docs bp-shield-chaser (new) and 6 bp docs. Commits 15e4718, bdea20e, e27b07d, 72eab0c.

## Relations
- [[task-028-balance]] — parent global task.
