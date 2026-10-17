---
id: task-221-01-xp-cube-import
description: "bp: import SM_XPCubeBig into Unreal and give it its own material instances matching the chip's look and glow."
parent: task-221-xp-cube-integration
kind: task
status: todo
---
# XP cube: import and materials

- goal: `/Game/RogueRigs/Art/XPCube/SM_XPCubeBig` with five slots set, ready for BP_PickupXPBig.
- agent: agent-bp
- depends_on: none
- inputs: `Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx` and `SM_XPCubeBig.report.json` (slots body #2D2C30, caps #868687, core_dark, core_bright, glow_disc; centred pivot; 748 + 16 tris); `vault/reference/tech/blueprints/fx/bp-pickup-xp.md` (chip materials: M_XPChip_* instances, emissive core instances, MI_XPGlowDisc_Chip, Lumen and distance field flags); `vault/reference/agents/bp-mcp-lessons.md`.
- method: import like SM_XPChipBig (centimetres, no x100, no collision, Nanite off, one LOD). New material instances in `/Game/RogueRigs/Art/XPCube/`, never depending on M_XPChip_* (they will be deleted): M_XPCube_body and M_XPCube_caps (parent M_GreyBox, linear colours from the report), M_XPCube_core_dark and M_XPCube_core_bright (same parent and parameter values as M_XPChip_core_dark and M_XPChip_core_bright, so the glow is unchanged); slot 4 glow_disc = MI_XPGlowDisc_Chip (kept, not chip-named). Set them as mesh slot defaults. Same mesh flags as SM_XPChipBig (bGenerateMeshDistanceField true).
- done_when: assets saved; slot materials and flags read back.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-221-xp-cube-integration]] — parent global task.
