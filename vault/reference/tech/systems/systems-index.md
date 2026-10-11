---
id: systems-index
description: "One doc per C++ system (GAS core, player, progression, damage, weapons, upgrades, auto-fire, projectiles, explosions, AI, grid, pickups, tests) plus verified GAS 5.8 facts."
parent: tech-index
kind: note
---
# C++ systems

Decision status inside each doc: approved, provisional, deferred or open. Planned, not written yet: sys-logging.

## Relations
- [[tech-index]] — parent index.
- [[sys-gas-core]] — which actors own an ASC, AttributeSets, plugin setup, debug tools. Read first for any GAS work.
- [[sys-player]] — miner pawn (ASC, base stats, camera, move and aim), player controller input routing, game mode.
- [[sys-hud]] — HUD widget C++ base: health, XP, level-up, upgrade choice and game-over display events, created by the player controller.
- [[sys-progression]] — miner XP and level: XPCurve.csv, XPGain scaling, carry-over, XP and level-up events.
- [[sys-level-up]] — level-up flow: heal, effect event, delayed pause on the upgrade cards, chained screens, Blueprint API to bind.
- [[sys-drone]] — helper drone actor spawned by the miner: orbit, contact arc dig (selection, speeds, per-block events), hack, MiningSpeed read.
- [[sys-damage]] — damage chain from shot to death, i-frames, death guard.
- [[sys-status-effects]] — stacking rules for bonuses and statuses, enemy stun.
- [[sys-weapons]] — weapon ASC lifecycle, shotgun cone, magazine and reload, turret launcher and turrets, impact effects.
- [[sys-turret]] — turret launcher and placed turrets: button placement, charges, recharge, stat copy, targeting with grid sight.
- [[sys-upgrades]] — fixed value table, luck-weighted rarity draw, targeted vs global application.
- [[sys-difficulty]] — alert and threat counters, enemy difficulty GE at spawn.
- [[sys-objectives]] — run flow (alert bar, console drop and pause, extraction-ready event) and the console objective with its Blueprint events.
- [[sys-auto-fire]] — auto-fire component and burst timing formulas.
- [[sys-projectiles]] — projectiles without GAS, piercing, collision, pooling.
- [[sys-explosions]] — explosion queue and chain reactions.
- [[sys-enemy-ai]] — AI framework choice, pathfinding, line of sight, stun reaction.
- [[sys-flow-field]] — flow field path provider and generic enemy movement component as built.
- [[sys-enemies]] — robots index: base and shared state-machine helpers, light movement, chaser, kamikaze, EnemyStats.csv.
- [[sys-world-queries]] — RRWorld lookups: the arena grid and the (living) miner of a world.
- [[sys-spawner]] — enemy spawner actor: spawn band outside the camera view, alive cap, recycling, tunables; its child sys-spawner-director holds the wave director (phases, packs, hordes, overtime, last-level profile).
- [[sys-grid]] — grid data, autotiling, instanced rendering, terrain damage.
- [[sys-pickups]] — ore, heal and XP pickups: drop rule, pop, attraction within PickupRadius, collection event, XP grant.
- [[sys-actor-pool]] — generic actor pool subsystem: reuse, creation spread over MaxFrames, IRRPoolable reset hooks, IsPooledIdle filter.
- [[sys-damage-numbers]] — damage number subsystem: one number per hit with a spawn disc (no merge since task-059), stacking, Niagara user parameters contract, player toggle.
- [[sys-stress]] — stress test tooling: rr.Stress.* cheat commands, bulk spawn around the miner, timed frame-time run.
- [[sys-text-content]] — runtime loader of the A4 text files: upgrade flavor, alert lines, barks, fallback, hooks.
- [[sys-cheats]] — cheats (XP, invulnerable, cards, alert speed, console, extraction): polled URRCheatSettings properties for MCP, plus rr.Cheat.* console variables.
- [[sys-automation-tests]] — headless C++ automation tests (RogueRigs.Dig.*, RogueRigs.Weapons.*): test world harness, what each test proves, gotchas.
- [[gas-engine-facts]] — GAS 5.8 behaviors read in engine source that fix project rules.
- [[gas-api-notes]] — GAS 5.8 APIs used by the code skeletons, with source lines.
