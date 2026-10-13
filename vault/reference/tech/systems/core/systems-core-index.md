---
id: systems-core-index
description: "Player, GAS, damage, progression, upgrades, cheats: the miner and its numbers."
parent: systems-index
kind: note
---
# C++ systems — core

Player, GAS, damage, progression, upgrades, cheats: the miner and its numbers.

## Relations
- [[systems-index]] — parent index.
- [[sys-gas-core]] — which actors own an ASC, AttributeSets, plugin setup, debug tools. Read first for any GAS work.
- [[sys-player]] — miner pawn (ASC, base stats, camera, move and aim), player controller input routing, game mode.
- [[sys-damage]] — damage chain from shot to death, i-frames, death guard.
- [[sys-status-effects]] — stacking rules for bonuses and statuses, enemy stun.
- [[sys-progression]] — miner XP and level: XPCurve.csv, XPGain scaling, carry-over, XP and level-up events.
- [[sys-level-up]] — level-up flow: heal, effect event, delayed pause on the upgrade cards, chained screens, Blueprint API to bind.
- [[sys-upgrades]] — fixed value table, luck-weighted rarity draw, targeted vs global application.
- [[sys-difficulty]] — alert and threat counters, enemy difficulty GE at spawn.
- [[sys-cheats]] — cheats (XP, invulnerable, cards, alert speed, console, extraction): polled URRCheatSettings properties for MCP, plus rr.Cheat.* console variables.
- [[sys-world-queries]] — RRWorld lookups: the arena grid and the (living) miner of a world.
- [[gas-engine-facts]] — GAS 5.8 behaviors read in engine source that fix project rules.
- [[gas-api-notes]] — GAS 5.8 APIs used by the code skeletons, with source lines.
