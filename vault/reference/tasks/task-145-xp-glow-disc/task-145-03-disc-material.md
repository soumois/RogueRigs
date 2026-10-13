---
id: task-145-03-disc-material
description: "Create the additive radial disc material and its two instances and set the new slot on both pickup Blueprints."
parent: task-145-xp-glow-disc
kind: task
status: todo
---
# XP glow disc: material and Blueprint slots

- goal: Create the additive radial disc material and its two instances and set the new slot on both pickup Blueprints.
- agent: bp
- depends_on: task-145-02-xp-glow-disc-reimport
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: M_XPGlowDisc (new, /Game/RogueRigs/Art/XPNut/ or a sibling folder): Unlit, Additive blend, two sided, emissive = Color x Intensity x pow(saturate(1 - 2 x distance(UV0, (0.5, 0.5))), Falloff); parameters Color, Intensity (default 0.5), Falloff (default 2.0); MI_XPGlowDisc_Nut (Color cyan (0, 0.9, 1), Intensity 0.5, Falloff 2) and MI_XPGlowDisc_Chip (Color (0.8, 0.25, 1.0), Intensity 0.5, Falloff 2); BP_PickupXP OverrideMaterials gets element [2] = MI_XPGlowDisc_Nut (the existing [0] and [1] untouched) and BP_PickupXPBig element [7] = MI_XPGlowDisc_Chip (existing [0..6] untouched), set through the CDO path with [] first then the elements (quirk of tasks 132 and 133); BP CastShadow stays false; wishlist line about the XP ground glow marked done with a link; doc bp-pickup-xp updated (disc radius 50 and 80 cm, parameters, provisional values to tune by eye, how to tune them in the editor).
- done_when: All values read back; assets saved with save_assets (the named assets only) and modified in git; the editor was not driven by key presses or UI automation; if a save fails STOP and report; doc and wishlist updated; lint clean
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-145-xp-glow-disc]] — parent global task.
