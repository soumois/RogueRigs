---
id: task-470-01-floor-material
description: "Bp: The floor material shows image 1 as its painted colour at the flagstone size of image 2, no added relief."
parent: task-470-cartoon-floor-texture
kind: task
status: done
---
# Floor uses the owner cartoon flagstone texture: bp

- goal: The floor material shows image 1 as its painted colour at the flagstone size of image 2, no added relief.
- agent: bp
- depends_on: none
- inputs: bp-floor-plates-material (vault/reference/tech/blueprints/world/bp-floor-plates-material.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Art/Source/floor-texture/owner-2026-11-05-floor-cartoon.webp, Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp
- expected_output: see done_when
- done_when: A1 to A5 and A7 hold in the editor and in a -game frame; the material change and the imported texture are committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-470
- usage: included in the executor run of task-470
- runs: 1
- milestone: M2
- started: 2026-11-06T10:33:53Z
- waiting_for: none
- result: Done (f3f50669). T_Floor_Cartoon_D imported from Art/Source/floor-texture/T_Floor_Cartoon_D.png (lossless PNG of image 1, 1254 px; PowerOfTwoMode StretchToPowerOfTwo so it gets mips, TC_Default, sRGB, wrap). New M_FloorCartoon (copy of M_FloorPlates): albedo = one world-planar sample (WP.xy / TileSize), the rotated 37 deg second sample removed (it blurred the strokes), slow value/hue modulation kept (NoiseAmount 0.08, HueAmount 0.03); normal output disconnected and normal nodes deleted (no relief). Defaults TileSize 1700 uu (stone about 3.5 to 4 miner widths in a -game frame), AlbedoBrightness 0.6, Roughness 0.8. MI_FloorCartoon parented to it; BP_ArenaGrid FloorMaterial = MI_FloorCartoon. M_FloorPlates, MI_FloorPlates, T_Floor_Plates_D/N untouched and unreferenced. Frames Saved/Captures/task470-floor-r1 (TileSize 1000, too small) and r2 (1700, brightness 0.5); no material or shader error in the run logs.

## Relations
- [[task-470-cartoon-floor-texture]] — parent global task.
