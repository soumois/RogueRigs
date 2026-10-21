---
id: blueprints-world-index
description: "Arena grid, objective console, bonus chest, shuttle, spawner and barrel asset: the placed world Blueprints."
parent: blueprints-index
kind: note
---
# Blueprints — world

Arena grid, objective console, bonus chest, shuttle, spawner and barrel asset: the placed world Blueprints.

## Relations
- [[blueprints-index]] — parent index.
- [[bp-arena-grid]] — BP_ArenaGrid doc, the M1 arena grid Blueprint.
- [[bp-objective-console]] — BP_ObjectiveConsole, the command console.
- [[bp-bonus-chest]] — BP_BonusChest, the optional gold bonus chest.
- [[bp-extraction-shuttle]] — BP_ExtractionShuttle, the extraction shuttle.
- [[bp-enemy-spawner]] — BP_EnemySpawner, the enemy spawner placed in the arena.
- [[bp-rock-slab-material]] — M_RockSlab and MI_RockSlab, the rock piece material (top texture, cracks, shake, broken sag).
- [[barrel-asset]] — there is NO barrel Blueprint: barrels are a grid cell type drawn by BP_ArenaGrid; open this for the barrel mesh, materials and integration.
- [[ppv-xp-glow]] — PPV_XPGlow, the PostProcessVolume of L_ArenaM1 that sets the bloom (halo) of emissive elements; open to tune it.
- [[lights-no-shadows]] — provisional: shadow casting off on the sun and sky light of L_ArenaM1 (owner 2026-10-20); open for the revert steps.
- [[lights-dark-ambient]] — provisional: dark violet ambient, sun 1.0 and sky light 0.5 tinted violet (owner 2026-10-20, task-265); open for previous values.
