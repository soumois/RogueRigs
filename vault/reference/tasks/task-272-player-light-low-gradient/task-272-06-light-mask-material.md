---
id: task-272-06-light-mask-material
description: "Material Parameter Collection MPC_PlayerLight and post-process material M_PP_PlayerLightMask: radial light mask around the player, neutral by default."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Light mask material

- goal: Material Parameter Collection MPC_PlayerLight and post-process material M_PP_PlayerLightMask: radial light mask around the player, neutral by default.
- agent: bp
- depends_on: none
- inputs: ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: MPC_PlayerLight and M_PP_PlayerLightMask in /Game/RogueRigs/Materials/PostProcess/, neutral by default; doc node in tech/blueprints/world/.
- done_when: Assets exist, compile without error, parameters and defaults read back; doc node; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: sonnet
- tokens: input 76, cache write 107883, cache read 2663177, output 2124
- runs: 1
- milestone: M1
- result: Commit 6b27761f. MPC_PlayerLight (MaskRadius 220, MaskExponent 2, NearGain 0, FarGain 1, AddIntensity 0, MaskEnabled 0, PlayerPosition, NearColour (1, 0.76, 0.62), FarColour white) and M_PP_PlayerLightMask (post process, Blendable Location Scene Color Before Bloom because MCP coerces Before Tonemapping; one Custom HLSL node rebuilding world position from SceneDepth; neutral by default) in /Game/RogueRigs/Materials/PostProcess/; doc mpp-player-light-mask. Compiled without logged errors; world position reconstruction not verified at runtime.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
