---
id: naming
description: "Naming conventions: RR class prefix, GE and GA prefixes, AttributeSet names, GameplayTag roots in use, single English weapon names."
parent: conventions-index
kind: doc
---
# Naming

- Project class prefix: `RR` (`URR…`, `ARR…`).
- Gameplay Effects: `URRGE_<Name>` (e.g. `URRGE_Damage`, `URRGE_DamageFlat`, `URRGE_IFrames`).
- Gameplay Abilities: `URRGA_<Name>` (e.g. `URRGA_OnDamaged`).
- AttributeSets: `URR<Domain>AttributeSet` — `URRHealthAttributeSet`, `URRMovementAttributeSet`, `URRPlayerAttributeSet`, `URRWeaponAttributeSet`, `URREnemyAttributeSet` (provisional).
- Native tags declared with `UE_DECLARE_GAMEPLAY_TAG_EXTERN` / `UE_DEFINE_GAMEPLAY_TAG`.

## GameplayTag roots in use
| Root | Examples | Use |
|---|---|---|
| `State.` | `State.Dead`, `State.Invulnerable`, `State.Stunned` | actor state |
| `Ability.` | `Ability.Attack` | ability families (cancel, block) |
| `Weapon.Player.` | per weapon | upgrade routing |
| `Effect.Damage.` | `Effect.Damage.DoT` | damage kinds |
| `Projectile.Behavior.` | deferred | behavior modules ([[wishlist]]) |

- Full tag hierarchy: open.
- One English name per weapon, shared by tag, asset and vault node; the burst rifle's final name is open ([[burst-rifle]]).

## Relations
- [[conventions-index]] — parent index.
- [[sys-gas-core]] — where these classes live.
