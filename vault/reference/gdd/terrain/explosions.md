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
  - terrain: tuned to break an ore block in one hit;
  - characters: hurts robots and the miner, never a one-shot on the miner.
- Chain reactions (barrel → kamikaze → barrel) resolve one link at a time, with a 0.05–0.1 s delay between links: provisional. The delay also serves juice (P1).
- An explosion source explodes only once.
- Radius and damage values: open.

## Relations
- [[terrain-index]] — parent index.
- [[sys-explosions]] — the explosion queue as built.
