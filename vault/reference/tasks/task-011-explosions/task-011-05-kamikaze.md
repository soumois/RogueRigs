---
id: task-011-05-kamikaze
description: "ARRKamikaze on ARREnemy: hover look, arms within 150 uu, 0.5 s fuse with a growing pulsing core, explodes on fuse or death; weighted spawner."
parent: task-011-explosions
kind: task
status: todo
---
# Kamikaze

- goal: the second robot, built on the enemy base.
- agent: cpp
- depends_on: task-011-04-barrel
- inputs: [[enemy-kamikaze]], [[sys-enemies]], [[sys-explosions]], [[sys-spawner]], [[sys-flow-field]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `EnemyStats.csv` row `Kamikaze` (MaxHealth 15, MoveSpeed 520, XPValue 1, other columns 0 or unused). `ARRKamikaze : ARREnemy`: capsule about 0.6 × the miner on the ground (flow field unchanged), visual meshes lifted by an editable `HoverHeight` with a slight bob; component `CoreMesh` pulses its scale (editable base rate); states Chase → Armed (within editable `ArmRange` 150 uu: stops, core pulses faster and grows over `FuseTime` 0.5 s) → explodes (queues a `Kamikaze` explosion, dies without XP); if killed before exploding, it queues the same explosion and drops its XP. A kamikaze caught in another explosion dies and explodes too. `ARREnemySpawner`: weighted classes (array of {class, weight}); the single `EnemyClass` still works when the array is empty. Docs [[sys-enemies]], [[sys-spawner]]. Tests `RogueRigs.Kamikaze.*`: arms within 150 uu and explodes after 0.5 s; killed by two shots it explodes and drops 1 XP; weighted spawning over 400 picks gives 20 to 30 % kamikazes.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-011-explosions]] — parent global task.
