---
id: bp-enemy-spawner
description: "BP_EnemySpawner, Blueprint of ARREnemySpawner with EnemyClass BP_ChaserV2, placed once in L_ArenaM1; open for its settings."
parent: blueprints-world-index
kind: doc
---
# BP_EnemySpawner

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner
- parent_class: ARREnemySpawner (/Script/RogueRigs.RREnemySpawner)
- purpose: spawns chasers, kamikazes and shooters outside the screen ([[sys-spawner]]).
- components: inherited only.
- exposed_defaults: WeightedEnemyClasses = BP_ChaserV2_C 65 (task-284-10, provisional owner 2026-10-22; before BP_Chaser_C 65 same MaxAlive 0), BP_Kamikaze_C 12, BP_Shooter_C 15 (all MaxAlive 0), BP_Heavy_C 8 (MaxAlive 3, was 5 before task-045-02), BP_ShieldChaser_C 0 (MaxAlive 0, director only, task-028-02) (task-022-02; before it 70 / 15 / 15; the placed instance inherits it, its saved actor file has no override); EnemyClass = BP_ChaserV2_C (fallback; before BP_Chaser_C); other tunables at C++ defaults. One instance placed in L_ArenaM1 at (8100, 8100, 100) (location irrelevant).
- restore: to go back to the old chaser, set the first WeightedEnemyClasses entry class and EnemyClass back to /Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C (weight 65, MaxAlive 0 unchanged), compile, save. The placed L_ArenaM1 instance has no override of these fields (checked task-284-10).
- events: none
- logic_in_bp: none
- assets: none
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-spawner]] — the C++ class.
- [[bp-chaser]] — the enemy it spawns.
- [[bp-chaser-v2]] — the chaser it spawns since task-284-10.
- [[bp-kamikaze]] — the other enemy it spawns.
- [[bp-shooter]] — the third enemy it spawns.
- [[bp-shield-chaser]] — fifth entry, spawned by the director only.
- [[bp-heavy]] — the fourth enemy it spawns, capped at 3 alive.
