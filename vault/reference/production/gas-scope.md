---
id: gas-scope
description: "Hours of GAS work per milestone: the reduced M1 scope (about 9 h), M2 and M3 additions, and what moved to the wishlist."
parent: production-index
kind: spec
status: provisional
---
# GAS scope per milestone

Hours are orders of magnitude, human integration and debugging included.

| M1 item | Hours | Scope |
|---|---|---|
| Plugin, native tags, custom ASC, AttributeSets | 1 | No custom logging: GAS already writes to the Visual Logger |
| Weapon ASC, registry, acquisition, upgrade broadcast and replay | 1 | No upgrade removal |
| Auto-fire, bursts, magazine, reload | 1 | `URRAutoFireComponent` + 3 GEs |
| Damage: ExecCalc, critical, flat GE, death, cue | 1.5 | `IncomingDamage`, death guard, cue on the GE |
| Upgrades, card types | 1.5 | Stats only |
| Enemy abilities (melee, explosion) | 1 | Explosion through the queue |
| CSV data | 0.5 | CSV → DataTable at runtime, minimal validation |
| Debug tool | 0 | Native tools only |
| Stress test | 1.5 | S0, S2, S7 in `-game` |
| **Total** | **9** | Initial estimate was 19–23 h |

- **M2 (about 4 h):** turrets by attribute copy (1), piercing and 4-direction (1), rarities through SetByCaller (0.5), shooter reusing projectile and flat damage GE (1), projectile pool if S2 needs it (0.5).
- **M3 if time allows:** EMP as a stun GE with ability cancel (1 h).
- Moved to [[wishlist]]: burn/DoT, ice, elements and immunities, reactions, bounce and homing, projectile module framework, dynamic GEs, Slate debug tool, enemy pooling.

## Relations
- [[production-index]] — parent index.
- [[milestones]] — the milestone plan.
- [[sys-gas-core]] — the architecture being scoped.
