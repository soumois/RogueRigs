---
id: gdd-pillars
description: "Rogue Rigs pitch, the no-runtime-AI constraint and the three approved design pillars with their veto tests. Open to judge whether a feature is worth keeping."
parent: gdd-index
kind: note
---
# Pitch and design pillars

**Pitch.** A top-down roguelite auto-shooter. The player is an independent miner stealing ore from a corporate claim on an asteroid before extraction begins. The arena itself is the player's best weapon.

**Hard constraint.** No AI runs at runtime. Runtime level generation is a deterministic C++ algorithm.

## Design pillars (approved)
Every feature must pass at least one test. A feature that passes none is cut.
- **P1 Gamefeel / juice** — passes if it adds feedback the player feels, not just a number.
- **P2 Manual aim + auto-fire** — weapons fire on their own; passes if the player decides where it happens.
- **P3 The arena is a weapon** — passes if the terrain changes how a fight plays out.

## Relations
- [[gdd-index]] — parent index.
- [[gdd-glossary]] — the terms these pillars and the other nodes use.
