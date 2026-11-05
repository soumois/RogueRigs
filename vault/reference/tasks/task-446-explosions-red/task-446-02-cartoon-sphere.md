---
id: task-446-02-cartoon-sphere
description: "Bp: The expanding sphere of the explosion has the cartoon look and still shows the radius."
parent: task-446-explosions-red
kind: task
status: done
---
# Barrel and kamikaze explosions in cartoon style: bp

- goal: The expanding sphere of the explosion has the cartoon look and still shows the radius.
- agent: bp
- depends_on: task-446-01-cartoon-burst
- inputs: bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), explosions (vault/reference/gdd/terrain/explosions.md)
- expected_output: see done_when
- done_when: Compiles and saves with no error; both rows play one class; visible radius and hold read back unchanged; the fade still works.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-446
- usage: included in the executor run of task-446
- runs: 1
- milestone: M2
- started: 2026-11-05T22:13:59Z
- waiting_for: none
- result: New M_CartoonExplosionSphere (Translucent, Unlit, one-sided, one Custom node, no texture): projected radius from the view angle on the sphere, flat bands white (2.0, 1.95, 1.7), yellow, orange, red-orange with jagged edges from triangle waves of the azimuth; outer edge jagged only between 0.956 and 1.0 of the mesh radius so the visible part stays above the collision radius; during the fade (progress = 1 - Opacity) each band cools one step to dark red and a jagged hole opens from the centre; alpha also x saturate(Opacity x 6), so it is gone at Opacity 0. Set as OverrideMaterials of the BP_ExplosionFX_Barrel Mesh (BP_ExplosionFX keeps MI_GB_Explosion). Material compiles, BP compiles and saves; Duration 0.25, FadeSeconds 0.25, FadeParameterName Opacity, BurstSystem and ShockwaveMaterial read back; radius x 1.05 and hold 0.1 s still come from C++ (unchanged); both CSV rows still play BP_ExplosionFX_Barrel. Commit fd697cb9. Look unverified in game (A8); formula preview only.

## Relations
- [[task-446-explosions-red]] — parent global task.
