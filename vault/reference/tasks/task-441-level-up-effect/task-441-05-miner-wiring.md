---
id: task-441-05-miner-wiring
description: "Bp: BP_Miner plays the wave on level-up and no longer has the cyan disc; the heal ring stays."
parent: task-441-level-up-effect
kind: task
status: done
---
# Level-up effect rework: bp

- goal: BP_Miner plays the wave on level-up and no longer has the cyan disc; the heal ring stays.
- agent: bp
- depends_on: task-441-04-floor-wave
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-heal-ring (vault/reference/tech/blueprints/fx/bp-heal-ring.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md)
- expected_output: see done_when
- done_when: Compiles and saves; no disc component or disc timer left; level-up in PIE logs no runtime error; heal ring still spawned; the owner's own light values in BP_Miner are untouched and any unsaved editor change of his is not lost.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-441
- usage: included in the executor run of task-441
- runs: 1
- milestone: M2
- started: 2026-11-05T20:25:29Z
- waiting_for: none
- result: BP_Miner LevelUp.LevelUpWaveSystem = NS_LevelUpWave; component LevelUpDisc, variable DiscStep, function DiscStep_Tick and their nodes removed; OnLevelUpEffect still spawns BP_HealRing. Compiled with warnings as errors, saved. PIE GrantXP 5 and ChooseUpgrade 0: no runtime error, no Niagara warning. FloorWallLight read back unchanged (location 0, intensity 4, attenuation 800). Test Integration.LevelUpDiscEndsHidden removed, LevelUp.WaveAsset added (25 sprites, 29 frames, ring 599 uu, shards 322 uu). RogueRigs.LevelUp 8 of 8 under -RenderOffScreen; Integration, Progression, DamageNumbers all pass.

## Relations
- [[task-441-level-up-effect]] — parent global task.
