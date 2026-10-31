---
id: task-410-03-doc-right-hand-offset
description: "Doc: sys-player-classes and bp-miner: right hand offset properties, cvar, ABP right arm nodes, how to keep values"
parent: task-410-right-hand-offset-live
kind: task
status: done
---
# Live control of the right hand offset: doc

- goal: Document the right hand offset from the cpp and bp change summaries: sys-player-classes (properties, cvar, ABP inputs, frame and pivot, no left hand refresh needed); bp-miner (the two right arm nodes in the AnimGraph order, the tuning recipe: cvar in PIE, read the log, type the values in ABP_Miner Class Defaults; offset 0 = V2Grip; the rifle follows the hand, owner Q1).
- agent: doc
- depends_on: task-410-01-right-hand-offset-code, task-410-02-abp-right-arm-offset
- inputs: task-410-right-hand-offset-live (vault/reference/tasks/task-410-right-hand-offset-live.md), agent-doc (vault/reference/agents/agent-doc.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md)
- expected_output: Updated vault/reference/tech/systems/core/sys-player/sys-player-classes.md and vault/reference/tech/blueprints/player/bp-miner.md; one commit `task-410-03-doc-right-hand-offset: <summary>`.
- done_when: The two nodes name the properties, the cvar with syntax, units and axes, the ABP nodes, that the rifle and the left hand follow the right hand, the reach risk and how to keep a value; unverified items stated (no PIE run by the crew); lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 32024, cache read 97766, output 3774
- usage: turns 5, context start 12k peak 32k, resumes 0, 0 min, units 0.07 M
- runs: 1
- milestone: M2
- started: 2026-10-31T20:21:24Z
- waiting_for: none
- result: sys-player-classes and bp-miner updated, commit e567cab9; lint OK

## Relations
- [[task-410-right-hand-offset-live]] — parent global task.
