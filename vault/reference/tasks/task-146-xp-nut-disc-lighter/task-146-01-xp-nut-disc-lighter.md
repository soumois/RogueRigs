---
id: task-146-01-xp-nut-disc-lighter
description: "Lower the glow disc intensity of the XP nut."
parent: task-146-xp-nut-disc-lighter
kind: task
status: done
---
# XP nut disc: lighter

- goal: Lower the glow disc intensity of the XP nut.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: MI_XPGlowDisc_Nut (/Game/RogueRigs/Art/XPNut/MI_XPGlowDisc_Nut) Intensity 0.2 (was 0.5), Color and Falloff unchanged, read back, saved (this asset only); the chip instance MI_XPGlowDisc_Chip stays at 0.5; doc updated (provisional, owner 2026-10-13). STRICT: save only the named assets with save_assets; if a save fails, a property cannot be set or a call is denied, STOP and report; no SlateInspectorToolset, no key presses (never Ctrl+S), no UI automation; the owner's Content/__ExternalActors__ files are not yours (never touch or commit them); no screenshot tool; no performance test; update the doc node bp-pickup-xp; commit by name.
- done_when: Intensity reads 0.2 on the nut instance and 0.5 on the chip instance; asset saved and modified in git; doc updated
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 24, cache write 21800, cache read 174850, output 183
- runs: 2
- milestone: M2
- result: MI_XPGlowDisc_Nut Intensity 0.2 (was 0.5), chip instance unchanged at 0.5; first attempt failed because the editor was closed.

## Relations
- [[task-146-xp-nut-disc-lighter]] — parent global task.
