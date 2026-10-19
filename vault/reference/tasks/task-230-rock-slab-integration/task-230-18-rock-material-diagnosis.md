---
id: task-230-18-rock-material-diagnosis
description: "Find why M_RockSlab and the Duskite crystal material changes do not show in game, and fix them."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Find why M_RockSlab and the Duskite crystal material changes do not show in game, and fix them

- goal: in -game (owner 2026-10-19, after 230-17): Duskite crystals render plain grey though MI_OreChunk_Bright (violet, glow 12) is assigned to both slots, while red heal crystals from the same parent M_XPNutGlow glow; rock tops are flat lavender with a lighter rim band and no crack lines, unchanged by 230-16 (TopParams.b 1.0, TopContrast 2.5) and by 0.3/0.7 damage, with Nanite on or off. Suspected common cause (unverified): the rendered material is not the one assigned (usage flags such as bUsedWithInstancedStaticMeshes/Nanite missing, shader compile failure falling back, stale overrides, imported slot materials M_RockSlab_001), or the custom data/vertex colour reads in the HLSL are wrong.
- agent: bp
- depends_on: task-230-17-rock-sag-visible-check
- inputs: vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/tech/blueprints/fx/bp-pickup-ore.md, vault/reference/tech/blueprints/world/bp-arena-grid.md, vault/reference/tech/systems/world/sys-grid/sys-grid-rock-piece-data.md, vault/reference/agents/bp-mcp-lessons.md; captures Saved/RockSlab/after2/ (rs_h_crystals_00, rs_s_sag_low_00); Art/Meshes/rock-slab/SM_RockSlab.report.json
- expected_output: cause of each symptom with evidence (material usage flags, compile errors, the material actually set on the running components in PIE, vertex colour and custom data reads), fixes in the material assets, doc nodes updated.
- done_when: assets compile and are saved; each symptom has a cause and a fix or a precise report; committed; the look is checked in task-230-19.
- editor_state: open
- token_ceiling: 60000
- model: sonnet
- tokens: input 98, cache write 114461, cache read 3291220, output 3264
- runs: 1
- milestone: M2
- result: Commits 22c2f1e4, decf917f. Duskite grey: M_XPNutGlow lacked bUsedWithInstancedStaticMeshes on disk (editor sets it on demand, -game does not), default material drawn; flag saved. Flat top, no cracks, no rim: the rock meshes were imported with VertexColorImportOption Ignore, white vertex colour sent every face to the side branch; M_RockSlab now detects sides from the vertex normal and only brightens the rim when real colours exist. Rim band restore needs a reimport with vertex colours (task-230-20). Look unverified until 230-19.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
