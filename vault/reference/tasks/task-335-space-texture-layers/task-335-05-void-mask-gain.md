---
id: task-335-05-void-mask-gain
description: "M_PP_PlayerLightMask: pixels below world Z -200 (void background) use their own far gain 0.5 instead of 0.08; floor and units unchanged."
parent: task-335-space-texture-layers
kind: task
status: in-progress
---
# Void: own far gain in the player light mask

- goal: the darkening mask dims the void background to at most half far from the miner (B2), while the floor, blocks and units keep FarBrightness 0.08.
- agent: bp
- depends_on: task-335-04-space-layers
- inputs: the global `request` and `feedback_2` with B2 (vault/reference/tasks/task-335-space-texture-layers.md); mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md, parameters, Custom HLSL formula, C++ writer names); ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md, where the blendable sits); bp-space-background (vault/reference/tech/blueprints/world/bp-space-background.md, plane depths: the opaque stars plane at Z -12000 gives the scene depth under the additive layers); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: MPC_PlayerLight gets two scalars, VoidFarBrightness 0.5 and VoidZ -200 (uu); the Custom HLSL of M_PP_PlayerLightMask uses `far = WorldPos.z < VoidZ ? VoidFarBrightness : FarBrightness` in place of FarBrightness, the rest of the formula unchanged; existing parameter defaults unchanged (Enable 1, FarBrightness 0.08, Radius 200, Exponent 1.8). No C++ change (the C++ writer does not touch the new names). Change summary for the doc subtask: new parameters, formula before and after, revert, commit hash. No vault doc node.
- done_when: both assets compile with no log error and are saved; the MPC parameters read back; the formula reads back; a PIE or editor view check states whether floor pixels and the arena cliff faces stay on FarBrightness (cliff faces below Z -200, if any, reported, not fixed); only MPC_PlayerLight and M_PP_PlayerLightMask change in `git status`; committed by explicit path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T22:50:34Z
- waiting_for: none
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
