---
id: shotgun
description: "Shotgun: instant conical wave that hits every enemy in the cone with line of sight, one crit roll each, knockback outward."
parent: weapons-index
kind: spec
status: approved
---
# Shotgun

- fire_mode: auto toward aim; each shot is an instant conical wave (no pellets, no projectile).
- hit rule: every enemy inside the cone is hit at once, if the line of sight between the miner and that enemy is clear on the grid. The wave's visual effect disappears into rock.
- effect_on_hit: damage with no falloff over distance; one critical roll per enemy hit; knockback outward from the miner, reduced by the target's knockback resistance.
- blocked_by_rock: yes (per-enemy line of sight)
- stats (provisional, owner 2026-10-03): damage 18 per enemy hit, fire rate 0.8 shots/s, cone 60°, range 700 uu (3.5 cells), knockback force 600, magazine 6, reload 2 s; crit chance 5 % (same as [[burst-rifle]]); crit damage ×1.5 base.
- M1: the miner starts with the shotgun and the burst rifle (owner 2026-10-03).
- Under review (owner 2026-10-04): knockback too weak and the weapon too strong overall; the owner is rethinking how it works. No change until decided.
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool: none (piercing and 4-direction fire do not apply).
- vfx_sfx: white-gold wave ([[color-code]]); sound open.
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[wishlist]] — knockback into the void.
