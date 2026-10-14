---
id: task-158-01-chaser-glow-mask-fix
description: "Stop the whole chaser from glowing: only the red parts of the texture may emit light."
parent: task-158-chaser-glow-mask-fix
kind: task
status: in-progress
---
# Chaser: glow only on the red parts

- goal: Stop the whole chaser from glowing: only the red parts of the texture may emit light.
- agent: bp
- depends_on: none
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In M_Chaser_Meshy_Anim and M_Chaser_Meshy (/Game/RogueRigs/Art/Chaser/) the emissive red mask becomes saturate((R - 2 x max(G, B)) x 3) on the linear base colour texture (yellow, olive and dark texels give 0, the red eye about 0.8 or more): the rest of the emissive chain (Base Color x mask x GlowAmount 25) stays; the docs bp-chaser mention the new mask
- done_when: Both materials recompile without error; the mask expression read back; the assets saved and committed by name; the docs updated; the editor stays open; the owner is told to look at the yellow body (no glow) and the red eye (glow)
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-158-chaser-glow-mask-fix]] — parent global task.
