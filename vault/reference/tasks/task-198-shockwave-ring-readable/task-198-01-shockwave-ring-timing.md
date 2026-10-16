---
id: task-198-01-shockwave-ring-timing
description: "C++: the explosion shockwave ring grows over about 0.6 s on its own timeline, after the 0.25 s sphere growth, then fades shortly."
parent: task-198-shockwave-ring-readable
kind: task
status: done
---
# Shockwave ring timing

- goal: in `ARRExplosionFX`, the shockwave ring gets its own timeline instead of following the sphere: `Progress` 0 to 1 over a new `ShockwaveGrowSeconds` (about 0.6 s, provisional), then `Opacity` 1 to 0 over a new `ShockwaveFadeSeconds` (about 0.2 s, provisional); the ring stays visible after the sphere has faded; the actor lifetime covers it (with a burst it is 1.5 s already; without a burst, the lifetime becomes the longer of the sphere and the ring only when a shockwave is set, unchanged otherwise); docs `sys-explosions`, `bp-explosion-shockwave` (timing line) updated.
- agent: cpp
- depends_on: none
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), explosions (vault/reference/gdd/terrain/explosions.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: `ARRExplosionFX` ring timing (C++), docs; one commit.
- done_when: build exit 0; explosion tests pass (only expectations broken by this change updated, no new test); ring growth and fade seconds named in sys-explosions; ring still hidden at the end of the actor life; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 50, cache write 66251, cache read 1180420, output 20256
- runs: 1
- milestone: M1
- result: Shockwave ring own timeline: ShockwaveGrowSeconds 0.6, ShockwaveFadeSeconds 0.2, hidden at 0.8 s after the sphere (0.6 s); lifetime max of sphere, ring, burst; build ok, explosion tests pass; commits c532ff13, d09f80d0

## Relations
- [[task-198-shockwave-ring-readable]] — parent global task.
