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
- exposed_defaults: WeightedEnemyClasses = BP_Chaser_C 65, BP_Kamikaze_C 12, BP_Shooter_C 15 (all MaxAlive 0), BP_Heavy_C 8 (MaxAlive 5) (task-022-02; before it 70 / 15 / 15; the placed instance inherits it, its saved actor file has no override); EnemyClass = BP_Chaser_C (fallback); other tunables at C++ defaults. One instance placed in L_ArenaM1 at (8100, 8100, 100) (location irrelevant).
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
- [[bp-heavy]] — the fourth enemy it spawns, capped at 5 alive.
