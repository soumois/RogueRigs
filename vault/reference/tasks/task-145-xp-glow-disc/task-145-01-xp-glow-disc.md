---
id: task-145-01-xp-glow-disc
description: "Add a flat glowing disc under the XP nut and chip: geometry in the FBX, additive material, reimport and Blueprint slots."
parent: task-145-xp-glow-disc
kind: task
status: done
---
# XP pickups: glow disc on the floor

- goal: Add a flat glowing disc under the XP nut and chip: geometry in the FBX, additive material, reimport and Blueprint slots.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), agent-3d (vault/reference/agents/agent-3d.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md)
- expected_output: Both Blender scripts (Tools/art/blender/xp_nut_mesh.py, xp_chip_big_mesh.py) add a flat 16-sided disc (centre vertex plus fan, normals up) as the LAST material slot named glow_disc with its own radial UV0 (u and v = 0.5 plus x or y over 2 R, so the distance to (0.5, 0.5) is the radius in 0 to 0.5). Pickups rest HoverHeight = 30 cm above the floor of their cell (RRPickup.h), so the disc lies 29.8 cm below the actor origin (0.2 cm above the floor to avoid z-fighting). Nut (scale 1 in BP_PickupXP): world radius 50 cm, disc z -29.8, radius 50. Chip (BP_PickupXPBig root scale 1.8): world radius 80 cm and z -29.8, so in local units radius 44.44 and z -16.56. The pivot and the body stay exactly where they are (body bbox centre at the origin, as now): the disc is excluded from the pivot centring, the report keeps the body bounds and adds the total bounds and a disc object (radius, z, tris). Body geometry, the other slots and their order, UVMap and LightMap of the body, fidelity objects: unchanged. The disc gets a LightMap (UV1) entry that does not overlap the body, or the report states that the disc does not use the lightmap (it is unlit, shadowless): either way uv_checks for the body stay as before. FBX re-exported at the same paths Art/Meshes/xp-nut-mesh/SM_XPNut.fbx and Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx; reports updated; committed by name (scripts, FBX, reports; no PNG).
- done_when: Reports: nut slots [M_XPNut_body, M_XPNut_hole, glow_disc], chip slots the seven M_XPChipBig_* then glow_disc; body tris unchanged (nut 108, chip 844) and disc 16 tris each; body bounds unchanged (nut z -5.85 to 5.85, chip z -5.3 to 5.3); disc radius and z as specified; nothing imported into Unreal; no Unreal process used
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 16, cache write 34173, cache read 182007, output 378
- runs: 1
- milestone: M2
- result: glow_disc added as the last slot of SM_XPNut (3 slots, 124 tris) and SM_XPChipBig (8 slots, 860 tris), radius 50 and 44.44 at z -29.8 and -16.56, body bounds and pivot unchanged; FBX re-exported.

## Relations
- [[task-145-xp-glow-disc]] — parent global task.
