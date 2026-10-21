---
id: task-274-02-darkener-material
description: "M_PP_PlayerLightMask outputs SceneColor x lerp(FarBrightness, 1, mask); MPC_PlayerLight keeps Enable, FarBrightness, Radius, Exponent, PlayerPosition; tint and CenterBrightness removed."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
---
# Darkening-only mask material

- goal: M_PP_PlayerLightMask outputs SceneColor x lerp(FarBrightness, 1, mask); MPC_PlayerLight keeps Enable, FarBrightness, Radius, Exponent, PlayerPosition; tint and CenterBrightness removed.
- agent: bp
- depends_on: task-274-01-mask-writer-cleanup
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Material and MPC changed and saved; doc with removed parameters and previous values.
- done_when: Material compiles; MPC parameter list and defaults read back (Enable 1, FarBrightness 0.005, Radius 200, Exponent 3); no other asset touched; saved by path; doc updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: sonnet
- tokens: input 36, cache write 49111, cache read 509738, output 735
- runs: 1
- milestone: M1
- result: Commit 2e5d5ae9: M_PP_PlayerLightMask Custom HLSL now gain = lerp(FarBrightness, 1, mask), out = lerp(SceneColor, SceneColor*gain, saturate(Enable)); CenterBrightness, TintStrength, WarmTint, FarTint nodes and MPC parameters removed (previous 30, 1, (1,0.9,0.06), (1,0.7,0.55) in the doc); MPC read back Enable 1, FarBrightness 0.15 -> 0.005, Radius 200, Exponent 3; compiled, saved, not dirty; doc rewritten.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
