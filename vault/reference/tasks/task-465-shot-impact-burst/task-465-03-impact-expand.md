---
id: task-465-03-impact-expand
description: "Bp: Turret impacts get the green burst sized to the turret shot; shotgun pellet impacts get the same burst in white with warm gold-orange accents"
parent: task-465-shot-impact-burst
kind: task
status: done
---
# Player shot impact burst: bp

- goal: Turret impacts get the green burst sized to the turret shot; shotgun pellet impacts get the same burst in white with warm gold-orange accents
- agent: bp
- depends_on: task-465-02-slice-capture
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), shotgun (vault/reference/gdd/weapons/shotgun.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png
- expected_output: see done_when
- done_when: In a -game run turret shot impacts show the green burst of task-465-01 sized to the turret shot; shotgun pellet impacts show the same burst in white with the warm gold-orange accents and glow of the reference; enemy impacts, mining laser and aim laser unchanged; no gameplay value changed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 6, cache write 20027, cache read 32826, output 3084
- usage: turns 3, context start 10k peak 24k, resumes 0, 1 min, units 0.04 M
- runs: 1
- milestone: M2
- started: 2026-11-06T09:42:43Z
- waiting_for: none
- result: Done (deae8529, warmth fix in the next commit). BP_TurretBullet ImpactSparkSystem = new NS_ShotImpactBurstGreenSmall (copy of the rifle burst at 0.7x: spikes 6.9-10.8 x 35-63 uu, core 29-35 uu, triangles 11-16 uu, triangle speed 108-186 uu/s, M_ImpactBurstGreen). BP_ShotgunPellet ImpactSparkSystem = new NS_ShotImpactBurstWhite (rifle burst sizes, new M_ImpactBurstWhite: Pale (4.5,4.2,3.0) white fill and core, Main (2.4,1.45,0.2) gold rims and gold spikes, Accent (2.6,0.9,0.08) orange rims and outlines, Dark (0.9,0.32,0.03) warm orange glow, black triangles kept). 8 sprites, at most 0.24 s each (A8). Seen in slow-motion -game frames: turret hits on chasers show a small green star (Saved/Captures/task465_03_check/turret.png); shotgun pellet hits on Heavies show white bursts with gold spikes (Saved/Captures/task465_03_shotgun2/crop.png). Style-guide: turret pass; shotgun fail (too pale, color-code warm accents) corrected once and rechecked. Enemy shots have no impact system (ARREnemyProjectile ImpactSparkSystem null, unchanged); mining and aim lasers untouched. No gameplay value changed. Old NS_ShotImpactSpark and NS_ShotImpactSparkGreen no longer referenced by any shot BP (NS_ShotImpactSpark stays the C++ constructor default and the asset test reference).

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
