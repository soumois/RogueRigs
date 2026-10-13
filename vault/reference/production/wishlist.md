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
- Ore scattered through the rock (owner 2026-10-08, next task, after the DRGS video): nuggets embedded in rock blocks so ore drips during the whole dig (about 3 chunks per second in DRGS) instead of 3 × 3 ore veins.

## Art
- Block faces that depend on neighbours (owner, after handover-2026-10-06; for the final block assets): a face with no neighbouring block gets a decoration layer so the wall is not smooth. One mesh variant per count of free faces (1, 2, 3, 4), rotated per instance toward the free sides instead of one mesh per direction; 2 free faces needs a corner and an opposite variant. Later: cracked blocks near the wall edge.
- Done (task-145-03, 2026-10-13): XP pickup ground glow, see [[bp-pickup-xp]] (original wish, owner 2026-10-13, set aside): a flat additive radial sprite under each XP nut and big chip, cyan or magenta, no shadow, one shared material, a quad component in the pickup Blueprints; only if the bloom and the emissive pulse alone are not enough. A real point light per pickup is rejected (Lumen and virtual shadow maps are on).

## Tech and tools
- Projectile behavior module framework (`Projectile.Behavior.*` tags, tag → module table, `OnHit` / `OnWallHit` / `Tick` hooks, no behavior rules in the projectile class).
- Dynamic GEs for upgrades (option B).
- Slate debug tool (and the `RogueRigsEditor` module it needs).
- Enemy pooling, unless a measurement shows spawn or GC spikes.
- Done (task-138-02, 2026-10-13): Cost check of the XP pickup glow (owner 2026-10-13): game-thread and GPU frame time with a burst of nuts and chips, with and without bloom and the emissive pulse; no measurable cost up to 400, see [[perf-xp-pickups-glow]].

## Agents
- Adversarial QA agent.

## Relations
- [[production-index]] — parent index.
- [[milestones]] — the scope these were cut from.
- [[perf-xp-pickups-glow]] — result of the XP pickup glow cost check.
- [[bp-pickup-xp]] — the XP pickups, with their ground glow disc.
