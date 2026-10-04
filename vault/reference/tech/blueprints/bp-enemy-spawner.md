---
id: bp-enemy-spawner
description: "BP_EnemySpawner, Blueprint of ARREnemySpawner with EnemyClass BP_Chaser, placed once in L_ArenaM1; open for its settings."
parent: blueprints-index
kind: doc
---
# BP_EnemySpawner

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner
- parent_class: ARREnemySpawner (/Script/RogueRigs.RREnemySpawner)
- purpose: spawns chasers, kamikazes and shooters outside the screen ([[sys-spawner]]).
- components: inherited only.
- exposed_defaults: WeightedEnemyClasses = BP_Chaser_C 70, BP_Kamikaze_C 15, BP_Shooter_C 15 (task-021-02; before it 85 / 15; the placed instance inherits it); EnemyClass = BP_Chaser_C (fallback); other tunables at C++ defaults. One instance placed in L_ArenaM1 at (8100, 8100, 100) (location irrelevant).
- events: none
- logic_in_bp: none
- assets: none
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-spawner]] — the C++ class.
- [[bp-chaser]] — the enemy it spawns.
- [[bp-kamikaze]] — the other enemy it spawns.
- [[bp-shooter]] — the third enemy it spawns.
