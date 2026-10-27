---
id: sys-damage-numbers-niagara
description: "Per-digit Niagara contract of the damage numbers (task-025-04): exact user parameters, batch id spawn rule, particle index, emitter recipe for NS_DamageNumbers."
parent: sys-damage-numbers
kind: doc
---
# Damage numbers: per-digit Niagara contract

- responsibility: what `URRDamageNumberSubsystem` writes on its Niagara component and how the emitter must read it, so the asset does no layout or digit math.
- main_classes: `URRDamageNumberSubsystem::PushToNiagara` (names as `static const FName` members, `Damage/RRDamageNumberSubsystem.h`).
- entry_points: user parameters, exact names, one entry per digit, all arrays the same length, digit order = numbers in push order, each number's digits contiguous, most significant first:
  - `User.DigitPositions` Niagara Position Array: digit centre, world space (layout done in C++).
  - `User.DigitIndices` Int32 Array: glyph 0–10 = atlas cell (SubImage index): digits 0–9, 10 = "+" (`URRDamageNumberSubsystem::PlusGlyph`, cell 10 of T_DamageDigitsOutline since task-324-01), written first for a heal number (task-324-02).
  - `User.DigitSizes` Float Array: glyph world size.
  - `User.DigitColors` Color Array.
  - `User.DigitCriticals` Int32 Array: 1 critical (pop), 0 normal.
  - `User.DigitSpawnTimes` Float Array: world time of the push.
  - `User.DigitVelocities` Niagara Vector Array: world rise velocity (uu/s) of the digit, drawn once per number in C++ (all digits of a number hold the same value): `RiseVelocity` turned by ±`RiseAngleMax` 35 degrees in the screen plane around screen up, × [`RiseSpeedMin` 0.7, `RiseSpeedMax` 1.3] (task-240-01). NS_DamageNumbers reads it in Particle Spawn (`SelectVectorFromArray`, Direct Set, index `Particles.DNIdx`) as `Particles.Velocity` (task-240-02), then multiplies it by 4 and applies a Gravity Force (0,0,-980) so the number is thrown in an arc: C++ still sends the nominal rise velocity (about 85.7 uu/s), the asset scales it (task-242-01).
  - `User.DigitCount` int32: array length of the last batch; kept until the next batch.
  - `User.DigitBatchId` int32: +1 per push, never reset; 0 = nothing pushed. Written last.
- data: emitter recipe (CPU sim, world space, fixed bounds):
  1. Emitter State: loop Infinite, system never completes.
  2. Emitter Spawn: `Emitter.LastBatchId` (int) = 0.
  3. Emitter Update, one module: spawn `User.DigitCount` particles when `User.DigitBatchId` != `Emitter.LastBatchId`, else 0; then `Emitter.LastBatchId` = `User.DigitBatchId`. The whole batch in one spawn module output, this frame only.
  4. Particle Spawn: `Index` = `Engine.ExecIndex` (display name "Exec Index"). `Engine.ExecutionIndex` is not an engine variable (`NiagaraModule.cpp` defines `Engine.ExecIndex`; `NiagaraHlslTranslator.cpp` writes `Engine.ExecIndex = ExecIndex()` per stage): reading it gives the default 0, the likely cause of [[perf-stress-damage-numbers]] defect 2. Cross-check: `Particles.UniqueID − Engine.Emitter.TotalSpawnedParticles` is the same value (translator sets UniqueID = TotalSpawnedParticles + ExecIndex(); `NiagaraEmitterInstanceImpl.cpp` sets TotalSpawnedParticles before each spawn call). Read every array at `Index`; no division, no modulo.
  5. Check: `fx.DumpParticleData 1` shows indices 0…DigitCount−1 and spawned per batch = `User.DigitCount`.
- depends_on: [[sys-damage-numbers]].
- implements: [[damage-numbers]].
- gotchas: Niagara Data Channel route rejected: it needs a channel asset, and its `GetNDCSpawnData` still takes the exec index, so it removes no index read; arrays are also checkable headless. No C++-pushed UniqueID base: one culled or skipped batch would offset every later index. The C++ reactivation of a completed system resets `Emitter.LastBatchId`, so the batch pushed with it spawns; any other system reset replays the last batch once. Parameters the asset does not expose are skipped with one warning per world.

## Relations
- [[sys-damage-numbers]] — parent: the subsystem writing these parameters.
- [[bp-damage-numbers]] — the asset that must follow this contract.
- [[perf-stress-damage-numbers]] — the defects this contract answers.
