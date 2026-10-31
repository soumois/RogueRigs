---
id: task-401-02-rifle-left-grip-socket
description: "Bp: SM_BurstRifle socket LeftHandGrip on the handguard"
parent: task-401-miner-hands-fit-rifle
kind: task
status: done
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
- model: sonnet
- tokens: input 72, cache write 101905, cache read 3916206, output 13184
- usage: turns 35, context start 58k peak 146k, resumes 0, 3 min, units 0.58 M
- runs: 1
- milestone: M2
- started: 2026-10-31T05:38:17Z
- waiting_for: none
- result: SM_BurstRifle socket LeftHandGrip at (22,0,0) cm rot 0 from Blender vertex read-back (handguard underside, x 14-27); set via Static Mesh Editor UI; saved transform not read back by API (unverified, capture will judge); commit aef312e9.

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
