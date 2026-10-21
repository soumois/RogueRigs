---
id: task-272-14-mask-owner-params
description: "MPC_PlayerLight and M_PP_PlayerLightMask rebuilt around the owner parameters: centre and far brightness, radius, exponent, warm tint."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: sonnet
- tokens: input 36, cache write 65329, cache read 777789, output 1881
- runs: 1
- milestone: M1
- result: Commit ac259849. MPC_PlayerLight: Enable 0, CenterBrightness 3, FarBrightness 0.4, Radius 225 uu, Exponent 2, TintStrength 1, PlayerPosition, WarmTint (1, 0.76, 0.62), FarTint (0.85, 0.55, 0.9); old parameters removed. M_PP_PlayerLightMask HLSL: mask = 1/(1+(r/Radius)^Exponent), gain = Far + (Center - Far) * mask, tint = lerp(1, lerp(FarTint, WarmTint, mask), TintStrength), out = lerp(scene, scene * gain * tint, Enable). Compiles, no log errors. Doc mpp-player-light-mask rewritten as the owner guide.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
