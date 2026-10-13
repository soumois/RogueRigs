---
id: explosions
description: "Explosion rules: separate terrain and character damage, no one-shot on the miner, chain reactions with a short delay."
parent: terrain-index
kind: spec
status: approved
---
# Explosions

- Sources: [[barrel]], [[enemy-kamikaze]].
- Every explosion has two damage values:
  - terrain: none; every block inside the radius is destroyed outright, whatever its type or health (owner decision, 2026-10-01);
  - characters: hurts robots and the miner, never a one-shot on the miner.
- Chain reactions (barrel → kamikaze → barrel) resolve one link at a time, with a 0.08 s delay between links (provisional, owner 2026-10-02). The delay also serves juice (P1).
- An explosion source explodes only once.
- Radius and damage per source: [[barrel]], [[enemy-kamikaze]]. Grey-box look (M1): an orange sphere that grows to the radius and fades in 0.25 s.

- Visible explosion (owner 2026-10-10): the visible part is about 5 % larger than the collision radius, so the player is never hit outside what is shown, and it stays 0.1 s at its final state before fading, so it can be seen.

- Barrel fuse (owner 2026-10-12): a barrel explodes 1.5 s after its health reaches 0, so the 0.08 s link delay of a chain becomes the barrel fuse for barrels; the kamikaze rules are unchanged ([[barrel]], [[enemy-kamikaze]]).

## Relations
- [[terrain-index]] — parent index.
- [[sys-explosions]] — the explosion queue as built.
