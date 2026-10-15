---
id: task-189-03-ore-chunk-materials
description: "BP: import SM_OreChunk and create its violet glowing materials (pulse like the XP nut), Duskite colour."
parent: task-189-ore-chunk-look
kind: task
status: todo
---
# Ore chunk: import and materials

- goal: import SM_OreChunk into /Game/RogueRigs/Art/OreChunk/ (no extra lightmap UV channel if avoidable) and create its material instances: crystal slots as children of M_XPNutGlow (same 1.2 s pulse, no tick) in Duskite violet (bright and dark tones from the chosen image, provisional values the owner tunes), any body slot from M_GreyBox; mesh slot defaults set; distinct from the lilac (0.8, 0.25, 1.0) of the big XP chip.
- agent: bp
- depends_on: task-189-02-ore-chunk-mesh
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), color-code (vault/reference/gdd/art-audio/color-code.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md); Art/Meshes/OreChunk/SM_OreChunk.fbx and its report
- expected_output: SM_OreChunk asset and material instances in /Game/RogueRigs/Art/OreChunk/, saved by explicit path; values written to bp-pickup-ore (assets section).
- done_when: mesh bounds read back (about 30 cm, centred); every slot has its material; materials compile; doc updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
