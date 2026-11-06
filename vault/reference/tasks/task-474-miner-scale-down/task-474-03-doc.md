---
id: task-474-03-doc
description: "Doc: Docs record the new miner scale."
parent: task-474-miner-scale-down
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-474-miner-scale-down]] — parent global task.
