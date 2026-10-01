---
id: sys-difficulty
description: "Difficulty as built: alert and threat counters per level, URRGE_EnemyDifficulty applied at spawn from a table indexed by run level and threat."
parent: systems-index
kind: doc
---
# Difficulty

- responsibility: level counters and enemy strength at spawn.
- main_classes: `URRGE_EnemyDifficulty`; counter owner: open (run flow system, not written yet).

## Rules
- Alert: grows with time and kills, pauses while a mandatory objective is pending, fires tier events.
- Threat: grows with time and kills, never pauses.
- At spawn, `URRGE_EnemyDifficulty` applies `MultiplyAdditive` bonuses on `MaxHealth` and `AttackPower`; magnitudes come from a table indexed by (run level, threat). Robots already alive are not updated.
- Enemy attack damage = `AttackDamage` (CSV) × `AttackPower`.
- Spawn rate also follows threat.

- data: difficulty table CSV; alert and threat gains (open).
- depends_on: [[sys-gas-core]].
- implements: [[difficulty]], [[level-flow]].
- gotchas: set `MaxHealth` before `Health` when initializing a spawned robot.

## Relations
- [[systems-index]] — parent index.
