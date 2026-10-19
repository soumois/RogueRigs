---
id: task-234-02-duskite-deposit-meshes
description: "Three Duskite crystal formation meshes (spire, ridge, crown) from the owner sheet, Blender route."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# Three Duskite crystal formation meshes (spire, ridge, crown) from the owner sheet, Blender route

- goal: 3 low-poly faceted crystal formations modelled in Blender after vault/assets/duskite-deposit-sheet.png (owner 2026-10-19): crystals only, no stone pad (the slab under the deposit is the rock pieces); footprint inside 134 x 134 uu (2 x 2 cells of 67 uu) with a small margin, base at z 0, pivot at the footprint centre, tallest crystal about 110 uu (provisional, the slab top is at 100 uu so the deposit stands out without hiding the miner); flat shading; one material slot `crystal`; the crystals sink 5 uu under z 0 so no gap shows on the slab.
- agent: 3d
- depends_on: none
- inputs: vault/assets/duskite-deposit-sheet.png, vault/reference/gdd/art-audio/art-prompts-ore-deposit.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/agents/art-3d-lessons.md, vault/reference/gdd/art-audio/art-style-guide.md, Art/Source/rock-look/20261017-131704-rock-look-2.png (look reference)
- expected_output: Art/Meshes/ore-deposit/SM_DuskiteDeposit_{01,02,03}.fbx, script Tools/art/blender/ore_deposit.py, report json (bounds, tris, slot), previews at the game camera angle in Art/Meshes/ore-deposit/preview/.
- done_when: 3 FBX exist inside the footprint; previews rendered and sent to the owner; no Blender process left; committed.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
