---
id: task-426-01-radial-mask-material
description: "Bp: M_PP_PlayerLightMask and MPC_PlayerLight: screen radial darkener"
parent: task-426-screen-radial-darkener
kind: task
status: in-progress
---
# Darkener shader: circles around the screen centre, min at centre, max at corners: bp

- goal: Rewrite the Custom HLSL of M_PP_PlayerLightMask and the parameters of MPC_PlayerLight exactly as the global goal says, with the values decided in Q6 to Q9 (defaults: CenterDarkness 0, CornerDarkness 0.85, VoidMaxDarkness 0.5, VoidZ -200, Enable 1, linear, true circles). Screen UV and view size from the post-process nodes (ScreenPosition / ViewSize). Keep the blendable in PPV_XPGlow. Compile, save, read the defaults back; record the old formula and old defaults for revert.
- agent: bp
- depends_on: none
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), vault/reference/tasks/task-426-screen-radial-darkener.md (goal, Questions)
- expected_output: M_PP_PlayerLightMask.uasset and MPC_PlayerLight.uasset changed, compiled without error, defaults read back, change summary with revert values, commit.
- done_when: Material compiles without error; MPC holds Enable, VoidZ, PlayerPosition, CenterDarkness, CornerDarkness, VoidMaxDarkness with the decided defaults and no FarBrightness, VoidFarBrightness, Radius, Exponent; the HLSL no longer reads PlayerPosition; committed.
- editor_state: Unreal editor open with the MCP server.
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-04T17:03:55Z
- waiting_for: none
- result: open

## Relations
- [[task-426-screen-radial-darkener]] — parent global task.
