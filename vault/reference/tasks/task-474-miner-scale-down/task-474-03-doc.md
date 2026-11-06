---
id: task-474-03-doc
description: "Doc: Docs record the new miner scale."
parent: task-474-miner-scale-down
kind: task
status: done
---
# Miner 10 % smaller: doc

- goal: Docs record the new miner scale.
- agent: doc
- depends_on: task-474-02-capture
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), miner (vault/reference/gdd/player/miner.md)
- expected_output: see done_when
- done_when: bp-miner and miner record the 90 % scale with date and revert value; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-474
- usage: included in the executor run of task-474
- runs: 1
- milestone: M2
- started: 2026-11-06T17:15:31Z
- waiting_for: none
- result: Commit of the doc: see git log (task-474-03-doc). bp-miner: new section Mesh scale 0.9 (170 -> 153 uu, unchanged items, follows, capture paths, revert RelativeScale3D 1); miner gdd size line gets heights and revert value 1 with a bp-miner relation; sys-player-classes: hand weapon attach SnapToTargetIncludingScale since task-474. Lint clean.

## Relations
- [[task-474-miner-scale-down]] — parent global task.
