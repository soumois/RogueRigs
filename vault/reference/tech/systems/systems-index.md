---
id: systems-index
description: "One doc per C++ system (GAS core, damage, statuses, weapons, upgrades, difficulty, auto-fire, projectiles, explosions, AI, grid, pickups, tests) plus verified GAS 5.8 facts."
parent: tech-index
kind: note
---
# C++ systems

Decision status inside each doc: approved, provisional, deferred or open. Planned, not written yet: sys-run-flow (beyond the M1 game over in sys-player), sys-logging.

## Relations
- [[tech-index]] — parent index.
- [[sys-gas-core]] — which actors own an ASC, AttributeSets, plugin setup, debug tools. Read first for any GAS work.
- [[sys-player]] — miner pawn (ASC, base stats, camera, move and aim), player controller input routing, game mode.
- [[sys-hud]] — HUD widget C++ base: health and game-over display events, created by the player controller.
- [[sys-drone]] — helper drone actor spawned by the miner: state enum, orbit tunables, MiningSpeed read.
- [[sys-damage]] — damage chain from shot to death, i-frames, death guard.
- [[sys-status-effects]] — stacking rules for bonuses and statuses, enemy stun.
- [[sys-weapons]] — weapon ASC lifecycle, shotgun cone, magazine and reload, turret launcher and turrets, impact effects.
- [[sys-upgrades]] — fixed value table, luck-weighted rarity draw, targeted vs global application.
- [[sys-difficulty]] — alert and threat counters, enemy difficulty GE at spawn.
- [[sys-auto-fire]] — auto-fire component and burst timing formulas.
- [[sys-projectiles]] — projectiles without GAS, piercing, collision, pooling.
- [[sys-explosions]] — explosion queue and chain reactions.
- [[sys-enemy-ai]] — AI framework choice, pathfinding, line of sight, stun reaction.
- [[sys-grid]] — grid data, autotiling, instanced rendering, terrain damage.
- [[sys-pickups]] — ore and heal pickups: drop rule, pop, attraction within PickupRadius, collection event.
- [[sys-target-dummy]] — debug target dummy: never dies, hit flash, M1 floating damage numbers, health refill.
- [[sys-automation-tests]] — headless C++ automation tests (RogueRigs.Dig.*, RogueRigs.Weapons.*): test world harness, what each test proves, gotchas.
- [[gas-engine-facts]] — GAS 5.8 behaviors read in engine source that fix project rules.
- [[gas-api-notes]] — GAS 5.8 APIs used by the code skeletons, with source lines.
