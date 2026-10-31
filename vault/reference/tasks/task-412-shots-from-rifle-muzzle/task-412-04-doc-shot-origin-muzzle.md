---
id: task-412-04-doc-shot-origin-muzzle
description: "Doc: sys-projectiles, sys-weapons, sys-weapons-shotgun, bp-burst-rifle, bp-projectile, bp-shotgun, bp-turret, bp-shooter, sys-automation-tests"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: todo
---
# Shots leave from the rifle muzzle: doc

- goal: Update the doc nodes from the cpp and bp change summaries and commit: capsule collision root and per-shot sizing, shot origin at the Muzzle, Q3 sweep, fallbacks, shotgun origin, Muzzle no longer visual only, the four projectile Blueprints, updated test expectations.
- agent: doc
- depends_on: task-412-01-shot-origin-muzzle-capsule, task-412-02-bp-projectiles-recompile
- inputs: task-412-shots-from-rifle-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle.md), agent-doc (vault/reference/agents/agent-doc.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-projectile (vault/reference/tech/blueprints/player/bp-projectile.md), bp-shotgun (vault/reference/tech/blueprints/player/bp-shotgun.md), bp-turret (vault/reference/tech/blueprints/player/bp-turret.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: Updated doc nodes, lint clean, one commit `task-412-04-doc-shot-origin-muzzle: <summary>` by path.
- done_when: Every listed node states the new rule and the commits; no stale "CollisionSphere", "visual only" or "shot origin unchanged" sentence left in them; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-412-shots-from-rifle-muzzle]] — parent global task.
