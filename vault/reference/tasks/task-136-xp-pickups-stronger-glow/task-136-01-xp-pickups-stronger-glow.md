---
id: task-136-01-xp-pickups-stronger-glow
description: "Strengthen the emissive and widen the pulse range of the XP nut and chip so the bloom is stronger."
parent: task-136-xp-pickups-stronger-glow
kind: task
status: todo
---
# XP pickups: stronger glow and wider pulse

- goal: Strengthen the emissive and widen the pulse range of the XP nut and chip so the bloom is stronger.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: Provisional values (owner default, to judge in game): GlowAmount 4 to 10 on M_XPNut_hole and M_XPChip_core_bright, 2 to 5 on M_XPChip_core_dark; PulseAmount 0.15 to 0.4 on M_XPNutGlow (period 1.2 s unchanged). No PostProcessVolume created, no global bloom setting changed; read-only: try to read the active camera post-process bloom values and report them (or say they could not be read). Note in the report that a global bloom intensity needs a PostProcessVolume (none exists in L_ArenaM1) and would affect all emissive elements. Editor open, MCP up; do not call CaptureEditorImage, Screenshot or CaptureViewport; after a component-template edit dirty the asset (toggle a value) and save again, then check git shows the .uasset modified; update the bp-pickup-xp doc node; commit by name.
- done_when: Parameters read back with the new values; no new asset beyond parameter changes; doc node updated; bloom read-only report
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-136-xp-pickups-stronger-glow]] — parent global task.
