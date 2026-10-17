---
id: task-218-01-shield-emblem-decal
description: "3D agent: cut out the emblem, composite it on a dark rounded plate, add it as a decal slot on the shield front, export, render."
parent: task-218-shield-emblem
kind: task
status: done
---
# Shield emblem decal

- goal: `SM_ChaserShield.fbx` with a decal slot carrying `T_ChaserShield_Emblem.png` on the front centre; the rest of the shield unchanged.
- agent: agent-3d (Blender route, no Meshy)
- depends_on: none
- inputs: emblem `Art/Source/kessler-voss-logo/20261017-kessler-voss-emblem-4.webp` (1254 px, white background, charcoal rock about #181820, yellow #F8D800); swatch `Art/Source/kessler-voss-logo/emblem-plate-colour-ref.png` (plate colour #1A1F25); shield script `Tools/art/blender/shield_v2.py` and its outputs in `Art/Meshes/ChaserShield/` (report, previews); nodes `vault/reference/tech/pipelines/blender-asset-route.md` (decal rule, UV channels), `vault/reference/tech/pipelines/mesh-fidelity-loop.md`, `vault/reference/agents/art-3d-lessons.md`.
- method: 1) Cut-out: alpha from the distance to the white background (smooth edge, no white fringe; keep the white-free emblem colours), save `Art/Source/kessler-voss-logo/20261017-kessler-voss-emblem-4-cutout.png` (RGBA, trimmed, square). 2) Texture `Art/Meshes/ChaserShield/T_ChaserShield_Emblem.png` 512 x 512 RGBA: a rounded rectangle of #1A1F25 (opaque, alpha 0 outside), the cut-out emblem centred on it filling about 80 % of the plate height. 3) Decal in `shield_v2.py` (add a step, keep everything else identical): a quad (2 tris) parallel to the front plate, offset 0.25 cm in front of it, centred horizontally on the plate and vertically at the plate centre, one planar UV 0 to 1, new last slot `M_ChaserShield_Emblem` (the shield has no glow disc); LightMap UV1 kept valid. Plate size provisional: 22 cm wide, 20 cm tall, corner radius 3 cm (the plate is 42.8 cm wide, front bolts stay outside it). 4) Re-export the FBX, update the report (tris, materials, decal size and position, uv checks).
- fidelity_criteria: C1 emblem cut out with no white fringe on a close-up; C2 dark rounded rectangle centred on the front, inside the four bolts, corners rounded; C3 yellow KV and facets readable and sharp on the front render and the three-quarter front view; C4 no z-fighting or gap seen at the decal in the three-quarter views; C5 the rest of the shield identical to the validated v2 (compare the back and top previews); C6 tris = 977 + 2.
- max_returns: 5
- render: the same cameras and light as the v2 previews (front, three-quarter front, three-quarter back) plus a close-up of the decal; overwrite the previews in `Art/Meshes/ChaserShield/preview/`.
- expected_output: cut-out PNG, `T_ChaserShield_Emblem.png`, updated script, FBX, report, previews.
- done_when: every criterion passes from the renders (or open failures after 5 returns); the Orchestrator opens the renders.
- editor_state: any (no Unreal import)
- token_ceiling: open
- model: sonnet (agent-3d)
- tokens: 76.5k subagent tokens in total as reported by the harness
- runs: 1
- milestone: M1
- result: `T_ChaserShield_Emblem.png` 512 RGBA (emblem cut out, 80 % of a 22 x 20 cm plate #1A1F25, radius 3 cm), decal quad 0.25 cm in front of the plate at z 41 cm, last slot `M_ChaserShield_Emblem` (needs Masked in Unreal); shield 979 tris; cut-out `Art/Source/kessler-voss-logo/20261017-kessler-voss-emblem-4-cutout.png`; built by `shield_v2.py`; 0 returns, C1-C6 pass. Committed before the owner's look (owner away, 2026-10-17: validates tomorrow).

## Relations
- [[task-218-shield-emblem]] — parent global task.
