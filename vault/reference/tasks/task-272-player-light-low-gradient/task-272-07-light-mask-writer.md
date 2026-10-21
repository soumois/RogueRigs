---
id: task-272-07-light-mask-writer
description: "C++ writer that pushes the miner position and the mask tunables into MPC_PlayerLight each tick, without per-frame allocation."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Light mask writer

- goal: C++ writer that pushes the miner position and the mask tunables into MPC_PlayerLight each tick, without per-frame allocation.
- agent: cpp
- depends_on: task-272-06-light-mask-material
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-player (vault/reference/tech/systems/core/sys-player.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ component on ARRMiner writing MPC_PlayerLight each tick from tunable properties; sys-player doc updated.
- done_when: Build exits 0 with the editor closed; tests still pass; doc updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 56, cache write 98951, cache read 1570575, output 7303
- runs: 1
- milestone: M1
- result: Commits 3497c5a5 (code), 1397f817 (docs). URRPlayerLightMaskComponent (Source/RogueRigs/World/), default subobject LightMask on ARRMiner, ticks TG_PostUpdateWork, writes PlayerPosition and tunables (bMaskEnabled false, MaskRadius 220, MaskExponent 2, NearGain 0, FarGain 1, AddIntensity 0, NearColour, FarColour) into MPC_PlayerLight, cached instance, no allocation. Build OK; 22/22 tests (Weapons+Damage). Runtime: setnopec /Game/RogueRigs/Maps/L_ArenaM1.L_ArenaM1:PersistentLevel.BP_Miner_C_0.LightMask <Prop> <Value> works. Smoke frames r8/smoke_mask_on/off: uniform on all surfaces but centred about 300 px left and 100 px up of the miner: material reconstruction defect, fix in task-272-12.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
