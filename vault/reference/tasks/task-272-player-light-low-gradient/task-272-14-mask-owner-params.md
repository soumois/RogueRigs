---
id: task-272-14-mask-owner-params
description: "MPC_PlayerLight and M_PP_PlayerLightMask rebuilt around the owner parameters: centre and far brightness, radius, exponent, warm tint."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Mask owner parameters

- goal: MPC_PlayerLight and M_PP_PlayerLightMask rebuilt around the owner parameters: centre and far brightness, radius, exponent, warm tint.
- agent: bp
- depends_on: task-272-13-mask-writer-position
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: MPC_PlayerLight with the owner parameters (Enable, CenterBrightness, FarBrightness, Radius uu, Exponent, WarmTint, FarTint, TintStrength) and M_PP_PlayerLightMask using them; doc with how to tune and revert.
- done_when: Parameters read back, material compiles, doc updated, lint clean, committed by path.
- editor_state: open
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
