---
id: task-405-02-doc-right-elbow-raise
description: "Doc: update bp-miner with the right elbow adjustment and how to tune it"
parent: task-405-right-elbow-raise
kind: task
status: cancelled
---
# Miner right elbow raised a little, owner tunable: doc

- goal: Record in bp-miner the right elbow adjustment (method, node order, RightElbowOffset default and axes) and how the owner tunes it.
- agent: doc
- depends_on: task-405-01-abp-right-elbow-ik
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Global request and gdd_check of task-405-right-elbow-raise
- expected_output: bp-miner updated; lint clean; commit `task-405-02-doc-right-elbow-raise: ...`.
- done_when: bp-miner states the right elbow node, its place in the AnimGraph, RightElbowOffset (default, provisional owner 2026-10-31, axis meaning) and the tuning step; lint clean; committed by path.
- editor_state: closed
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
- [[task-405-right-elbow-raise]] — parent global task.
