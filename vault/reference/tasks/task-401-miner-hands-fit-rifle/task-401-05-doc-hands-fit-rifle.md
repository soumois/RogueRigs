---
id: task-401-05-doc-hands-fit-rifle
description: "Doc: update bp-miner and sys-player-classes, record LeftHandGrip in burst-rifle-mesh"
parent: task-401-miner-hands-fit-rifle
kind: task
status: done
---
# Miner hands fitted to the burst rifle: doc

- goal: Doc nodes record the left hand IK target, the ABP_Miner IK node and the LeftHandGrip socket.
- agent: doc
- depends_on: task-401-01-left-hand-ik-target, task-401-02-rifle-left-grip-socket, task-401-03-abp-left-hand-ik
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md)
- expected_output: Updated doc nodes, lint clean, commit `task-401-05-doc-hands-fit-rifle: ...`.
- done_when: sys-player-classes describes LeftHandIKTarget and LeftHandIKAlpha; bp-miner describes the ABP_Miner Two Bone IK; burst-rifle-mesh lists LeftHandGrip; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 38714, cache read 401296, output 4267
- usage: turns 6, context start 59k peak 83k, resumes 0, 1 min, units 0.11 M
- runs: 1
- milestone: M2
- started: 2026-10-31T05:55:59Z
- waiting_for: none
- result: bp-miner, sys-player-classes, burst-rifle-mesh updated; lint OK; commit 91f89f3b.

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
