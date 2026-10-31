---
id: task-412-03-doc-shot-origin-muzzle
description: "Doc: sys-weapons, sys-weapons-shotgun, bp-burst-rifle, sys-automation-tests: shot origin at the rifle Muzzle"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: todo
---
# Shots leave from the rifle muzzle: doc

- goal: Update the doc nodes from the cpp change summary and commit: the shot origin rule (Muzzle X, Y at floor + ShotHeight, segment check, fallbacks), the shotgun origin, the Muzzle no longer visual only in bp-burst-rifle, the updated test expectations.
- agent: doc
- depends_on: task-412-01-shot-origin-muzzle
- inputs: task-412-shots-from-rifle-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle.md), agent-doc (vault/reference/agents/agent-doc.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: Updated doc nodes, lint clean, one commit `task-412-03-doc-shot-origin-muzzle: <summary>` by path.
- done_when: sys-weapons Shot origin and Hand socket entries, sys-weapons-shotgun, bp-burst-rifle (Muzzle used as shot origin X, Y) and sys-automation-tests state the new rule and the commit; no stale "visual only" or "shot origin unchanged" sentence left in them; sys-projectiles only if the projectile code changed; lint clean; committed by path.
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
