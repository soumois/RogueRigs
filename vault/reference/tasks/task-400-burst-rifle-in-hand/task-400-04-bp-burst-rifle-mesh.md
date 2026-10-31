---
id: task-400-04-bp-burst-rifle-mesh
description: "Bp: BP_BurstRifle shows SM_BurstRifle, Muzzle at the tip, hand socket WeaponSocket_R"
parent: task-400-burst-rifle-in-hand
kind: task
status: done
---
# Burst rifle 3D mesh in the miner right hand: bp

- goal: BP_BurstRifle: RifleMesh uses SM_BurstRifle at scale 1 and zero offset (cube removed), NoCollision, no shadow change; Muzzle moved to the mesh muzzle tip (about X 41.8); the new hand socket property set to WeaponSocket_R. Shotgun and turret launcher untouched.
- agent: bp
- depends_on: task-400-01-weapon-hand-socket, task-400-02-burst-rifle-import, task-400-03-miner-hand-socket
- inputs: bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle`
- expected_output: BP_BurstRifle compiled and saved; change summary; a PIE read-back of the rifle parent socket and world location; commit `task-400-04-bp-burst-rifle-mesh: ...`.
- done_when: BP_BurstRifle compiles with SM_BurstRifle on RifleMesh and the socket property WeaponSocket_R; in PIE the spawned BP_BurstRifle_C is attached to the miner mesh at WeaponSocket_R and projectiles still spawn at about 50 uu height; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 48, cache write 44828, cache read 704999, output 12903
- usage: turns 24, context start 12k peak 50k, resumes 0, 3 min, units 0.19 M
- runs: 1
- milestone: M2
- started: 2026-10-31T00:09:39Z
- waiting_for: none
- result: BP_BurstRifle: RifleMesh=SM_BurstRifle scale 1 offset 0, NoCollision profile, Muzzle (41.8,0,0), HandSocketName=WeaponSocket_R. PIE: parent miner CharacterMesh0, world scale 1, bounds 60x7.3x20.7, projectiles z=50. Unverified: AttachSocketName readback, visual look, collisionEnabled field stale. Commit a78a8be5

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
