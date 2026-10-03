---
id: task-011-03-explosion-system
description: "URRExplosionSubsystem: queued explosions, 0.08 s chain delay, every block in radius destroyed, character damage without one-shot, FX actor."
parent: task-011-explosions
kind: task
status: done
---
# Explosion system

- goal: one safe path for every explosion.
- agent: cpp
- depends_on: task-011-01-feel-tweaks
- inputs: [[sys-explosions]], [[explosions]], [[sys-damage]], [[sys-grid]], [[destruction-rules]], [[naming]], [[gas-api-notes]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `Source/RogueRigs/Explosions/`: `FRRExplosionStatsRow` and `Content/RogueRigs/Data/Tables/ExplosionStats.csv` rows `Barrel` (Radius 350, Damage 40) and `Kamikaze` (Radius 250, Damage 30); `URRExplosionSubsystem` (world subsystem): `QueueExplosion(Center, RowName, Instigator, SourceObject)` never explodes synchronously; processes the queue on tick, one link at a time with `ChainDelay` 0.08 s (editable) between links; each source object explodes once. An explosion: `DestroyCellsInRadius` on the grid (source Barrel or Kamikaze); damages every actor with an ASC and a health set inside the 2D radius once, through a new `URRGE_DamageExplosion` (SetByCaller `Data.Damage`, refuses Dead and Invulnerable); no-one-shot rule on the miner: an explosion never takes the miner from full health to 0 (document the exact rule); spawns an editable FX actor class. `ARRExplosionFX` (abstract): a mesh scaled from 0 to the radius and fading over 0.25 s, then destroyed. Delegate `OnExplosion(Center, RowName)`. Rewrite `sys-explosions` to match (template `system.md`); [[sys-grid]], [[sys-damage]] updated. Tests `RogueRigs.Explosions.*`: blocks in the radius destroyed, outside kept; miner and a dummy damaged once; two queued explosions resolve 0.08 s apart; the same source queued twice explodes once; a full-health miner survives a 1000-damage explosion.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 100000
- milestone: M1
- result: URRExplosionSubsystem (FIFO queue, one link per tick after ChainDelay 0.08 s, each source once; QueueExplosion, OnExplosion); ExplosionStats.csv (Barrel 350/40, Kamikaze 250/30, TerrainSource, FXClass = /Game/RogueRigs/Blueprints/FX/BP_ExplosionFX); URRGE_DamageExplosion (Data.Damage, refuses Dead and Invulnerable; explosions respect i-frames); no one-shot: a full-health miner keeps at least 1 health; ARRExplosionFX (scale 0 → radius, Opacity 1 → 0 over 0.25 s). 5 Explosions tests. Build exit 0, 41/41 (orchestrator re-ran). Docs sys-explosions (rewritten), sys-grid, sys-damage, sys-automation-tests, build-toolchain. About 111k tokens (over ceiling).

## Relations
- [[task-011-explosions]] — parent global task.
