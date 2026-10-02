---
id: sys-target-dummy
description: "Debug target dummy: immobile ASC actor that never dies, hit flash, M1 floating damage numbers, health refill. Open to test weapons or hit feedback."
parent: systems-index
kind: doc
---
# Target dummy (debug)

- responsibility: a punching ball to test weapons, damage and hit feedback in M1.
- main_classes (in `Source/RogueRigs/Debug/`):
  - `ARRTargetDummy` (abstract `AActor` + `IAbilitySystemInterface`): root `UStaticMeshComponent` `Mesh`, `URRAbilitySystemComponent`, `URRHealthAttributeSet`. No tick.
  - `ARRFloatingDamageText`: one short-lived actor per hit; `UTextRenderComponent` showing the rounded damage, rises at `RiseSpeed` (150 uu/s), fades its color alpha over `Lifetime` (0.8 s), faces the active player camera every tick, then destroys itself. A second text render `OutlineRender` (attached to `TextRender`, same value) draws a near-black outline/drop shadow behind it for readability: `OutlineColor` (0.02, 0.02, 0.02), world size × `OutlineSizeScale` 1.12, relative `OutlineOffset` (-2, 2, -3) (X negative = behind, away from the camera), `TranslucencySortPriority` -1, same material and same alpha fade. `TextMaterial` defaults to the engine `DefaultTextMaterialTranslucent` (the default opaque text material ignores alpha).
- Collision: `Mesh` object type Pawn, blocks every channel except `Projectile` (overlap), overlap events on. The projectile sphere overlaps Pawn, so the pair overlaps and the projectile applies its spec ([[sys-projectiles]]).
- Hit flow: `URRHealthAttributeSet::OnDamageReceived` (damage, crit, health left; see [[sys-damage]]) → log on `LogRogueRigs` (damage, crit, health left / max), flash, floating number, refill scheduling.
- Flash: dynamic instance of the mesh material slot 0 created at `BeginPlay`; vector parameter `ColorParameterName` ("Color" on `M_GreyBox`) set to `FlashColor` for `FlashDuration`, then restored to its value read at `BeginPlay`. Missing material or parameter logs a warning.
- Health: `MaxHealth` and `Health` set to `MaxHealth` at `BeginPlay`. `RefillHealth` sets `Health` to the current `MaxHealth` `RefillDelay` after the last hit (timer restarted per hit), instantly when `Health` reaches 0. Never dies, never gets `State.Dead` (`SetDeathEnabled(false)` on its health set).
- Editable (`EditAnywhere`): `MaxHealth` 1000, `RefillDelay` 3 s, `FlashDuration` 0.1 s, `FlashColor` white, `ColorParameterName` `Color`, `FloatingTextClass` (native class by default), `TextSpawnOffset` (0, 0, 150), `TextSpawnJitter` 25, `NormalTextColor` white, `CriticalTextColor` (1, 0.85, 0.45), `NormalTextSize` 60, `CriticalTextSize` 90 (owner feel feedback, task-008-02). Text class (`EditDefaultsOnly`): `TextMaterial`, `Lifetime`, `RiseSpeed`, `OutlineColor`, `OutlineSizeScale`, `OutlineOffset`.
- entry_points: place a Blueprint subclass (sets `Mesh` static mesh and an `M_GreyBox`-based material); damage arrives through any GE writing `IncomingDamage`.
- data: none; all values are editable properties.
- depends_on: [[sys-gas-core]], [[sys-damage]], [[sys-projectiles]].
- implements: [[damage-numbers]] (M1 debug version, colors from [[color-code]]).
- gotchas:
  - One actor spawned per hit: debug only, not for horde scale (M2 uses Niagara). No merging window.
  - Text faces the camera of the first player controller; no camera, no rotation update.
  - Flash and refill run inside the GE execution call stack (attribute set delegate); setting `Health` there is safe.

## Relations
- [[systems-index]] — parent index.
- [[sys-damage]] — the per-hit damage event the dummy listens to.
- [[damage-numbers]] — the feedback this debug version stands in for.
- [[sys-automation-tests]] — RogueRigs.Weapons.ProjectileDamagesDummy and DummyRefill prove its damage and refill.
