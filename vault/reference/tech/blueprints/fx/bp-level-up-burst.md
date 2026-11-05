---
id: bp-level-up-burst
description: "NS_LevelUpText and NS_LevelUpWave with their materials and texture: the gold level-up text and floor wave; open for emitters, sizes and colours."
parent: blueprints-fx-index
kind: doc
---
# NS_LevelUpText and NS_LevelUpWave

- asset_path: /Game/RogueRigs/FX/NS_LevelUpText, M_LevelUpText, T_LevelUpText; /Game/RogueRigs/FX/NS_LevelUpWave, M_LevelUpWave (task-441, commits 57a5a758, 6572a205).
- parent_class: UNiagaraSystem; no Blueprint graph.
- purpose: the look of a level-up ([[xp-level-up]]): text above the miner, wave on the floor. Played by C++ once per level-up moment, see [[sys-level-up]]; set on [[bp-miner]]. Values provisional (owner 2026-11-05), colour in [[color-code]].
- components: none. CPU sim, one-shot (Loop Once), no light, no shadow. Both were duplicated from NS_MuzzleFlash, so their emitters keep the names below.
- exposed_defaults: NS_LevelUpWave `User.WaveRadius` float, default 300, written by C++ with `PushRadius`. NS_LevelUpText has no user parameter.
- events: none.
- logic_in_bp: none.
- assets:
  - NS_LevelUpText, emitter `Core`, world space, 1 sprite, Unaligned, FaceCamera: 640 x 80 uu, life 0.7 s, velocity (0, 0, 100) with no force, so it rises straight. Pop `float2(640, 80) * (1 + 0.15 * (1 - saturate(Particles.Age / 0.25)))`, the amplitude and time of a normal number in [[bp-damage-numbers]]. Fade `saturate((1 - NormalizedAge) / 0.45)`, same as a number. Colour gold (1, 0.72, 0.15).
  - T_LevelUpText, 512 x 64, no mips, clamped: "LEVEL UP" in the 5x7 font of T_DamageDigitsOutline, white fill, 3 px black outline, word box x 107 to 404. Source `Art/Source/level-up-text/T_LevelUpText.png` (keep), drawn by a throwaway script. In game the word is about 372 uu wide.
  - M_LevelUpText: copy of M_DamageNumber with this texture on its ParticleSubUV node; the renderer keeps SubImage 1 x 1.
  - NS_LevelUpWave, emitter `Core`: 1 flat ring, life 0.45 s, size `2 * User.WaveRadius * (1 - (1 - NormalizedAge)^2)`. Emitter `Sparks`: 24 flat shards, 12 to 20 x 60 to 110 uu, life 0.4 to 0.5 s, even angles with jitter, speed `User.WaveRadius / 0.45` x 0.85 to 1. Both local space, `FacingMode` CustomFacingVector with `Particles.SpriteFacing` (0, 0, 1), so they lie on the floor; shards VelocityAligned. 25 sprites at most.
  - M_LevelUpWave: copy of M_MuzzleFlash with new Custom code. ParticleColor carries data: R kind (0 ring, 1 shard), G progress, A opacity. Gold (1, 0.72, 0.15) and hot (1, 0.93, 0.7), x 2 for the glow, are in the code, so no system tints it. The ring band thins from 30 % to 5 % of the radius and trails a faint glow; a shard is a diamond.
- owner_agent: agent-bp
- verified: compile UpToDate, 0 errors, 0 warnings on both systems. Under `-RenderOffScreen`, `RogueRigs.LevelUp.TextAsset`: 1 sprite, 41 frames, width 730 to 640 uu, rises Z only, gold. `RogueRigs.LevelUp.WaveAsset`: 25 sprites, 29 frames, ring up to 599 uu, shards reach 322 uu, height 0. PIE: a level-up logs no runtime error and no Niagara warning.
- unverified: the look in game (no capture, the owner judges it, task-441 A7): text size and legibility, the wave lying flat on the floor, its strength, and whether it reads as what pushes the robots. Shards can pass 300 uu by about 7 %.

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-level-up]] — the C++ that plays both and pushes the robots.
- [[bp-miner]] — sets both systems on its LevelUp component.
- [[bp-damage-numbers]] — the pop and font it copies; it does not use that system.
- [[bp-muzzle-flash]] — the system and material the wave was copied from.
- [[xp-level-up]] — the design.
- [[color-code]] — the gold.
- [[task-441-level-up-effect]] — the task that made them.
