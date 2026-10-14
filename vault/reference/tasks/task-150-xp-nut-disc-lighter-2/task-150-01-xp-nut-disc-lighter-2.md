---
id: task-150-01-xp-nut-disc-lighter-2
description: "Lower the glow disc intensity of the XP nut a little more."
parent: task-150-xp-nut-disc-lighter-2
kind: task
status: todo
---
# XP nut disc: lighter again

- goal: Lower the glow disc intensity of the XP nut a little more.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: MI_XPGlowDisc_Nut Intensity 0.12 (was 0.2), the other parameters and the chip instance unchanged, read back, saved (this asset only); doc updated (provisional, owner 2026-10-13). STRICT: save only the named assets with save_assets; if a save fails, a property cannot be set or a call is denied, STOP and report; no SlateInspectorToolset, no key presses (never Ctrl+S), no UI automation; the owner's Content/__ExternalActors__ files are not yours (never touch or commit them); no screenshot tool; no performance test; update the doc node bp-pickup-xp; commit by name.
- done_when: Intensity reads 0.12 on the nut instance and 0.5 on the chip instance; asset saved and modified in git; doc updated
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-150-xp-nut-disc-lighter-2]] — parent global task.
