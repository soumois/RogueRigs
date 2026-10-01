---
id: sys-explosions
description: "Explosion queue processed on the next tick, already-exploded flag, delay between chain links, terrain and character damage paths."
parent: systems-index
kind: doc
---
# Explosions

- responsibility: resolve every explosion and chain reaction safely.
- main_classes: `URRExplosion` (`Explode`).

## Rules (provisional)
- `Explode` never runs inside a damage callback: explosions go through a queue processed on the next tick.
- Each source has an "already exploded" flag, so a barrel hit twice explodes once.
- A 0.05–0.1 s delay between two links of a chain; it also serves juice (P1).
- A kamikaze's death adds its explosion to the queue.
- Character damage enters [[sys-damage]]; terrain damage goes to the grid through its own interface, outside GAS ([[sys-grid]]).
- Barrels have no ASC: a health value on the barrel plus the already-exploded flag. What damages a barrel (shots, explosions): open.

- data: radius, terrain damage and character damage per source, from CSV (values open).
- depends_on: [[sys-damage]], [[sys-grid]].
- implements: [[explosions]], [[barrel]], [[enemy-kamikaze]].
- gotchas: synchronous chains in damage callbacks cause recursion and double hits; that is why the queue exists.

## Relations
- [[systems-index]] — parent index.
