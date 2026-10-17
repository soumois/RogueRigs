---
id: task-217-01-xp-cube-mesh
description: "3D agent: Meshy data cube to about 600 tris, flat materials, emissive window slots and glow disc as the chip; FBX, report, renders."
parent: task-217-xp-cube-simplify
kind: task
status: review
---
# XP data cube: simplified game mesh

- goal: `Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx`, same look as the Meshy cube, 844 tris max (about 600 target, body only, glow disc excluded).
- agent: agent-3d (Blender route on a Meshy mesh: no Meshy call, no credit)
- depends_on: none
- inputs: `Art/Source/xp-core/meshy/Meshy_AI_Neon_Power_Cube_1017063448_texture_fbx/` (FBX: object `Mesh_0`, 5044 tris, 33 islands, UVMap, size 0.95 x 0.95 x 0.87 file units; base colour `..._texture.png`; normal, metallic, roughness maps NOT used, A1); raw renders `Art/Source/xp-core/meshy/inspect/cube_{front,back,left,right,tq}.png`; current chip contract: `Tools/art/blender/xp_chip_big_mesh.py` (materials, emissive warning, centred pivot, glow disc block from line 352, report fields) and `Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.report.json`; nodes `vault/reference/tech/pipelines/blender-asset-route.md`, `vault/reference/tech/pipelines/mesh-fidelity-loop.md`, `vault/reference/agents/art-3d-lessons.md`.
- method: script `Tools/art/blender/xp_cube_big_mesh.py`. Rebuild or simplify freely (the owner wants the look unchanged, not the Meshy topology): bevelled cube body, recessed square window on each of the six faces (outer magenta frame and lighter magenta centre), four octagonal bolts per face (low side count), eight corner caps. All six faces identical. Flat materials, no texture: `M_XPCubeBig_body` (dark gunmetal), `M_XPCubeBig_caps` (corner caps and bolts, lighter grey), `M_XPCubeBig_core_dark` (window frame, emissive), `M_XPCubeBig_core_bright` (window centre, emissive); colours sampled from the Meshy base colour; then the `glow_disc` slot built exactly as in the chip script (last slot, after pivot and body checks). Size: 30 cm across in X and Y like the chip, Z from the Meshy proportion; pivot at the geometric centre; centimetres, FBX_SCALE_UNITS; UVMap (100 cm per unit for the wear mask, disc radial UV as the chip) and LightMap UV1.
- fidelity_criteria: C1 body tris 844 max (report the count; about 600 aimed); C2 each of the six faces shows the same recessed window with darker frame and lighter centre; C3 four bolts per face, eight corner caps, reading as on the Meshy cube; C4 silhouette and proportions as the Meshy cube from front and three-quarter view (width to height 1.09 +/- 0.05); C5 flat colours close to the Meshy ones, no stretched or noisy texture; C6 glow_disc last slot, same construction as the chip; C7 pivot centred, 30 cm across.
- max_returns: 5
- render: front and three-quarter top view (elevation 25, azimuth 35) matching the inspect renders, plus one straight top-down view, light-grey background, camera-following sun 1.0; emissive shown as flat colour; PNGs in `Art/Meshes/xp-cube-big-mesh/preview/`.
- expected_output: FBX, `SM_XPCubeBig.report.json` (tris body and total, dimensions_cm, materials, uv checks, glow_disc, fidelity), script, previews.
- done_when: every criterion passes from the renders (or open failures reported after 5 returns); the Orchestrator opens the renders.
- editor_state: any (no Unreal import)
- token_ceiling: open
- model: sonnet (agent-3d)
- tokens: 103.2k subagent tokens in total as reported by the harness
- runs: 1
- milestone: M1
- result: scripts `Tools/art/blender/xp_cube_big_mesh.py` and `xp_cube_big_preview.py`; `Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx` body 748 tris (total 764 with the 16-tri glow disc), 30 x 30 x 27.5 cm, centred pivot, slots body #2D2C30, caps #868687, core_dark #FA11F8, core_bright #FB81FD, glow_disc; UV0 and UV1 checks pass; 2 returns, C1-C7 pass. Differences: hexagonal bolts (octagons would exceed 844), boxier caps, frame brighter than the shaded Meshy render. Renders sent; files not committed until the owner validates.

## Relations
- [[task-217-xp-cube-simplify]] — parent global task.
