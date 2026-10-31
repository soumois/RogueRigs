---
id: task-404-02-doc-rifle-mesh-offset
description: "Doc: update bp-burst-rifle (RifleMesh transform, Muzzle), sys-player-classes and bp-miner (IK refresh limit)"
parent: task-404-rifle-mesh-owner-offset
kind: task
status: in-progress
---
# Burst rifle mesh at the owner position and scale 1.25: doc

- goal: Document the new RifleMesh relative transform and Muzzle in bp-burst-rifle (owner values 2026-10-31) and, in sys-player-classes and bp-miner, that LeftHandIKTarget is computed only at weapon add (a live RifleMesh or LeftHandGrip edit in PIE is not followed; a saved value is, at the next spawn), plus where the left hand is set (LeftHandGrip) versus the left elbow pole (Joint Target).
- agent: doc
- depends_on: task-404-01-rifle-mesh-transform
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Global request, gdd_check and Questions of task-404-rifle-mesh-owner-offset
- expected_output: bp-burst-rifle, sys-player-classes and bp-miner updated; lint clean; commit `task-404-02-doc-rifle-mesh-offset: ...`.
- done_when: bp-burst-rifle gives RifleMesh location (9.898091, -1.129037, -13.0), scale 1.25 (owner 2026-10-31) and the Muzzle value; sys-player-classes states the refresh-at-weapon-add limit (or the Q5 B outcome); bp-miner states that LeftHandGrip sets the left hand and Joint Target is the left elbow pole, alpha 0 in the ABP preview; lint clean; committed by path.
- editor_state: closed
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T06:25:41Z
- waiting_for: none
- result: open

## Relations
- [[task-404-rifle-mesh-owner-offset]] — parent global task.
