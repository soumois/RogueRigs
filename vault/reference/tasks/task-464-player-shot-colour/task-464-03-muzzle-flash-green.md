---
id: task-464-03-muzzle-flash-green
description: "Bp: Burst rifle muzzle flash green with several tones in the style of the owner reference; shotgun flash stays white-gold"
parent: task-464-player-shot-colour
kind: task
status: done
---
# Player shot colour: bp

- goal: Burst rifle muzzle flash green with several tones in the style of the owner reference; shotgun flash stays white-gold
- agent: bp
- depends_on: task-464-02-turret-laser-green
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), shotgun (vault/reference/gdd/weapons/shotgun.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: A green flash effect the turret reuses in task-464-04.
- done_when: In a -game run each rifle shot shows a green muzzle flash with a pale hot core, bright green burst, forward streak, shards and darker green glow, growing then fading into shards, about as brief as today; the shotgun volley flash is white-gold as today.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-464
- usage: included in the executor run of task-464
- runs: 1
- milestone: M2
- started: 2026-11-06T09:07:41Z
- waiting_for: none
- result: Done. NS_MuzzleFlash (rifle) Core and Sparks renderers use new M_MuzzleFlashGreen, a copy of M_MuzzleFlash whose Custom code colours are Green (0.45, 3.2, 0.12) rim, Pale (2.8, 4.4, 1.9) fill and core, plus a dark green (0.02, 0.35, 0.01) glow of alpha 0.7 within 0.26 (shapes, core) or 0.08 (streak, shards) of the shape edge, fading from progress 0.35 to 0.85; shapes, sizes, timing and sprite counts unchanged (task-440). NS_MuzzleFlashShotgun still renders M_MuzzleFlash (read back). Seen in slow-motion -game frames (slomo 0.15, Saved/Captures/task464_03_check/z.png, task464_03_shotgun/z.png): green flashes with pale centre lines and core, bright green blades, the seven-spike star, the crossed blades and the wings, a soft dark green halo, thinning out. Not seen: the shotgun white-gold flash in a frame (no volley caught in 4 runs; unchanged asset), left to the capture subtask C4. The reference was used as a style example; the shapes are the task-440 ones.

## Relations
- [[task-464-player-shot-colour]] — parent global task.
