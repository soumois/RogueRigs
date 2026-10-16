---
id: task-197-02-explosion-fx-burst-shockwave
description: "C++: ARRExplosionFX gains an optional Niagara burst and an optional ground shockwave decal, both off when unset, so only a barrel FX class shows them."
parent: task-197-barrel-explosion-feedback
kind: task
status: in-progress
---
# Explosion FX burst and shockwave hooks

- goal: `ARRExplosionFX` gains (1) an optional Niagara system property spawned once at the start of the visual at the explosion centre, with a float user parameter carrying the visual radius (name documented); (2) an optional ground shockwave: a decal component (projected on the floor and rock tops, like the ground telegraph) whose material gets the visual radius, a progress 0..1 that grows over the sphere growth and an opacity that fades with the sphere fade (parameter names documented); both do nothing when unset, so `BP_ExplosionFX` and the kamikaze stay unchanged; the actor lifetime covers the burst; docs `sys-explosions` and `bp-explosion-fx` (C++ side) updated.
- agent: cpp
- depends_on: task-197-01-barrel-fuse-instability
- inputs: explosions (vault/reference/gdd/terrain/explosions.md), barrel (vault/reference/gdd/terrain/barrel.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in `Source/RogueRigs/Explosions/`, docs; one commit.
- done_when: build exit 0; existing tests pass (only broken expectations updated, no new test); with both properties unset an explosion behaves as before; property and parameter names listed in `sys-explosions`; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
