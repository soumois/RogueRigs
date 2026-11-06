---
id: task-464-01-rifle-laser-green
description: "Bp: Burst rifle laser shot and its impact sparks in the varied green of the owner reference, as vivid as the red enemy laser"
parent: task-464-player-shot-colour
kind: task
status: done
---
# Player shot colour: bp

- goal: Burst rifle laser shot and its impact sparks in the varied green of the owner reference, as vivid as the red enemy laser (representative slice)
- agent: bp
- depends_on: none
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-432-player-shot-impact-sparks (vault/reference/tasks/task-432-player-shot-impact-sparks.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: In a -game run the burst rifle shot and its impact sparks are the green of the reference with several tones, not white-gold, not the Shooter acid green; shotgun pellets, mining and aim lasers still white-gold; enemy shots unchanged; no gameplay value changed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-464
- usage: included in the executor run of task-464
- runs: 1
- milestone: M2
- started: 2026-11-06T08:53:11Z
- waiting_for: none
- result: Done. BP_Projectile ShotMesh uses new M_PlayerLaserShot (unlit; CoreColor (0.75, 1, 0.5), BodyColor (0.12, 1, 0.02), EdgeColor (0.02, 0.28, 0.01) by fresnel exponent 1, core below 0.15, Glow 3) and ImpactSparkSystem NS_ShotImpactSparkGreen (copy of NS_ShotImpactSpark, colour random range (0.15, 2.4, 0.05) to (3.2, 6.5, 1.8)). Seen in -game frames (Saved/Captures/task464_01_check, task464_01_sparks): green shot with pale centre and darker green rim, about the size of the red robot shot, pure green not the Shooter yellow-lime; one tiny green spark seen on a heavy (sparks are 1-2 uu wide, as before). Shotgun pellet, mining and aim lasers untouched (white-gold aim laser overlaps the shots on screen). Also added rr.Debug.AddWeapon (debug command, build only) for the capture. No gameplay value changed.

## Relations
- [[task-464-player-shot-colour]] — parent global task.
