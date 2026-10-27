---
id: bp-space-background
description: "BP_SpaceBackground, actor of three large planes (stars, nebula, dust) under the arena; depths, scales, coverage arithmetic, placement in L_ArenaM1 and removal."
parent: blueprints-world-index
kind: doc
---
# BP_SpaceBackground

- asset_path: /Game/RogueRigs/Environment/Space/BP_SpaceBackground (task-332-02, provisional)
- parent_class: AActor
- purpose: three stacked planes below the floor that show the space material in the void; depth gives parallax.
- components: three StaticMeshComponents, mesh /Engine/BasicShapes/Plane (100 uu base), all Movable, NoCollision, CastShadow off, bReceivesDecals off, bAffectDistanceFieldLighting off.
  - StarsPlane: Z -12000, scale 1010 (101000 uu), MI_Space_Stars.
  - NebulaPlane: Z -5000, scale 615 (61500 uu), MI_Space_Nebula.
  - DustPlane: Z -2000, scale 450 (45000 uu), MI_Space_Dust.
- exposed_defaults: none beyond the component values above; materials in [[bp-space-background-material]].
- events: none.
- logic_in_bp: none (no graph logic).
- assets: /Engine/BasicShapes/Plane; MI_Space_Stars, MI_Space_Nebula, MI_Space_Dust.
- owner_agent: agent-bp
- placed_in: /Game/RogueRigs/Maps/L_ArenaM1 at (8040, 8040, 0), rotation 0, scale 1 (arena spans 0 to 16080), stored as an external actor file. Read back after map reload: OK.
- remove: delete the actor from L_ArenaM1; nothing else references it.

## Coverage arithmetic
Camera: pitch -50, arm 1400, horizontal FOV 75, 16:9, camera Z 1162.5, 899.9 uu behind the miner. Plane half-size = 1.10 x (8040 x sqrt2 + 899.9 + maxR), where maxR is the ground distance of the farthest frustum corner ray hit on the plane. Valid for any yaw.

| Plane | Depth h | maxR | Reach | Half | Margin |
|---|---|---|---|---|---|
| Stars | 13162 | 33391 | 45661 | 50500 | 4839 |
| Nebula | 6162 | 15633 | 27903 | 30750 | 2847 |
| Dust | 3162 | 8023 | 20293 | 22500 | 2207 |

A different camera pitch or arm needs the scales recomputed.

## Verification (task-332-03)
On screen with `-game` near the east edge: no plane edge visible, no tiling. Look and tuning finding are in [[bp-space-background-material]].

- not_verified: parallax (camera static); other arena edges; packaged build; nebula and dust visibility at normal brightness (found invisible, see the material node).

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-space-background-material]] — material, parameters, tuning and finding.
- [[bp-arena-grid]] — the map's void this actor fills.
- [[task-332-space-background]] — global task with the decided values.
