---
id: task-272-08-light-mask-ppv
description: "M_PP_PlayerLightMask added to the PPV_XPGlow blendables (neutral until the mask is tuned)."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: sonnet
- tokens: input 24, cache write 48441, cache read 433106, output 1247
- runs: 1
- milestone: M1
- result: Commit 6e1deae5. PPV_XPGlow WeightedBlendables empty -> [M_PP_PlayerLightMask, weight 1]; bloom 1.3/-1 and exposure 1.8 read back unchanged; docs ppv-xp-glow, mpp-player-light-mask updated (revert: remove the entry). Neutral until the MPC is enabled.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
