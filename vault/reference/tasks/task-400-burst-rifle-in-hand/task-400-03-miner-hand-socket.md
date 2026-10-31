---
id: task-400-03-miner-hand-socket
description: "Bp: SKEL_Miner socket WeaponSocket_R on the right hand bone"
parent: task-400-burst-rifle-in-hand
kind: task
status: in-progress
---
# Burst rifle 3D mesh in the miner right hand: bp

- goal: Add socket `WeaponSocket_R` on the right hand bone of SKEL_Miner (Q4): the rifle pivot (pistol grip) in the palm, rifle +X along the miner forward and +Z up in Rifle Aiming Idle; offsets computed from the bone axes read back, values provisional.
- agent: bp
- depends_on: none
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), miner (vault/reference/gdd/player/miner.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Characters/Miner/SKEL_Miner`, `/Game/RogueRigs/Characters/Miner/Anims/A_Miner_Idle`
- expected_output: Socket saved on SKEL_Miner; change summary with bone name, relative location and rotation, and how they were derived; commit `task-400-03-miner-hand-socket: ...`.
- done_when: SKEL_Miner lists WeaponSocket_R on the right hand bone with the reported transform; in the A_Miner_Idle pose its X axis points within 15 degrees of the mesh forward (read back by Python); committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T00:03:55Z
- waiting_for: none
- result: open

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
