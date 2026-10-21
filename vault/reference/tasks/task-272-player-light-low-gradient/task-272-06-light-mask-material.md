---
id: task-272-06-light-mask-material
description: "Material Parameter Collection MPC_PlayerLight and post-process material M_PP_PlayerLightMask: radial light mask around the player, neutral by default."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
