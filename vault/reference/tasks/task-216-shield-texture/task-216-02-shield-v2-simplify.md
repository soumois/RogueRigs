---
id: task-216-02-shield-v2-simplify
description: "3D agent: simplify the owner's second Meshy shield to about 1k flat tris, remove the back side bracket, rebake and fix the texture, stripes over the top."
parent: task-216-shield-texture
kind: task
status: in-progress
---
# Shield v2: simplify, remove bracket, retexture back

- goal: `SM_ChaserShield.fbx` from the owner's second Meshy shield, about 1000 tris with flat surfaces, side bracket removed, texture rebaked and corrected.
- request: "il va falloir le simplifier (principalement aplatir les surfaces). Retirer cette partie là aussi (derrière c'est une surface plate) et il faudrait que tu retextures l'arrière. Tu peux garder les lignes noires et jaunes sur le haut du bouclier à l'arrière, il faudrait que les lignes passent sur la surface du dessus et rejoignent les lignes de devant. Il faudrait que ça tourne autour de 1k triangles." (owner 2026-10-17)
- agent: agent-3d (Blender route on a Meshy mesh: no Meshy call, no credit)
- depends_on: none (replaces the mesh of task-216-01)
- inputs: `Art/Source/shield-meshy/meshy-v2/Meshy_AI_Hazard_Panel_1017060515_texture_fbx/` (FBX: object `output_unwrapped`, 2804 tris, one island, UVMap, size 0.99 x 0.26 x 1.90 file units, Z up, front plate toward -Y; base colour `..._texture.png` 2048; normal, metallic, roughness maps NOT used, A1); owner crops `meshy-v2/owner-remove-bracket.png` (the part to remove: the horizontal bracket on the back, from the central grip block to the right edge of the back view) and `meshy-v2/owner-back-top-stripes.png` (back top stripes to keep); quick renders of the raw mesh `meshy-v2/inspect/v2_{front,back,left,right,tq}.png`; previous script `Tools/art/blender/shield_texture.py` (reuse its transform, export, report, UV1 and render code); nodes `vault/reference/tech/pipelines/blender-asset-route.md`, `vault/reference/tech/pipelines/mesh-fidelity-loop.md`, `vault/reference/agents/art-3d-lessons.md`.
- defaults (Orchestrator, owner may correct): the back stays yellow as in the Meshy texture; the vertical grip bar and its three mounts stay dark; only the horizontal bracket goes, replaced by the flat yellow back plate.
- method: new script `Tools/art/blender/shield_v2.py`. 1) Delete the bracket faces, close the plate flat. 2) Simplify: planar dissolve and flatten the plate faces (front, back, top, sides, bevels each exactly planar), then decimate the grip to reach 900 to 1100 tris; keep the silhouette, cut corners, raised inner border, octagonal bolts. 3) New UV unwrap of the simplified mesh, bake the base colour from the original Meshy mesh (selected to active, diffuse colour only) into `T_ChaserShield_BaseColor.png` 1024. 4) Repaint in the texture: the bracket area on the back flat yellow (sampled from the back plate); the top face of the shield carries the same yellow and black 45 degree stripes, continuous with the front top band and the back top band (stripe phase matched at both top edges). 5) LightMap UV1, 82 cm tall, front +X, bottom-centre pivot, centimetres, FBX_SCALE_UNITS; one material `M_ChaserShield`; `grip_offset_cm` = centre of the middle grip block.
- fidelity_criteria: C1 tris 900 to 1100; C2 front, back and top faces flat (no visible wobble or diagonal fold in renders); C3 no side bracket on the back, flat yellow where it was, no seam or stretched texel visible; C4 grip bar with three dark mounts kept; C5 stripes: front top band, top-right and bottom-left front corners, back top band and back corners as in the Meshy texture; C6 on the top face the stripes are present and meet the front and back bands with no break visible in the three-quarter top view; C7 front centre plain yellow (emblem decal later); C8 height 82 cm, height to width ratio 1.91 +/- 0.1.
- max_returns: 5
- render: front, back, left, right orthographic, three-quarter top view looking at the front and one looking at the back (elevation 35), close-up of the top edge; light-grey background, camera-following sun energy 1.0 (the neutral set of task-216-01); PNGs in `Art/Meshes/ChaserShield/preview/`.
- expected_output: `Art/Meshes/ChaserShield/SM_ChaserShield.fbx` (overwrites the v1 output), `SM_ChaserShield.report.json` (tris, dimensions_cm, uv checks, grip_offset_cm, fidelity), `T_ChaserShield_BaseColor.png`, script, previews. Remove the v1 outputs no longer used (stripe texture, old previews).
- done_when: every criterion passes from the renders (or open failures reported after 5 returns); the Orchestrator opens the renders.
- editor_state: any (no Unreal import)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-216-shield-texture]] — parent global task.
