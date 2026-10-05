---
id: sys-damage-numbers
description: "Damage number pipeline as built: world subsystem merging hits per target over 0.25 s, Niagara array user parameters, player toggle, tunables."
parent: systems-index
kind: doc
---
# Damage numbers

- responsibility: every floating damage number of a world, from hit to Niagara particles (task-025-01). No actor per hit.
- main_classes: `URRDamageNumberSubsystem` (`Damage/RRDamageNumberSubsystem.h/.cpp`, `UTickableWorldSubsystem`, Game and PIE worlds only), struct `FRRDamageNumber` (Location, Value, bCritical, Size, Color, SpawnTime); `URRGameUserSettings` (`Settings/RRGameUserSettings.h/.cpp`, `UGameUserSettings` subclass, `bShowDamageNumbers` saved in the user's GameUserSettings.ini; `Config/DefaultEngine.ini` `[/Script/Engine.Engine]` `GameUserSettingsClassName=/Script/RogueRigs.RRGameUserSettings`).
- merge: `AddHit(Target, Value, bCritical, Location)` adds to the target's open entry while world time − first hit < `MergeWindow`, else opens one with a random horizontal jitter. Merged value = sum, critical if any hit was. The entry keeps the last hit location, so a robot that dies or is destroyed keeps its number where it was hit. Value ≤ 0 ignored; a null target never merges. Safe inside a GE execution (no spawn, no broadcast).
- push: each tick, due entries (oldest first, at most `MaxNumbersPerTick`; the rest wait one tick) become `FRRDamageNumber`s (size `NumberSize`, × `CriticalScale` on a crit; `NormalColor` / `CriticalColor`), are written to the Niagara component and broadcast by the native `OnNumbersPushed`. Pauses with the world.
- Niagara contract (system asset built in [[task-025-02-bp-numbers]]): one persistent `UNiagaraComponent` spawned at world BeginPlay at the origin (not attached, no auto-destroy, no pre-cull) from `NumberSystem`; unset or missing package = nothing rendered, the merge still runs. User parameters, exact names: `User.DamageNumberPositions` (Niagara Position Array), `User.DamageNumberValues` (Float Array), `User.DamageNumberSizes` (Float Array, glyph world size), `User.DamageNumberColors` (Color Array), `User.DamageNumberCriticals` (Bool Array, drives the pop), `User.DamageNumberSpawnTimes` (Float Array, world time of the push), `User.DamageNumberNewCount` (int32: count of the batch this frame, set back to 0 on the next tick without numbers). Expected emitter: CPU sim, world space, fixed bounds covering the arena, Emitter Update `SpawnPerFrame` with Spawn Count = `User.DamageNumberNewCount`; Particle Spawn reads each array at `Engine.ExecutionIndex` (index within the frame's spawn batch, verified in `NiagaraEmitterInstanceImpl.cpp` / `NiagaraEmitterInstanceShader.usf`). A missing user parameter logs a `LogNiagara` warning per push.
- entry_points: `ARREnemy::HandleDamageReceived` (capsule top + `DamageNumberHeight` 40, [[sys-enemies-base]]), `ARRTargetDummy::HandleDamageReceived` (+ `DamageNumberOffset` (0, 0, 150), [[sys-target-dummy]]); `AreNumbersShown`, `GetPendingCount`, `GetMergeWindow`, `GetNiagaraComponent`; `URRGameUserSettings::GetRRGameUserSettings`, `SetShowDamageNumbers` (effect at once; `SaveSettings` keeps it), `GetShowDamageNumbers`.
- data: config properties, `DefaultGame.ini` section `[/Script/RogueRigs.RRDamageNumberSubsystem]`: `NumberSystem` (default `/Game/RogueRigs/FX/NS_DamageNumbers.NS_DamageNumbers`), `MergeWindow` 0.25, `NumberSize` 60, `CriticalScale` 1.6, `NormalColor` white, `CriticalColor` (1, 0.85, 0.45), `Jitter` 25, `MaxNumbersPerTick` 64.
- depends_on: [[sys-damage]] (`OnDamageReceived`); `Niagara` private module dependency ([[build-toolchain]]).
- implements: [[damage-numbers]].
- gotchas: toggle off drops open entries on the next tick and ignores new hits. Tests `RogueRigs.DamageNumbers.*` ([[sys-automation-tests]]). Per-number cost: one linear scan of open entries per hit (newest first). Glyph rendering, rise, fade and pop timing live in the Niagara asset.

## Relations
- [[systems-index]] — parent index.
- [[damage-numbers]] — the design this implements.
- [[sys-damage]] — the per-hit event feeding it.
- [[task-025-02-bp-numbers]] — the Niagara system built on this contract.
