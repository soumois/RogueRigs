---
id: task-405-02-doc-left-elbow-joint-target
description: "Doc: update bp-miner left hand IK section with the cause and the fix"
parent: task-405-left-elbow-joint-target
kind: task
status: todo
---
# Left elbow follows the Two Bone IK joint target: doc

- goal: Record in bp-miner (Left hand IK section) the cause of the no-effect joint target, the fix and how the owner tunes the elbow.
- agent: doc
- depends_on: task-405-01-joint-target-diagnose-fix
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Global request and gdd_check of task-405-left-elbow-joint-target
- expected_output: bp-miner updated; lint clean; commit `task-405-02-doc-left-elbow-joint-target: ...`.
- done_when: bp-miner Left hand IK section states the cause, the changed node settings and the tuning step; lint clean; committed by path.
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
- [[task-405-left-elbow-joint-target]] — parent global task.
