---
id: task-148-02-xp-disc-fade-material
description: "Make the glow disc material read the fade value and turn it on in the two pickup Blueprints."
parent: task-148-xp-disc-fade
kind: task
status: done
---
# XP glow disc: material reads the fade

- goal: Make the glow disc material read the fade value and turn it on in the two pickup Blueprints.
- agent: bp
- depends_on: task-148-01-xp-disc-fade
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md)
- expected_output: M_XPGlowDisc multiplies its emissive by a scalar parameter Fade with Use Custom Primitive Data on and the index of GlowFadeDataIndex (0), default value 1 so the editor preview shows the disc; BP_PickupXP and BP_PickupXPBig class defaults bGlowFade true (GlowFadeDuration 0.2, index 0); both Blueprints compiled and saved; doc bp-pickup-xp updated (the fade, how to change the duration, provisional, owner 2026-10-13)
- done_when: The material compiles; bGlowFade reads true on both Blueprint defaults and the duration 0.2; assets saved (material, the two Blueprints) and modified in git; the strict rules of task-145 apply (no UI automation, no Ctrl+S, only named assets, no screenshots, no performance test); doc updated; lint clean
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 36, cache write 38025, cache read 455972, output 915
- runs: 1
- milestone: M2
- result: M_XPGlowDisc: Fade scalar parameter (default 1, Custom Primitive Data index 0) multiplies the emissive; bGlowFade true on BP_PickupXP and BP_PickupXPBig (duration 0.2, index 0); compiles; runtime effect not verified, for the owner.

## Relations
- [[task-148-xp-disc-fade]] — parent global task.
