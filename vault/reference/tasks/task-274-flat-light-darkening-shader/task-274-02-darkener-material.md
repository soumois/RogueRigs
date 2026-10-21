---
id: task-274-02-darkener-material
description: "M_PP_PlayerLightMask outputs SceneColor x lerp(FarBrightness, 1, mask); MPC_PlayerLight keeps Enable, FarBrightness, Radius, Exponent, PlayerPosition; tint and CenterBrightness removed."
parent: task-274-flat-light-darkening-shader
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
