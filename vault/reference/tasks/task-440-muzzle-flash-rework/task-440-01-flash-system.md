---
id: task-440-01-flash-system
description: "Bp: The muzzle flash system and its soft material have the new look, larger on the shotgun."
parent: task-440-muzzle-flash-rework
kind: task
status: done
---
# Muzzle flash rework: bp

- goal: The muzzle flash system and its soft material have the new look, larger on the shotgun.
- agent: bp
- depends_on: none
- inputs: bp-muzzle-flash (vault/reference/tech/blueprints/fx/bp-muzzle-flash.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), Art/Source/muzzle-flash/20261105-owner-ref-video-1328-1333-sheet.png, bp-shotgun (vault/reference/tech/blueprints/player/bp-shotgun.md)
- expected_output: see done_when
- done_when: System and material compile with no error, values read back within the bounds, the rifle and the shotgun both play it, no light, tests of A6 pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 8, cache write 36528, cache read 21986, output 2696
- usage: turns 4, context start 9k peak 21k, resumes 1 (message x1), 0 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-11-05T19:39:46Z
- waiting_for: none
- result: M_MuzzleFlash (procedural, 4 shapes, core, streak, white-gold fixed in the material), NS_MuzzleFlash 3 + 3 sprites at most 0.1 s, NS_MuzzleFlashShotgun 3 + 5 sprites at 1.5x on BP_Shotgun, no light. Compile 0 errors, values read back; RogueRigs.Weapons.MuzzleFlashAsset passes under -RenderOffScreen (6 and 8 sprites, 4 of 4 shapes over 16 shots, shape up to 199 and 295 uu, gone within 0.1 s); Weapons, Shotgun, Turret 29 tests pass. Critic: one fail on the flame fixed, second pass clean. Look in game unverified, owner validates.

## Relations
- [[task-440-muzzle-flash-rework]] — parent global task.
