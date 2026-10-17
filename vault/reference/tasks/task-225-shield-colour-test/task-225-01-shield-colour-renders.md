---
id: task-225-01-shield-colour-renders
description: "3D agent: recolour the shield texture in charcoal and light steel grey (stripes kept, yellow logo, no plate) and render both, preview only."
parent: task-225-shield-colour-test
kind: task
status: in-progress
---
# Shield colour renders

- goal: two candidate textures and comparison renders; the game files stay untouched.
- agent: agent-3d (Blender, no Meshy)
- depends_on: none
- inputs: `Art/Meshes/ChaserShield/T_ChaserShield_BaseColor.png`, `T_ChaserShield_Emblem.png`, `SM_ChaserShield.fbx` and report; `Tools/art/blender/shield_v2.py` (how the base colour and emblem were made: plate colour, stripes, emblem cut-out); emblem cut-out `Art/Source/kessler-voss-logo/20261017-kessler-voss-emblem-4-cutout.png`; combined mesh `Art/Meshes/shield-chaser/SM_ShieldChaser_Meshy_Anim.fbx` and `Tools/art/blender/shield_chaser_bake.py` (to pose the robot in clip 0 frame 0); in-game reference `vault/assets/shield-chaser-ingame-run-close.png` (floor colour, camera); `vault/reference/agents/art-3d-lessons.md`.
- method: work in `Art/Source/shield-colour-test/` only (never overwrite the Art/Meshes files). For each variant (A charcoal about #33363B, B light steel grey about #A9AFB5): recolour the yellow plate texels of the base colour to the variant colour, keep the yellow and black stripe texels and the dark grip and bolt texels as they are; emblem texture: drop the dark rounded plate and the charcoal rock, keep only the yellow parts (KV and facets) on transparent, so the logo is yellow straight on the shield. Renders per variant: shield three-quarter front close-up (the light of the shield previews), and the robot holding it (clip 0 frame 0) seen from a steep near-vertical camera like the game, on a floor of the in-game colour; plus one side-by-side sheet of A, B and the current yellow.
- done_when: the textures and renders exist in that folder, the sheet is opened by the Orchestrator; commit by path.
- editor_state: any (no Unreal)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-225-shield-colour-test]] — parent global task.
