---
id: task-414-03-doc-muzzle-flash
description: "Doc: create bp-muzzle-flash (fx index); update sys-weapons, sys-weapons-shotgun, bp-burst-rifle"
parent: task-414-muzzle-flash
kind: task
status: todo
---
# Muzzle flash on every miner shot: doc

- goal: Document task-414: create the doc node bp-muzzle-flash for NS_MuzzleFlash (template blueprint.md) under blueprints-fx-index and add its Children line; update sys-weapons (MuzzleFlashSystem property, spawn rule), sys-weapons-shotgun (one flash per volley) and bp-burst-rifle (its Muzzle now also hosts the flash, asset unchanged); facts only from the commits and change summaries.
- agent: doc
- depends_on: task-414-01-ns-muzzle-flash, task-414-02-spawn-muzzle-flash
- inputs: task-414-muzzle-flash (vault/reference/tasks/task-414-muzzle-flash.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-shooter-charge (vault/reference/tech/blueprints/fx/bp-shooter-charge.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), agent-doc (vault/reference/agents/agent-doc.md), vault/templates/blueprint.md
- expected_output: New vault/reference/tech/blueprints/fx/bp-muzzle-flash.md, updated index and doc nodes; lint clean; one commit `task-414-03-doc-muzzle-flash: <summary>` by path.
- done_when: bp-muzzle-flash exists with parent blueprints-fx-index and a Children line there; sys-weapons, sys-weapons-shotgun and bp-burst-rifle state the flash rule and commit hashes; `node vault/scripts/lint.js --write` clean; committed by path.
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
- [[task-414-muzzle-flash]] — parent global task.
