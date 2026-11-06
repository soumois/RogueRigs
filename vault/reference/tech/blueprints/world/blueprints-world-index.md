---
id: blueprints-world-index
description: "Arena grid, objective console, bonus chest, shuttle, spawner and barrel asset: the placed world Blueprints."
parent: blueprints-index
kind: note
---
# Blueprints — world

Arena grid, objective console, bonus chest, shuttle, spawner and barrel asset: the placed world Blueprints.

## Children
- [[bp-arena-grid]] — BP_ArenaGrid doc, the M1 arena grid Blueprint.
- [[bp-objective-console]] — BP_ObjectiveConsole, the command console.
- [[bp-bonus-chest]] — BP_BonusChest, the optional gold bonus chest.
- [[bp-extraction-shuttle]] — BP_ExtractionShuttle, the extraction shuttle (extractor body, four rotors, chevrons).
- [[bp-enemy-spawner]] — BP_EnemySpawner, the enemy spawner placed in the arena.
- [[bp-rock-slab-material]] — M_RockSlab and MI_RockSlab, the rock piece material (top texture, cracks, shake, broken sag).
- [[bp-floor-plates-material]] — the floor material: M_FloorCartoon with the owner painted flagstones (task-470), M_FloorPlates kept unused, revert.
- [[bp-space-background-material]] — M_SpaceBackground and its three instances (procedural stars, nebula, dust; nebula plane replaced in task-335), parameters, tuning, revert.
- [[bp-space-texture-material]] — M_SpaceTexture, MI_Space_Deep and MI_Space_Clouds, generated-texture void layers: parameters, final intensities, capture finding.
- [[bp-space-background]] — BP_SpaceBackground, the four planes under the arena: depths, scales, placement, removal.
- [[bp-asteroid-field]] — BP_AsteroidField, 12 rotating low-poly asteroids under the arena: positions table, BeginPlay fix, placement, removal.
- [[bp-asteroid-material]] — M_Asteroid, MI_Asteroid and SM_Asteroid_01..04: parameters, import settings, reimport trap, revert.
- [[ppv-xp-glow]] — PPV_XPGlow, the PostProcessVolume of L_ArenaM1 that sets the bloom (halo) of emissive elements; open to tune it.
- [[mpp-player-light-mask]] — provisional: MPC_PlayerLight and M_PP_PlayerLightMask, the radial post-process light mask around the player with its own far gain for the void (task-272-06, task-335-05).
- [[lights-no-shadows]] — provisional: shadow casting off on the sun and sky light of L_ArenaM1 (owner 2026-10-20); open for the revert steps.
- [[lights-dark-ambient]] — provisional: dark violet ambient, sun 1.0 and sky light 0.5 tinted violet (owner 2026-10-20, task-265); open for previous values.

## Relations
- [[blueprints-index]] — parent index.
- [[barrel-asset]] — there is NO barrel Blueprint: barrels are a grid cell type drawn by BP_ArenaGrid; open this for the barrel mesh, materials and integration.
