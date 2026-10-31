---
id: task-404-01-rifle-mesh-transform
description: "Bp: BP_BurstRifle: RifleMesh at the owner location and scale 1.25, Muzzle at the scaled tip"
parent: task-404-rifle-mesh-owner-offset
kind: task
status: todo
---
# Burst rifle mesh at the owner position and scale 1.25: bp

- goal: In BP_BurstRifle set RifleMesh relative location (9.898091, -1.129037, -13.0) and relative scale (1.25, 1.25, 1.25), rotation unchanged; set Muzzle relative location (62.148091, -1.129037, -13.0) (Q1 default; follow the Q1 answer in the global request); nothing else changes (HandSocketName, ProjectileClass, materials kept).
- agent: bp
- depends_on: none
- inputs: bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle`, Global request and Questions of task-404-rifle-mesh-owner-offset for the decided values
- expected_output: BP_BurstRifle compiled and saved; change summary with the read-back transforms; PIE read-back of projectile spawn z and of the left hand to LeftHandGrip distance; commit `task-404-01-rifle-mesh-transform: ...`.
- done_when: Read-back: RifleMesh location (9.898091, -1.129037, -13.0) within 0.001, scale 1.25 on each axis, rotation unchanged; Muzzle at the Q1 value; BP compiles; PIE on L_ArenaM1: projectiles spawn at z 50 as before; left hand bone within 3 uu of the rifle LeftHandGrip standing; committed by path.
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
- [[task-404-rifle-mesh-owner-offset]] — parent global task.
