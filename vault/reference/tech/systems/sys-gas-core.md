---
id: sys-gas-core
description: "GAS foundation: one ASC per player, weapon, turret and enemy, none on projectiles; AttributeSets, ability base class, plugin setup, debug commands."
parent: systems-index
kind: doc
---
# GAS core

- responsibility: owns the ability system layout shared by every combat actor.
- main_classes:
  - ASC on the player, on each weapon, on each turret, on each enemy pawn (approved). None on projectiles (approved, for now).
  - Enemy ASC lives on the pawn itself; no replication (single-player).
  - AttributeSets `URRHealthAttributeSet`, `URRMovementAttributeSet`, `URRWeaponAttributeSet` (naming provisional). The health set is shared by the player and enemies and holds the `IncomingDamage` meta-attribute (provisional).
  - `URRGameplayAbility`: sets `InstancedPerActor` in its constructor (5.8 default is `InstancedPerExecution`; `NonInstanced` is deprecated).
- entry_points: `InitAbilityActorInfo` at spawn or acquisition; GEs for every stat change.
- data: base values from CSV loaded into DataTables at runtime (approved); Curve Tables and hot reload in M2 (provisional).
- depends_on: plugin GameplayAbilities, modules `GameplayAbilities`, `GameplayTags`, `GameplayTasks` ([[task-000-enable-gas]]).
- implements: [[upgrades]], [[miner]], [[enemies-index]].
- gotchas:
  - Clamp base values in `PreAttributeBaseChange`; if `MaxHealth` drops, bring `Health` back in `PostAttributeChange`.
  - A missing SetByCaller returns 0 and logs an error.
  - Debug with native tools, no custom tool: `showdebug abilitysystem`, `AbilitySystem.Debug.NextCategory`, `AbilitySystem.DebugAttribute <attrs>`, `AbilityListGranted`, `EffectListActive`.
  - Cost at 150–200 enemies is expected in movement, animation, navmesh and rendering, not in GAS ([[perf-risks]]).

## Relations
- [[systems-index]] — parent index.
- [[gas-engine-facts]] — engine behaviors behind these rules.
- [[naming]] — class and tag names.
- [[gas-scope]] — hours per milestone.
