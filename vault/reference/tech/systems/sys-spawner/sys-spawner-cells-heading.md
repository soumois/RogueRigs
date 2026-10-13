---
id: sys-spawner-cells-heading
description: "Spawn cell choice: reachable candidate cells, run heading sectors and the view rectangle. Open to change where robots appear."
parent: sys-spawner
kind: doc
---
# Spawner — cells heading

  - candidate cell (task-031-02): walkable and reachable in the flow field ([[sys-flow-field]] `IsWalkableCell`, `GetCellDistance`), no pawn's actor location in it, center at a box distance from the view rectangle between `SpawnBandMinDistance` and `SpawnBandMaxDistance` uu; band empty: the nearest band of the same width beyond it (near edge within `SpawnBandFallbackMaxDistance`). Detour filter: cells whose walk to the miner (flow steps × cell size) exceeds `MaxPathDetourRatio` × straight distance are dropped while others remain (a robot behind rock took 7.35 s to show; now 2.25 s). Since task-041-01 the filter applies only to side and back sector cells, and to every cell while the miner is still; front sector cells (run heading below) are kept whatever their detour, so robots also wait beyond the rock the miner digs toward. Spawn at the cell center, capsule bottom `SpawnFloorClearance` above the floor top (grid actor Z), AdjustIfPossibleButAlwaysSpawn.
- run heading (task-031-02): miner (flow-field goal) location sampled every tick; heading = movement over `HeadingWindowSeconds`; below `HeadingMinSpeed` = still. While running, each lone robot, director pack and horde side draws a sector around the miner (`ERRSpawnSector` Front within `FrontHalfAngle` of the heading, Back within `BackHalfAngle` of its reverse, Side between; weights `FrontSectorWeight` / `SideSectorWeight` / `BackSectorWeight`, empty sectors left out), then spawns there: lone = random cell of the sector; pack and horde = side of a random sector cell (unused sides with room first), the pack anchored on that cell. Still: uniform as before. The older `TrySpawnPack` ignores the heading. Measured ([[sys-automation-tests]]): running, 600 lone robots 56.0 / 34.7 / 9.3 %, 150 packs 56.7 / 35.3 / 8.0 %, horde back side 4 %; still, sector shares match the cell shares.
- view rectangle (world XY bounding box of the four frustum corner rays projected on the floor plane; rays above the horizon clamped to a 0.05 downward slope), first source available: `SetViewRectangleOverride` (tests; `ClearViewRectangleOverride`); the first player controller's camera manager (`GetCameraCacheView`, viewport aspect ratio); the first `UCameraComponent` of the flow-field goal actor (headless worlds). Miner camera on cell (40, 40) of the 200 uu grid: X [7217, 10223], Y [5312, 10888] (same world point as cell (121, 121) at 67 uu).

## Relations
- [[sys-spawner]] — parent hub: summary and verdict.
- [[sys-flow-field]] — referenced above.
- [[sys-automation-tests]] — referenced above.
