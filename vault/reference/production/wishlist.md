---
id: wishlist
description: "Every idea cut or deferred from the capstone scope, with where it came from. Open before proposing a new feature."
parent: production-index
kind: note
---
# Wishlist

## Game design
- Weapon tags (light, medium, projectile, area…) with tag-targeted upgrades, from DRGS: out of the prototype scope with only 3 weapons (owner 2026-10-05).
- Meta-progression shop spending ore ([[ore-economy]]).
- Per-weapon-type upgrades (laser, physical projectile, light weapon…).
- Grenades, with limited charges.
- Glowing weak point on the heavy robot's back (rewards manual aim).
- Boss objective, with a boss entity still to define.
- Player armor stat.
- Robots knocked into the void by explosions or shotgun knockback.
- Ice, burn/DoT, other elements and immunities, elemental reactions (explosion, lightning, tornado).
- Projectile behaviors: bounce, homing, explode on impact.

## Tech and tools
- Projectile behavior module framework (`Projectile.Behavior.*` tags, tag → module table, `OnHit` / `OnWallHit` / `Tick` hooks, no behavior rules in the projectile class).
- Dynamic GEs for upgrades (option B).
- Slate debug tool (and the `RogueRigsEditor` module it needs).
- Enemy pooling, unless a measurement shows spawn or GC spikes.

## Agents
- Adversarial QA agent.

## Relations
- [[production-index]] — parent index.
- [[milestones]] — the scope these were cut from.
