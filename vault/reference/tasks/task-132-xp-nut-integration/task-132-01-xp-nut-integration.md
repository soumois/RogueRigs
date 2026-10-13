---
id: task-132-01-xp-nut-integration
description: "Import the XP hex nut mesh into Unreal, give it a glowing material with a light emissive pulse, and use it in BP_PickupXP."
parent: task-132-xp-nut-integration
kind: task
status: in-progress
---
# XP nut integration in Unreal

- goal: Import the XP hex nut mesh into Unreal, give it a glowing material with a light emissive pulse, and use it in BP_PickupXP.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: SM_XPNut imported (cm, bottom-centre pivot, no auto lightmap UV channel if the tool allows); material instances for slots M_XPNut_body (grey, same grey as the FBX #70757B) and M_XPNut_hole (emissive cyan (0, 0.9, 1), GlowAmount 4, pulse); BP_PickupXP PickupMesh = SM_XPNut instead of the cube, the 30 cm footprint matches the old cube at scale 0.3 so the BP scale becomes 1; OverrideMaterials set for the two slots; the bp-pickup-xp doc node updated. Provisional values (owner default 2026-10-13, to judge in game): emissive pulse of plus or minus 15 percent of the emissive strength, period 1.2 s, phase from the actor world position so pickups do not blink together, driven by a time-based material parameter (no tick, no Blueprint logic); no ground glow disc, no light component, no shadow-casting, no collision change; check whether bloom is enabled in the level post-process and REPORT the result without changing it; do not call CaptureEditorImage, Screenshot or CaptureViewport; commit by group.
- done_when: Blueprint compiles and is saved; mesh, two material slots and emissive in place; doc node bp-pickup-xp updated; bloom status reported; no screenshot tool used
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-132-xp-nut-integration]] — parent global task.
