---
id: task-272-08-light-mask-ppv
description: "M_PP_PlayerLightMask added to the PPV_XPGlow blendables (neutral until the mask is tuned)."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Light mask in the volume

- goal: M_PP_PlayerLightMask added to the PPV_XPGlow blendables (neutral until the mask is tuned).
- agent: bp
- depends_on: task-272-06-light-mask-material (run before 07 while the editor is open; the mask is neutral by default)
- inputs: ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: PPV_XPGlow blendables hold M_PP_PlayerLightMask; doc updated.
- done_when: Blendable read back; level saved; doc updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
