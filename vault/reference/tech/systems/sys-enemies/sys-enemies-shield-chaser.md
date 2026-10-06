---
id: sys-enemies-shield-chaser
description: "ARRShieldChaser as built: chaser variant whose 30-point shield absorbs damage before health; OnShieldBroken event, shield getters, director mix after 1:00."
parent: sys-enemies
kind: doc
---
# Shield chaser (`ARRShieldChaser`)

- responsibility: the shield variant of the chaser ([[enemy-chaser]]): same melee state machine, speed, damage and capsule as [[sys-enemies-chaser]]; a shield taken before health; the break event for the shell visuals.
- main_classes: `ARRShieldChaser : ARRChaser` (Blueprintable, `Enemies/RRShieldChaser.h/.cpp`, task-028-01). The shield is the `Shield` attribute of `URRHealthAttributeSet` ([[sys-damage]] step 5): each hit is taken from it first, the rest of the same hit from `Health`; it never regenerates. No tick and no per-frame cost: the robot only binds the set's native `OnShieldBroken` (BeginPlay, removed in EndPlay).
- Blueprint-facing: event `OnShieldBroken` (once, when a hit takes the shield to 0; fired inside the GE execution, after the damage number hit and before the death event when the same hit kills); pure `GetShield`, `GetMaxShield` (CSV value), `GetShieldFraction` (0 → 1, 0 without shield), `HasShield`.
- entry_points: the getters above, `GetShieldBreakCount` (tests).
- data: `StatsRowName` `ShieldChaser` (CSV in [[sys-enemies]]: chaser values, XPValue 2, Shield 30); the rest inherited from `ARRChaser` (`XPCubeCount` 3).
- spawning ([[sys-spawner-director]]): pack type `ShieldChasers` = chaser 6–12 + shield chaser 2–4 + kamikaze 2–3 (mean 3 of 12 chasers = 25 %), drawn by the phase `FlowShields` (60 s, the Flow weights with `ShieldChasers` in place of `Chasers`). Lone robots, the Heavies escort and hordes stay plain chasers (choice of task-028-01). The spawner resolves `ARRShieldChaser` to the `WeightedEnemyClasses` entry whose nearest native class is `ARRShieldChaser`, so its Blueprint must derive from `ARRShieldChaser` directly (a child of `BP_Chaser` would resolve as a chaser); without that entry the native class spawns (no mesh).
- depends_on: [[sys-enemies-chaser]], [[sys-damage]], [[sys-gas-core]].
- implements: [[enemy-chaser]] (shield variant).
- gotchas: the shield works on any robot whose row has Shield > 0, but only `ARRShieldChaser` forwards the break to a Blueprint. Explosions also hit the shield first. Tests: `RogueRigs.ShieldChaser.*` ([[sys-automation-tests]]).

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemies-chaser]] — the chaser it extends.
- [[sys-spawner-director]] — the pack type and phase that spawn it.
