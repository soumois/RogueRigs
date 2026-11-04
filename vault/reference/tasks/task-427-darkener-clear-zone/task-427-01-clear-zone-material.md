---
id: task-427-01-clear-zone-material
description: "Bp: M_PP_PlayerLightMask and MPC_PlayerLight: ClearRadius and FadeRadius"
parent: task-427-darkener-clear-zone
kind: task
status: todo
---
# Darkener shader: clear zone around the screen centre with a transition radius: bp

- goal: Add ClearRadius and FadeRadius scalars to MPC_PlayerLight with the decided defaults (Q13; default 0.15 and 0.35) and extend the Custom HLSL of M_PP_PlayerLightMask exactly as the global goal says, with the choices of Q11, Q12, Q14, Q15 (defaults: share unit, no remap, smoothstep). Keep every existing parameter, PlayerPosition included, and the blendable in PPV_XPGlow. Compile, save, read the defaults back; record the task-426 HLSL for revert.
- agent: bp
- depends_on: none
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), vault/reference/tasks/task-427-darkener-clear-zone.md (goal, Questions)
- expected_output: M_PP_PlayerLightMask.uasset and MPC_PlayerLight.uasset changed, compiled without error, defaults read back, change summary with the revert HLSL, commit.
- done_when: Material compiles without error; MPC holds Enable, VoidZ, PlayerPosition, CenterDarkness, CornerDarkness, VoidMaxDarkness unchanged plus ClearRadius and FadeRadius with the decided defaults; the HLSL applies the clear-zone mask before the void cap; committed.
- editor_state: Unreal editor open with the MCP server.
- token_ceiling: 0.8 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-427-darkener-clear-zone]] — parent global task.
