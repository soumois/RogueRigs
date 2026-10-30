---
id: task-401-02-rifle-left-grip-socket
description: "Bp: SM_BurstRifle socket LeftHandGrip on the handguard"
parent: task-401-miner-hands-fit-rifle
kind: task
status: todo
---
# Miner hands fitted to the burst rifle: bp

- goal: Add socket `LeftHandGrip` on SM_BurstRifle under the handguard, ahead of the trigger guard, where a left palm holds it (from the mesh bounds and a vertex read-back), values provisional.
- agent: bp
- depends_on: none
- inputs: burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Weapons/BurstRifle/SM_BurstRifle`
- expected_output: Socket saved; change summary with location and rotation and how they were derived; commit `task-401-02-rifle-left-grip-socket: ...`.
- done_when: SM_BurstRifle lists LeftHandGrip with the reported transform, inside the mesh bounds, under the barrel, X between the grip and the muzzle; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
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
