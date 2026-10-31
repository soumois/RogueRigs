---
id: task-400-07-doc-burst-rifle-in-hand
description: "Doc: update bp-burst-rifle, bp-shotgun, bp-miner, sys-weapons, sys-player-classes"
parent: task-400-burst-rifle-in-hand
kind: task
status: done
---
# Burst rifle 3D mesh in the miner right hand: doc

- goal: Doc nodes record the per-weapon hand socket, the imported rifle assets, WeaponSocket_R and the new BP_BurstRifle defaults and BP_Shotgun without its cube.
- agent: doc
- depends_on: task-400-01-weapon-hand-socket, task-400-02-burst-rifle-import, task-400-03-miner-hand-socket, task-400-04-bp-burst-rifle-mesh, task-400-05-shotgun-cube-removal
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-shotgun (vault/reference/tech/blueprints/player/bp-shotgun.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md)
- expected_output: Updated doc nodes, lint clean, commit `task-400-07-doc-burst-rifle-in-hand: ...`.
- done_when: bp-burst-rifle names SM_BurstRifle, Muzzle position and socket; bp-shotgun no longer lists ShotgunMesh; bp-miner names WeaponSocket_R; sys-weapons and sys-player-classes describe the per-weapon socket; burst-rifle-mesh lists the Unreal asset paths; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 14, cache write 36821, cache read 177409, output 9726
- usage: turns 7, context start 13k peak 40k, resumes 0, 1 min, units 0.11 M
- runs: 1
- milestone: M2
- started: 2026-10-31T02:40:55Z
- waiting_for: none
- result: 7 doc nodes updated (bp-burst-rifle, bp-shotgun, bp-miner, sys-weapons, sys-player-classes, burst-rifle-mesh, unreal-mcp-quirks). Commit db07da46. Doc agent wrote from summaries, no git show; colour/C4/C5 recorded as open observations

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
