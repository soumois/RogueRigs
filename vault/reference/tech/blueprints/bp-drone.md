---
id: bp-drone
description: "BP_Drone, Blueprint of ARRDrone: 40 uu teal grey-box sphere orbiting the miner; open for its mesh, material and PIE orbit readings."
parent: blueprints-index
kind: doc
---
# BP_Drone

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_Drone
- parent_class: ARRDrone (/Script/RogueRigs.RRDrone)
- purpose: the visible helper drone, spawned by the miner from its `DroneClass`; visuals only, all logic in C++ ([[sys-drone]]).
- components: inherited `Mesh` (root static mesh) and `LaserMesh` (cutting laser, hidden outside Dig); none added.
- exposed_defaults: Mesh StaticMesh = /Engine/BasicShapes/Sphere, RelativeScale3D (0.4, 0.4, 0.4) = 40 uu, OverrideMaterials[0] = MI_GB_Drone; C++ defaults kept (OrbitRadius 150, OrbitPeriod 2, HoverHeight 150, FollowInterpSpeed 10). LaserMesh StaticMesh = /Engine/BasicShapes/Cylinder, OverrideMaterials[0] = MI_GB_Laser.
- events: none.
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Drone (instance of M_GreyBox, vector `Color` linear (0, 0.45, 0.45, 1), teal after [[color-code]]). /Game/RogueRigs/Materials/GreyBox/MI_GB_Laser (vector `Color` linear (2, 0, 0, 1), saturated red brighter than heal ore; [[color-code]] lists the mining laser as white-gold, owner to confirm).
- dig by contact (task-029-06): no override of the removed `DigHoverDistance` or `DigCancelDistanceCells` exists in the Blueprint. The per-block laser fan is C++ (task-029-08, [[sys-drone]]): its extra beams copy `LaserMesh`'s mesh and material at BeginPlay, so the Blueprint needs no new property.
- owner_agent: agent-bp
- verified: PIE (fine grid): with the miner teleported against a rock wall at (7603, 5929.5), the drone read state Dig for about 1.6 s, then Orbit once the arc was dug; no log error. PIE on L_ArenaM1 spawns one BP_Drone_C_0; with the miner at (8100, 8100, 90.15) the drone read (7950, 8100, 152.15) then 0.5 s later (8175, 7970, 152.15): horizontal distance 150 uu, height 150 uu above the capsule bottom.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-drone]] — the C++ class it derives from.
- [[bp-miner]] — the Blueprint that spawns it through `DroneClass`.
- [[color-code]] — colour convention for the teal tint.
