---
id: task-406-03-doc-upper-body-layer
description: "Doc: doc: bp-miner (AnimGraph with the upper body layer, how to swap the source) and sys-player-classes (UpperBodySequence)"
parent: task-406-miner-upper-body-idle-layer
kind: task
status: done
---
# Miner upper body from the idle rifle pose over the locomotion legs: doc

- goal: bp-miner and sys-player-classes describe the upper body layer, UpperBodySequence and how to swap the upper source for another weapon.
- agent: doc
- depends_on: task-406-01-upper-body-sequence-prop, task-406-02-abp-upper-body-layer
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Nodes updated, values left open, lint result, commit hash.
- done_when: bp-miner AnimGraph line shows the Layered blend per bone with its settings and the swap procedure (change UpperBodySequence in ABP_Miner class defaults); sys-player-classes lists UpperBodySequence; the PIE readings and unverified items from the bp summary are stated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 8, cache write 28459, cache read 64396, output 3493
- usage: turns 4, context start 11k peak 28k, resumes 0, 0 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-10-31T07:11:47Z
- waiting_for: none
- result: bp-miner and sys-player-classes updated (52670816); unverified items stated; lint clean.

## Relations
- [[task-406-miner-upper-body-idle-layer]] — parent global task.
