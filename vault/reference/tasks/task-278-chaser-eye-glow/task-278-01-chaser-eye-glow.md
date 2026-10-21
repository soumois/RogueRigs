---
id: task-278-01-chaser-eye-glow
description: "The chaser and shield chaser red eyes glow stronger; the body look is unchanged."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# Chaser eye glow stronger

- goal: The chaser and shield chaser red eyes glow stronger; the body look is unchanged.
- agent: bp
- depends_on: none
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Leak check first: list the texels of T_Chaser_Meshy_BaseColor (and the ShieldChaser body, shield and emblem textures) where the red-eye mask saturate((R - 2 max(G, B)) x 3) is above 0 outside the eye; report them. If the mask is clean: GlowAmount default 25 -> 50 (proposed, provisional, owner tunes by eye) on M_Chaser_Meshy_Anim and M_ShieldChaser_Anim, instance overrides listed; if it leaks, stop and report the leaking areas and a tighter mask (no change). Read back, recompile, save; bp-chaser, bp-shield-chaser and chaser-meshy-pipeline updated with before, after, revert 25.
- done_when: Leak check reported; GlowAmount read back on both masters and their instances; materials compile and are saved; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.
