---
id: sys-upgrades
description: "Upgrades as built: fixed value table per stat, target and rarity, luck-weighted rarity draw, targeted vs global application, effect pools."
parent: systems-index
kind: doc
---
# Upgrades

- responsibility: build the 3-card offer and apply the picked upgrade.
- main_classes: open (offer builder, upgrade GEs per stat).

## Rules
- Value table: one CSV row per (stat, target, rarity) with a constant value and the GAS operation of the row (`MultiplyAdditive` for %, `AddBase` for points). Applied through SetByCaller. Values: open (balance agent).
- Targets: one weapon (targeted), all weapons (global, broadcast to every weapon ASC and replayed on acquisition, see [[sys-weapons]]), or the player ASC (player stats).
- Effect cards come only from the weapon's effect pool and are always yellow.
- Rarity draw (provisional): weight = base weight × (1 + k × `Luck`)^rank, rank 0 (grey) to 4 (yellow), normalized. A bonus chest adds a temporary luck bonus to its own draw only.
- Upgrades are never removed during a run; the list of picked upgrades feeds replay, the stat sheet and the run log.

- data: upgrade value table CSV; rarity base weights and `k`.
- depends_on: [[sys-gas-core]], [[sys-weapons]].
- implements: [[upgrades]], [[rarities]], [[player-stats]].
- gotchas: a missing SetByCaller returns 0 and logs an error ([[gas-api-notes]]).

## Relations
- [[systems-index]] — parent index.
- [[sys-status-effects]] — stacking arithmetic.
