---
id: task-045-02-bp-heavy-cap
description: "BP_EnemySpawner WeightedEnemyClasses: BP_Heavy MaxAlive 5 → 3."
parent: task-045-easier-first-level
kind: task
status: in-progress
---
# Heavy alive cap in the spawner Blueprint

- goal: at most 3 heavies alive.
- agent: bp
- depends_on: task-045-01-easier-first-level
- inputs: bp-enemy-spawner (vault/reference/tech/blueprints/bp-enemy-spawner.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: BP_EnemySpawner class defaults, WeightedEnemyClasses entry BP_Heavy: MaxAlive 5 → 3; check the spawner placed in L_ArenaM1 does not override WeightedEnemyClasses (if it does, set the same value there); save; doc bp-enemy-spawner.
- done_when: value read back 3 on the Blueprint (and the placed actor); docs updated; lint clean.
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-045-easier-first-level]] — parent global task.
