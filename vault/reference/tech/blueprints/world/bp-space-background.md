---
id: bp-space-background
description: "BP_SpaceBackground, actor of four large planes (stars, deep, clouds, dust) under the arena; depths, scales, coverage arithmetic, placement in L_ArenaM1 and removal."
parent: blueprints-world-index
kind: doc
---
# BP_SpaceBackground

- asset_path: /Game/RogueRigs/Environment/Space/BP_SpaceBackground (task-332-02, provisional)
- parent_class: AActor
- purpose: four stacked planes below the floor that show the space material in the void; depth gives parallax.
- components: four StaticMeshComponents, mesh /Engine/BasicShapes/Plane (100 uu base), all Movable, NoCollision, CastShadow off, bReceivesDecals off, bAffectDistanceFieldLighting off.
  - StarsPlane: Z -12000, scale 1010 (101000 uu), MI_Space_Stars.
  - DeepPlane (task-335-04, 03e515f2): Z -8000, scale 790 (79000 uu), MI_Space_Deep.
  - NebulaPlane: Z -5000, scale 615 (61500 uu), MI_Space_Clouds (was MI_Space_Nebula, changed in task-335-04).
  - DustPlane: Z -2000, scale 450 (45000 uu), MI_Space_Dust.
- exposed_defaults: none beyond the component values above; materials in [[bp-space-background-material]].
- events: none.
- logic_in_bp: none (no graph logic).
- assets: /Engine/BasicShapes/Plane; MI_Space_Stars, MI_Space_Deep, MI_Space_Clouds, MI_Space_Dust (MI_Space_Nebula no longer used).
- owner_agent: agent-bp
- placed_in: /Game/RogueRigs/Maps/L_ArenaM1 at (8040, 8040, 0), rotation 0, scale 1 (arena spans 0 to 16080), stored as an external actor file. Read back after map reload: OK (three planes, task-332); four planes read back on the instance after task-335-04 (reload may have been a no-op, render unverified at that step).
- remove: delete the actor from L_ArenaM1; nothing else references it.

## Coverage arithmetic
Camera: pitch -50, arm 1400, horizontal FOV 75, 16:9, camera Z 1162.5, 899.9 uu behind the miner. Plane half-size = 1.10 x (8040 x sqrt2 + 899.9 + maxR), where maxR is the ground distance of the farthest frustum corner ray hit on the plane. Valid for any yaw.

| Plane | Depth h | maxR | Reach | Half | Margin |
|---|---|---|---|---|---|
| Stars | 13162 | 33391 | 45661 | 50500 | 4839 |
| Deep | 9163 | 23244 | 35514 | 39500 | 3986 |
| Nebula | 6162 | 15633 | 27903 | 30750 | 2847 |
| Dust | 3162 | 8023 | 20293 | 22500 | 2207 |

Deep maxR is derived from the reported reach and margin (task-335-04). A different camera pitch or arm needs the scales recomputed.

## Verification (task-332-03)
On screen with `-game` near the east edge: no plane edge visible, no tiling. Look and tuning finding are in [[bp-space-background-material]].

- not_verified: parallax (camera static); other arena edges; packaged build; nebula and dust visibility at normal brightness (found invisible, see the material node).

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-space-background-material]] — procedural material (stars, dust), parameters, tuning and finding.
- [[bp-space-texture-material]] — textured material of DeepPlane and NebulaPlane.
- [[bp-arena-grid]] — the map's void this actor fills.
- [[bp-asteroid-field]] — nearer asteroids placed in front of these planes (task-334).
- [[task-332-space-background]] — global task with the decided values.
