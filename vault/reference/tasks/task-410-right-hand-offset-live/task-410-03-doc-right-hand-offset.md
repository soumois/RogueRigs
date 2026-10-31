---
id: task-410-03-doc-right-hand-offset
description: "Doc: sys-player-classes, bp-miner, bp-burst-rifle: right hand offset, cvar, counter-move, how to keep values"
parent: task-410-right-hand-offset-live
kind: task
status: todo
---
# Live control of the right hand offset on the rifle grip: doc

- goal: Document the right hand offset from the cpp and bp change summaries: sys-player-classes (properties, cvar, ABP inputs, rifle counter-move, left hand refresh after a change); bp-miner (the two right arm nodes, the tuning recipe: cvar in PIE, read the log, type the values in ABP_Miner Class Defaults; offset 0 = V2Grip); bp-burst-rifle (RifleMesh and Muzzle are moved at runtime by the inverse offset, asset values stay (0, 0, 0) and (52.25, 0, 0)).
- agent: doc
- depends_on: task-410-01-right-hand-offset-code, task-410-02-abp-right-arm-offset
- inputs: task-410-right-hand-offset-live (vault/reference/tasks/task-410-right-hand-offset-live.md), agent-doc (vault/reference/agents/agent-doc.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md)
- expected_output: Updated vault/reference/tech/systems/core/sys-player/sys-player-classes.md, vault/reference/tech/blueprints/player/bp-miner.md, vault/reference/tech/blueprints/player/bp-burst-rifle.md; one commit `task-410-03-doc-right-hand-offset: <summary>`.
- done_when: The three nodes name the properties, the cvar with syntax, units and axes, the ABP nodes, the counter-move, the reach risk and how to keep a value; unverified items stated (no PIE run by the crew); lint clean; committed by path.
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
- [[task-410-right-hand-offset-live]] — parent global task.
