---
id: task-189-03-ore-chunk-materials
description: "BP: import SM_OreChunk and create its violet glowing materials (pulse like the XP nut), Duskite colour."
parent: task-189-ore-chunk-look
kind: task
status: done
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
- model: sonnet
- tokens: input 70, cache write 69553, cache read 1511580, output 537
- runs: 1
- milestone: M1
- result: SM_OreChunk_01..03 imported in /Game/RogueRigs/Art/OreChunk/ (bounds read back, 30 cm, centred, 396/398/496 tris); MI_OreChunk_Bright (0.55, 0.1, 1.0) x12 and MI_OreChunk_Dark (0.25, 0.04, 0.55) x6, children of M_XPNutGlow, set on both slots of all three meshes; first import was 100x too small (ore_chunk.py worked in metres), script fixed and FBX re-exported; look not checked; commit a27ae043

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
