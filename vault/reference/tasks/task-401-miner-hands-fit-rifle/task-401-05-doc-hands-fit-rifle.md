---
id: task-401-05-doc-hands-fit-rifle
description: "Doc: update bp-miner and sys-player-classes, record LeftHandGrip in burst-rifle-mesh"
parent: task-401-miner-hands-fit-rifle
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
