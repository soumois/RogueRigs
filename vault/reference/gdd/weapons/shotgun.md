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
- stats: damage, fire rate, cone angle, range, knockback force, magazine size, reload time → open; crit damage ×1.5 base.
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool: none (piercing and 4-direction fire do not apply).
- vfx_sfx: white-gold wave ([[color-code]]); sound open.
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[wishlist]] — knockback into the void.
