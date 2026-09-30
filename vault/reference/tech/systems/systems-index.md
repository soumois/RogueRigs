---
id: systems-index
description: "One doc per C++ system (GAS core, damage, status effects, weapons, auto-fire, projectiles, explosions, enemy AI, grid) plus verified GAS 5.8 facts."
parent: tech-index
kind: note
---
# C++ systems

Decision status inside each doc: approved, provisional, deferred or open. Planned, not written yet: sys-drone, sys-run-flow, sys-logging.

## Relations
- [[tech-index]] — parent index.
- [[sys-gas-core]] — which actors own an ASC, AttributeSets, plugin setup, debug tools. Read first for any GAS work.
- [[sys-damage]] — damage chain from shot to death, i-frames, death guard.
- [[sys-status-effects]] — stacking rules for bonuses and statuses, enemy stun.
- [[sys-weapons]] — weapon ASC lifecycle, upgrade broadcast, magazine, turrets, impact effects.
- [[sys-auto-fire]] — auto-fire component and burst timing formulas.
- [[sys-projectiles]] — projectiles without GAS, piercing, collision, pooling.
- [[sys-explosions]] — explosion queue and chain reactions.
- [[sys-enemy-ai]] — AI framework choice, pathfinding, line of sight, stun reaction.
- [[sys-grid]] — grid data, autotiling, instanced rendering, terrain damage.
- [[gas-engine-facts]] — GAS 5.8 behaviors read in engine source that fix project rules.
- [[gas-api-notes]] — GAS 5.8 APIs used by the code skeletons, with source lines.
