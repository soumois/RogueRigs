---
id: task-272-12-light-mask-centre-fix
description: "M_PP_PlayerLightMask world position fixed so the mask is centred on the miner (smoke frames showed it 300 px off to the left)."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Light mask centre fix

- goal: M_PP_PlayerLightMask world position fixed so the mask is centred on the miner (smoke frames showed it 300 px off to the left).
- agent: bp
- depends_on: task-272-07-light-mask-writer
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Material graph with the engine world position reconstruction; doc updated.
- done_when: Material compiles, graph read back, doc updated, committed by path; centring checked by the next sweep (task-272-09).
- editor_state: open
- token_ceiling: 40k
- model: sonnet
- tokens: input 50, cache write 62394, cache read 1021964, output 1749
- runs: 1
- milestone: M1
- result: Commit be12cbd7. Custom HLSL reconstruction (SvPosition, SceneDepth, SVPositionToTranslatedWorld) replaced by the engine Absolute World Position node (valid in post process: PostProcessMaterialShaders.usf sets SvPosition.z to scene depth); SceneDepth input removed; formula, neutral defaults and Blendable Location unchanged; compiles, no shader error logged. Centring checked in task-272-09.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
