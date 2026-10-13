---
id: task-112-01-ground-telegraph
description: "Ground telegraph component: a zone circle and a growing circle on the floor, ignoring robots, miner and weapons."
parent: task-112-ground-telegraph
kind: task
status: done
---
# Ground telegraph component (C++)

- goal: A generic ground signal component draws a zone circle and a growing circle on the floor, dies with its owner, and ignores robots, the miner and player weapons.
- agent: cpp
- depends_on: task-111-02-pool-prewarm
- inputs: ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), sys-enemies-heavy (vault/reference/tech/systems/sys-enemies/sys-enemies-heavy.md), bp-explosion-fx (vault/reference/tech/blueprints/bp-explosion-fx.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: build the shared object the owner asked for (read the GDD node ground-telegraph): `URRGroundTelegraphComponent` (a component, not an actor, so it is destroyed with its owner: no cleanup code), deriving from UDecalComponent (DBuffer-style decal projected downward so it follows the floor and the top of rocks) or the nearest equivalent you justify. API: SetRadius(uu), SetProgress(0..1), SetMode (enum: Danger = orange, ZoneBlocked = dotted gold, ZoneActive = dotted white-blue), Show / Hide, plus the default material soft path read from config (`DefaultMaterialPath`, default /Game/RogueRigs/FX/M_GroundTelegraph: the material asset is created by task-112-02; until then the component works without a visible result and logs one warning). It drives a dynamic material instance with scalar/vector parameters Radius, Progress, ZoneColor, ProgressColor, Opacity (names written in the doc for the material). Behaviour: no collision at all, no overlap events, not navigation relevant; ignores robots, the miner and player projectiles (they are not affected: decals do not collide; set bReceivesDecals = false on the meshes of the miner and of every robot so they are not tinted: do it once in their C++ base classes, say which); hides at once when its owner dies or is pending death (ARREnemy::IsDeathPending / IsDead), when the owner is released to the pool (IRRPoolable hooks: pooled robots are not destroyed) and when the owner is destroyed. Tests (RogueRigs.* style): parameters set on the material instance, hide on owner death, hide on pool release, no collision profile, destroyed with the owner. Docs: a new system node sys-ground-telegraph (parameters, how an owner uses it, the material parameter names), linked from systems-index (or its parent as the repo does), sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 74, cache write 92022, cache read 2199012, output 39402
- runs: 1
- milestone: M2
- result: URRGroundTelegraphComponent (UDecalComponent): SetRadius/SetProgress/SetMode/Show/Hide, DefaultMaterialPath config, material params Radius, Progress, ZoneColor, ProgressColor, Opacity; hides on owner death/pool release/miner death, bReceivesDecals off on miner, robots, weapons; 5 tests, 253/253. Commits 50f77d6, 829401f.

## Relations
- [[task-112-ground-telegraph]] — parent global task.
