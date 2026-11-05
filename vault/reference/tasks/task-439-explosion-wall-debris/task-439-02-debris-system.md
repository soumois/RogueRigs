---
id: task-439-02-debris-system
description: "Bp: The wall debris Niagara system exists, flies outward from the centre it is given, and is the one the game plays."
parent: task-439-explosion-wall-debris
kind: task
status: done
---
# Wall debris thrown away from explosions: bp

- goal: The wall debris Niagara system exists, flies outward from the centre it is given, and is the one the game plays.
- agent: bp
- depends_on: task-439-01-debris-request
- inputs: bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md)
- expected_output: see done_when
- done_when: System compiles with no error, one-shot, direction read from the given centre, at most 60 particles and 1.5 s read back.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-439
- usage: included in the executor run of task-439
- runs: 1
- milestone: M2
- started: 2026-11-05T19:30:19Z
- waiting_for: none
- result: NS_ExplosionWallDebris (from NS_DigDebris): compile UpToDate, 0 errors, 0 warnings; one-shot, world space, CPU; burst min(User.DebrisCount, 60); start from User.DebrisPositions, colour from User.DebrisColors, direction from User.ExplosionCenter with 8.5 deg spread; life 0.9 to 1.4 s. RogueRigs.Explosions.WallDebrisAsset under -RenderOffScreen: 60 particles in simulation, smallest cos to outward 0.990, back in the pool after 84 frames (1.4 s). Look in game not seen (no capture, owner validates).

## Relations
- [[task-439-explosion-wall-debris]] — parent global task.
