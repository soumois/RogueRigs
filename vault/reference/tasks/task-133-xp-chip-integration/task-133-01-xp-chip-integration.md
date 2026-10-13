---
id: task-133-01-xp-chip-integration
description: "Import the big XP chip mesh into Unreal, give it a glowing magenta core with a light pulse, use it in BP_PickupXPBig."
parent: task-133-xp-chip-integration
kind: task
status: in-progress
---
# Big XP chip integration in Unreal

- goal: Import the big XP chip mesh into Unreal, give it a glowing magenta core with a light pulse, use it in BP_PickupXPBig.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: SM_XPChipBig imported (cm, bottom-centre pivot); seven material slots board, casing, casing_chamfer, edge_band, legs, core_bright, core_dark: flat greys from the FBX report for the six non-emissive ones, core_bright emissive magenta (1, 0.2, 0.8) GlowAmount 4, core_dark emissive darker (0.69, 0.06, 0.5) GlowAmount 2 to keep the facet relief, same pulse; BP_PickupXPBig PickupMesh = SM_XPChipBig instead of the cube, root scale 0.54 stays unchanged only if the 30 cm footprint matches the old cube at 0.3, so the scale becomes 1.8 relative to the nut (check the pool restoring the scale 0.54 in sys-pickups-xp and keep it consistent); MI_PickupXPBig replaced by the slot materials; the bp-pickup-xp doc node updated (BP_PickupXPBig section). Provisional values (owner default 2026-10-13, to judge in game): emissive pulse of plus or minus 15 percent of the emissive strength, period 1.2 s, phase from the actor world position so pickups do not blink together, driven by a time-based material parameter (no tick, no Blueprint logic); no ground glow disc, no light component, no shadow-casting, no collision change; check whether bloom is enabled in the level post-process and REPORT the result without changing it; do not call CaptureEditorImage, Screenshot or CaptureViewport; commit by group.
- done_when: Blueprint compiles and is saved; mesh, seven slots and emissive in place; doc node updated; bloom status reported; no screenshot tool used; the two XP tasks never run in the Editor at the same time
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-133-xp-chip-integration]] — parent global task.
