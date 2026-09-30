---
id: gdd-index
description: "Game specification entry: pitch, design pillars with veto tests, glossary, and the branches for run, player, weapons, enemies, terrain, art and audio."
parent: root
kind: note
---
# Rogue Rigs — game specification

**Pitch.** A top-down roguelite auto-shooter. The player is an independent miner stealing ore from a corporate claim on an asteroid before extraction begins. The arena itself is the player's best weapon.

**Hard constraint.** No AI runs at runtime. Runtime level generation is a deterministic C++ algorithm.

## Design pillars (approved)
Every feature must pass at least one test. A feature that passes none is cut.
- **P1 Gamefeel / juice** — passes if it adds feedback the player feels, not just a number.
- **P2 Manual aim + auto-fire** — weapons fire on their own; passes if the player decides where it happens.
- **P3 The arena is a weapon** — passes if the terrain changes how a fight plays out.

## Glossary
- **Rigs**: the independent miners (player side).
- **Kessler-Voss Mining Consortium**: the corporation that owns the claim; its robots are the enemies.
- **Alert bar**: level progress bar; fills over time, faster with each robot killed.
- **Beacon**: the claim beacon the drone hacks (default objective).
- **Shuttle**: the Rigs' extraction craft; lands far from the miner with a countdown.
- **Risk window**: time between shuttle landing and countdown end.
- **Drone**: the miner's small helper; digs, hacks, carries dialogue.
- **Ore / special ore**: meta-progression resource / rare objective resource.

## Relations
- [[lore]] — world, factions, the colleague behind the drone, how lore maps onto mechanics. Open for narrative or text work.
- [[run-index]] — run structure, level flow, objectives, extraction, ore economy. Open for loop or win/loss questions.
- [[player-index]] — the miner and the drone: controls, states, stats.
- [[weapons-index]] — weapons, turrets, upgrades and rarities.
- [[enemies-index]] — the corporate robots: one node per robot, locomotion, rig, behavior, look.
- [[terrain-index]] — grid, blocks, destruction sources, ore blocks, explosions.
- [[art-audio-index]] — visual style, color code, readability rules, sound, dialogue tone.
- [[root]] — the vault map.
