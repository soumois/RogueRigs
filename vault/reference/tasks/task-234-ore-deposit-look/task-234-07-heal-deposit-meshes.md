---
id: task-234-07-heal-deposit-meshes
description: "Three heal deposit meshes (cracked mound, inlaid slab, half-buried cluster) from the owner sheet, Blender route."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# Heal deposit meshes

- goal: 3 low-poly faceted heal deposit formations modelled in Blender after vault/assets/heal-deposit-sheet.png (owner validated 2026-10-19): grey rocks with red crystals and glowing red veins/pockets, no stone pad (the slab under the deposit is the rock pieces; the veins of the middle variant sit just above z 0), same conventions as SM_DuskiteDeposit (footprint inside 134 x 134 uu, base sinking 5 uu under z 0, pivot at the footprint centre, flat shading, UVMap only), height kept low as in the sheet (rocks about 40-60 uu, crystals at most about 70 uu, provisional); two material slots: `rock` and `crystal` (veins and crystals).
- agent: 3d
- depends_on: task-234-02-duskite-deposit-meshes
- inputs: vault/assets/heal-deposit-sheet.png, vault/reference/gdd/art-audio/art-prompts-ore-deposit.md, Art/Meshes/ore-deposit/ (Duskite meshes, reports and Tools/art/blender/ore_deposit.py, ore_deposit_preview.py to reuse), vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/agents/art-3d-lessons.md, vault/reference/gdd/art-audio/art-style-guide.md
- expected_output: Art/Meshes/ore-deposit/SM_HealDeposit_{01,02,03}.fbx with reports; previews (game camera, side, all three) in Art/Meshes/ore-deposit/preview/.
- done_when: 3 FBX inside the footprint; previews sent to the owner; no Blender process left; committed.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
