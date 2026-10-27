---
id: task-334-04-asteroid-field
description: "Build BP_AsteroidField with the background asteroids at their positions, depths and slow rotations, and place one instance in L_ArenaM1."
parent: task-334-near-asteroids
kind: task
status: in-progress
---
# Asteroids: field actor and placement

- goal: one data-only actor holds the asteroids around the arena per A3, A4, A5 and A7, placed and saved in L_ArenaM1.
- agent: bp
- depends_on: task-334-03-asteroid-import
- inputs: the global `request` and `gdd_check` with the owner answers A0 to A7 (vault/reference/tasks/task-334-near-asteroids.md); the task-334-03 change summary; bp-space-background (vault/reference/tech/blueprints/world/bp-space-background.md, same actor pattern, plane depths, camera numbers); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, map, arena origin, cell size); level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md, where the arena_m1 layout file is and how void cells are coded); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: /Game/RogueRigs/Environment/Space/BP_AsteroidField (AActor, no graph logic): per A3 (default 12) StaticMeshComponents using SM_Asteroid_01 to 04 with varied scale (200 to 1200 uu across) and rotation, each with a RotatingMovement component at 1 to 5 deg/s on varied axes (A4); all Movable, NoCollision, CastShadow off, bReceivesDecals off; every asteroid top below Z -300 and centres over void cells only, 300 to 3000 uu outside the floor edge, spread around the whole edge, positions computed from the layout void cells (method and the list of positions in the change summary); one instance in /Game/RogueRigs/Maps/L_ArenaM1 at the origin (0, 0, 0). Change summary for the doc subtask: components with mesh, location, scale, rotation rate, placement, commit hash. No vault doc node.
- done_when: the Blueprint compiles and is saved; the map is saved and, after reload, shows one BP_AsteroidField with the expected component count; every component top Z below -300 and no component centre over a floor cell (checked against the layout, result in the summary); collision and shadow flags read back; committed by explicit paths.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T23:40:39Z
- waiting_for: none
- result: open

## Relations
- [[task-334-near-asteroids]] — parent global task.
