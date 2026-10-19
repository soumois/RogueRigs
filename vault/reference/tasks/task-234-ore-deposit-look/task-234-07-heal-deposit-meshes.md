---
id: task-234-07-heal-deposit-meshes
description: "Three heal deposit meshes (cracked mound, inlaid slab, half-buried cluster) from the owner sheet, Blender route."
parent: task-234-ore-deposit-look
kind: task
status: done
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
- model: sonnet
- tokens: input 36, cache write 51552, cache read 657179, output 8242
- runs: 1
- milestone: M2
- result: Commit 81d6b267, validated by the owner 2026-10-19 as is. SM_HealDeposit_01 mound (1016 tris, max 69.6 uu), _02 flat (1336, 28.8), _03 half-buried (1028, 68.6), slots rock and crystal, inside the 134 uu footprint, base -5; scripts heal_deposit.py and heal_deposit_preview.py; previews sent.

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
