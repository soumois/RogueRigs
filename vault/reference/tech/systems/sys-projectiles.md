---
id: sys-projectiles
description: "Projectiles without GAS: carry effect specs to impact, pierce counter, Projectile collision channel, pooling, deferred behavior modules."
parent: systems-index
kind: doc
---
# Projectiles

- responsibility: carry shot data from the weapon to the impact.
- main_classes: projectile actor; no ASC (approved, for now).

## Rules
- The projectile carries the specs created by the weapon's ASC at fire time and applies them only on impact ([[sys-damage]]).
- Piercing: a `PierceRemaining` counter on the projectile, copied from the weapon attribute at fire time (provisional).
- Collision: a `Projectile` channel that overlaps pawns and blocks world geometry (provisional). The template projectile blocks all channels and dies on the first enemy hit.
- Every projectile stops on rock ([[destruction-rules]]).
- Pool projectiles before any enemy pool: they are born and destroyed by the hundreds (provisional; M2, if stress test S2 asks for it).
- The template sets `bForceSubStepping = true`: turn sub-stepping off unless needed.
- Bounce, homing, tornado, lightning, explode on impact: deferred with the module framework ([[wishlist]]).

- data: none of its own; values come from the weapon.
- depends_on: [[sys-weapons]].
- implements: [[burst-rifle]], [[shotgun]], [[turret]], [[enemy-shooter]].
- gotchas: see [[perf-risks]].

## Relations
- [[systems-index]] — parent index.
