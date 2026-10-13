---
id: task-145-02-disc-reimport
description: "Reimport SM_XPNut and SM_XPChipBig in place so they gain the glow_disc slot."
parent: task-145-xp-glow-disc
kind: task
status: in-progress
---
# XP glow disc: reimport the two meshes

- goal: Reimport SM_XPNut and SM_XPChipBig in place so they gain the glow_disc slot.
- agent: bp
- depends_on: task-145-01-xp-glow-disc
- inputs: tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Run Tools/unreal/reimport_static_mesh.py on /Game/RogueRigs/Art/XPNut/SM_XPNut and /Game/RogueRigs/Art/XPChip/SM_XPChipBig with the editor CLOSED (see tool-unreal-reimport for the exact command; mind its translator settings note). The tool output lines show the new slots and bounds (the bounds now include the disc).
- done_when: Tool exits 0; both assets show the glow_disc slot last (nut 3 slots, chip 8), paths unchanged, no duplicate asset; the body bounds checked from the script output or the report (the total bounds include the disc); the two .uasset committed by name; the Blueprints are NOT edited here (their overrides are step 3)
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-145-xp-glow-disc]] — parent global task.
