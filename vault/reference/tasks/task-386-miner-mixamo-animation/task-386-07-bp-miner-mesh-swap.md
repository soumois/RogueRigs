---
id: task-386-07-bp-miner-mesh-swap
description: "Bp: BP_Miner: SK_Miner and ABP_Miner_C instead of SKM_Quinn_Simple and ABP_Unarmed, feet on the floor, facing the aim"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: bp

- goal: On BP_Miner set Mesh SkeletalMeshAsset = SK_Miner and AnimClass = ABP_Miner_C, and fit the mesh relative location and yaw so the feet touch the capsule bottom and the mesh faces the actor forward (the aim); capsule, weapons and every other default unchanged.
- agent: bp
- depends_on: task-386-03-miner-skeletal-import, task-386-06-abp-miner
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player (vault/reference/tech/systems/core/sys-player.md), owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), current values in bp-miner: location (0, 0, -95), yaw -90 for Quinn; capsule half height 88; the new values come from SK_Miner bounds (pivot at the feet expected) and its forward axis read in the editor; Quinn assets stay in the project.
- expected_output: BP_Miner compiled and saved; change summary (mesh, anim class, location, rotation, how the forward axis was checked) for the doc step.
- done_when: read back on the BP_Miner CDO: SkeletalMeshAsset SK_Miner, AnimClass ABP_Miner_C, AnimationMode AnimationBlueprint, the new location and yaw; capsule 88 / 34, WeaponAttachSocketName None and WeaponAttachOffset (0, 20, -40) unchanged; a PIE start on L_ArenaM1 logs no Blueprint runtime error and no animation warning for BP_Miner; committed by path as `task-386-07-bp-miner-mesh-swap: <summary>`.
- editor_state: open
- token_ceiling: 0.5 M units
- model: sonnet
- tokens: input 42, cache write 64934, cache read 909193, output 9911
- usage: turns 21, context start 12k peak 70k, resumes 0, 2 min, units 0.22 M
- runs: 1
- milestone: M2
- started: 2026-10-30T05:53:06Z
- waiting_for: none
- result: BP_Miner CharacterMesh0: SK_Miner, ABP_Miner_C, location (0,0,-88), yaw -90; capsule, weapon attach unchanged; PIE 6 s on L_ArenaM1 no error/animation warning; facing INFERRED not measured (if -X, yaw +90), feet fit not viewed; commit 46f17540

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
