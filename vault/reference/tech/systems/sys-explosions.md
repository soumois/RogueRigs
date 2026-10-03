---
id: sys-explosions
description: "Explosion queue as built: one link per tick, 0.08 s chain delay, one per source, blocks in radius destroyed, character damage, miner no-one-shot cap, FX."
parent: systems-index
kind: doc
---
# Explosions

- responsibility: the single path of every explosion and chain reaction: terrain, character damage, visual, event.
- main_classes (`Source/RogueRigs/Explosions/`): `URRExplosionSubsystem` (`UTickableWorldSubsystem`, `Config = Game`), `FRRExplosionStatsRow` (CSV row), `ARRExplosionFX` (abstract visual actor). Damage effect `URRGE_DamageExplosion` in `Source/RogueRigs/Damage/` ([[sys-damage]]).
- entry_points: `QueueExplosion(Center, RowName, Instigator, SourceObject)` (BlueprintCallable; false when the row is missing or the source is spent); `QueueExplosionWithStats(Center, RowName, Stats, Instigator, SourceObject)` (C++); `GetExplosionStats(RowName, OutStats)`; `IsSourceSpent(SourceObject)`; `GetPendingCount()`; `GetChainDelay()` / `SetChainDelay()`; `SetGrid()` (default: first `ARRArenaGrid`). Dynamic delegate `OnExplosion(Center, RowName)` (BlueprintAssignable), broadcast after each explosion is resolved.

## Rules (as built)
- Never synchronous: `QueueExplosion` only appends to a FIFO queue; `Tick` resolves at most one explosion per tick, when world time ≥ previous explosion time + `ChainDelay` (tolerance 1e-4 s). A queue that was idle resolves on the next subsystem tick. At 60 fps the 0.08 s delay gives a 5-frame gap (0.083 s, measured).
- One explosion per source: the source object is recorded when queued (`FObjectKey`, kept for the world's life); queuing it again, before or after it explodes, is refused. A null source is never deduplicated.
- Resolution order: grid `DestroyCellsInRadius(Center, Radius, TerrainSource)` (every block whose cell center is within the 2D radius, whatever its type or health); then characters; then FX; then `OnExplosion`.
- Characters: every actor whose ASC (`UAbilitySystemGlobals::GetAbilitySystemComponentFromActor`) owns a `URRHealthAttributeSet` and whose actor location is within the 2D radius gets one `URRGE_DamageExplosion` (deduplicated by ASC; targets collected before any damage). Context: instigator = `Instigator`, effect causer = the source when it is an actor (else the instigator), source object = the source.
- No-one-shot rule: if the target is an `ARRMiner` with `Health >= MaxHealth` (full health), the damage is capped at `Health − MinerHealthLeftAtFullHealth` (1): a full-health miner survives any single explosion with at least 1 Health. Below full health the damage is not capped and an explosion can kill.
- I-frames: respected. `URRGE_DamageExplosion` refuses targets with `State.Invulnerable` or `State.Dead`; the miner's i-frames (applied after a non-lethal explosion hit like any hit) make a following chain link within 0.25 s miss him.
- FX: the row's `FXClass` is loaded once (package checked first, a missing or abstract class is logged once at Log level and skipped), spawned deferred at the center with `SetRadius(Radius)`.
- `ARRExplosionFX`: root `Mesh` (no collision, no shadow); over `Duration` (0.25 s) its scale goes linearly from 0 to `Radius / MeshRadius` (`MeshRadius` 50 = engine sphere) and the scalar parameter `FadeParameterName` (`Opacity`) of a dynamic instance of material slot 0 from 1 to 0; then the actor destroys itself. The Blueprint child (`/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX`, task-011-06) sets the sphere mesh and a translucent material exposing `Opacity`.

- data: `Content/RogueRigs/Data/Tables/ExplosionStats.csv` (`FRRExplosionStatsRow`: `Radius`, `Damage`, `TerrainSource`, `FXClass`), loaded at runtime with `FRRBaseStatsLoader::LoadTableFromCSV` on first use: `Barrel` 350 / 40 / Barrel, `Kamikaze` 250 / 30 / Kamikaze, both FX `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX.BP_ExplosionFX_C`. Config properties (defaults in code, override in `DefaultGame.ini` section `[/Script/RogueRigs.RRExplosionSubsystem]`): `ChainDelay` 0.08 s (provisional), `MinerHealthLeftAtFullHealth` 1.
- depends_on: [[sys-grid]], [[sys-damage]].
- implements: [[explosions]], [[destruction-rules]] (explosion side); [[barrel]] and [[enemy-kamikaze]] queue into it (task-011-04, task-011-05).
- gotchas: synchronous chains in damage callbacks cause recursion and double hits; that is why the queue exists — never call the grid or apply explosion damage directly. Target search iterates every actor of the world once per explosion (fine at M1 horde size). The character radius test uses the actor location only (no capsule radius). Barrels have no ASC (task-011-04 owns their shot health).

## Relations
- [[systems-index]] — parent index.
- [[sys-damage]] — the damage chain explosions enter.
- [[sys-grid]] — the terrain explosions destroy.
- [[sys-automation-tests]] — `RogueRigs.Explosions.*` tests.
