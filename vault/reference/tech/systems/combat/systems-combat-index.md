---
id: systems-combat-index
description: "Weapons, projectiles, explosions, drone, turret, ground telegraph, damage numbers: how the player fights."
parent: systems-index
kind: note
---
# C++ systems — combat

Weapons, projectiles, explosions, drone, turret, ground telegraph, damage numbers: how the player fights.

## Relations
- [[systems-index]] — parent index.
- [[sys-weapons]] — weapon ASC lifecycle, shotgun cone, magazine and reload, turret launcher and turrets, impact effects.
- [[sys-auto-fire]] — auto-fire component and burst timing formulas.
- [[sys-projectiles]] — projectiles without GAS, piercing, collision, pooling.
- [[sys-turret]] — turret launcher and placed turrets: button placement, charges, recharge, stat copy, targeting with grid sight.
- [[sys-explosions]] — explosion queue and chain reactions.
- [[sys-drone]] — helper drone actor spawned by the miner: orbit, contact arc dig (selection, speeds, per-block events), hack, MiningSpeed read.
- [[sys-ground-telegraph]] — ground circle decal component: API, material parameter names (Radius, Progress, ZoneColor, ProgressColor, Opacity), hiding rules.
- [[sys-damage-numbers]] — damage number subsystem: one number per hit with a spawn disc (no merge since task-059), stacking, Niagara user parameters contract, player toggle.
- [[sys-damage-numbers-niagara]] — system
