---
id: task-225-01-shield-colour-renders
description: "3D agent: recolour the shield texture in charcoal and light steel grey (stripes kept, yellow logo, no plate) and render both, preview only."
parent: task-225-shield-colour-test
kind: task
status: done
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
- model: sonnet
- tokens: input 36, cache write 104578, cache read 1368571, output 17543
- runs: 1
- milestone: M1
- result: Commit bd0837c4: Art/Source/shield-colour-test/ (sheet_shield_colours.png: columns current yellow, A charcoal #33363B, B light steel grey #A9AFB5; close-ups, robot mid and game-scale views; textures T_ChaserShield_BaseColor_A/_B and T_ChaserShield_Emblem_YellowOnly), script Tools/art/blender/shield_colour_test.py; stripes, grip and bolts unchanged, logo yellow without plate. A stands out clearly from robot and floor; B is close to the floor brightness, its logo faint; on A the bolts barely show. Game files untouched. Owner choice pending.

## Relations
- [[task-225-shield-colour-test]] — parent global task.
