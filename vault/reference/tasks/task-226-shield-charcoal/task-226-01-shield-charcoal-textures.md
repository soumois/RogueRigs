---
id: task-226-01-shield-charcoal-textures
description: "3D agent: shield_v2.py produces the charcoal base colour and the yellow-only emblem of variant A; regenerate the shield textures in place."
parent: task-226-shield-charcoal
kind: task
status: done
---
# Charcoal shield textures

- goal: `Art/Meshes/ChaserShield/T_ChaserShield_BaseColor.png` and `T_ChaserShield_Emblem.png` are the variant A textures, and the shield script rebuilds exactly them.
- agent: agent-3d (Blender, no Meshy)
- depends_on: none
- inputs: `Tools/art/blender/shield_v2.py` (base colour, stripes, emblem plate composite), `Tools/art/blender/shield_colour_test.py` and `Art/Source/shield-colour-test/` (variant A: T_ChaserShield_BaseColor_A.png, T_ChaserShield_Emblem_YellowOnly.png, the recolour rules); `Tools/art/blender/shield_debris_pieces.py` and `Art/Source/shield-chaser/debris/` (keeps copies of the two textures; cut caps sample a texel of the base colour); `vault/reference/agents/art-3d-lessons.md`.
- method: change `shield_v2.py` so its output is variant A: plate colour #33363B instead of the yellow, stripes, grip and bolts unchanged; emblem = yellow parts only (KV and facets) on transparent, no rounded plate, no charcoal rock. Keep geometry, UVs and the FBX unchanged (do not re-export the mesh unless the script cannot write the textures without it; if it re-exports, the FBX must be byte-identical or report why). Regenerate the two textures in `Art/Meshes/ChaserShield/` and compare them with the task-225 variant A files (report the pixel difference). Update the two texture copies in `Art/Source/shield-chaser/debris/`. Check the debris cut-cap texel: report its colour now (yellow or charcoal) and keep it as it falls. Re-render the shield previews (`Art/Meshes/ChaserShield/preview/`: front, three-quarter front, emblem close-up).
- done_when: textures regenerated and matching variant A; previews opened by the Orchestrator; commit by path.
- editor_state: any (no Unreal)
- token_ceiling: open
- model: sonnet
- tokens: input 34, cache write 70206, cache read 823291, output 784
- runs: 1
- milestone: M1
- result: Commit 89f28130: shield_v2.py now outputs variant A (plate #33363B, stripes kept, grip and bolts gunmetal with a stricter yellow clean-up, emblem yellow parts only on transparent); textures regenerated (vs task-225 A: 26 base colour texels differ by more than 1/255), 12 previews re-rendered, debris texture copies updated; FBX and report restored unchanged (re-export is byte-different only in ids); debris cut-cap texel now charcoal. Open: shield_debris_pieces.py asserts a yellow cap texel and would fail if re-run; SM_ChaserShield.report.json texture notes still describe the yellow plate.

## Relations
- [[task-226-shield-charcoal]] — parent global task.
