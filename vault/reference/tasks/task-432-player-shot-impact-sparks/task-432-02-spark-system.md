---
id: task-432-02-spark-system
description: "Bp: The white-gold impact spark Niagara system exists and is the one the game spawns."
parent: task-432-player-shot-impact-sparks
kind: task
status: done
---
# Player shot impact sparks: bp

- goal: The white-gold impact spark Niagara system exists and is the one the game spawns.
- agent: bp
- depends_on: task-432-01-spawn-on-hit
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), bp-muzzle-flash (vault/reference/tech/blueprints/fx/bp-muzzle-flash.md)
- expected_output: see done_when
- done_when: One-shot CPU sprite system, at most 8 sprites and 0.25 s, white-gold, no compile or stack error, values read back, assigned so the game path of the previous subtask plays it.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-432
- usage: included in the executor run of task-432
- runs: 1
- milestone: M2
- started: 2026-11-05T15:56:21Z
- waiting_for: none
- result: NS_ShotImpactSpark (commit 4d6554ab), from NS_MuzzleFlash Sparks: CPU, one burst of 6 sprites, lifetime 0.1 to 0.2 s, HDR white-gold (6, 4.6, 1.6), size 1.2-1.8 x 6-9, speed 150-400, cone +Z 80 deg, drag 2. Compile UpToDate, 0 errors 0 warnings, values read back. Plays from ARRProjectile constructor reference: RogueRigs.Weapons.ImpactSparkAsset passes -RenderOffScreen (played, back in the pool after 12 frames of 1/60 s = 0.2 s) and -nullrhi; Weapons + Barrel 20 tests pass. Look unverified: owner judges in game.

## Relations
- [[task-432-player-shot-impact-sparks]] — parent global task.
