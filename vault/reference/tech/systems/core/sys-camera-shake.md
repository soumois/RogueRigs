---
id: sys-camera-shake
description: "Shared camera shake: Perlin translation shake, additive mixing with 12 uu cap, PlayCameraShake and located PlayCameraShakeAtLocation, chest, hit and explosion presets."
parent: systems-core-index
kind: doc
---
# Camera shake

- responsibility: one light camera shake any actor can play on the local player's top-down camera through one static call. Caller: the bonus chest at the end of its lid hold (`ARRBonusChest`, task-309-01, [[sys-objectives-console-chest]]). The miner plays the hit preset `URRHitCameraShake` on each hit that takes damage (task-417, plain call). Each resolved explosion plays the explosion preset `URRExplosionCameraShake` through the located call (task-418, [[sys-explosions]]); this reverses the earlier "explosions do not shake" line. The heavy slam is the remaining located caller ([[task-419-camera-shake-on-heavy-slam]]). Shakes of different classes add up; the sum is capped (task-416).
- main_classes (all in `Source/RogueRigs/Camera/`):
  - `URRCameraShake` (`RRCameraShake.h`, `UCameraShakeBase`, Blueprintable): holds the tunables, creates a `URRCameraShakePattern` as root pattern in its constructor (no Blueprint asset), `bSingleInstance` true (see the modifier for same-class restarts).
  - `URRCameraShakePattern` (`RRCameraShakePattern.h`, `UCameraShakePattern`): translation-only Perlin noise (`FMath::PerlinNoise1D` per axis, random start offset per play), timing through `FCameraShakeState`, blend weight applied to the offset; no rotation, no FOV. Written in the project because the engine Perlin pattern lives in the `EngineCameras` plugin (not a module dependency).
  - `URRCameraShakeModifier` (`RRCameraShakeModifier.h`, `UCameraModifier_CameraShake`): added on first use to the first local camera manager. Different shake classes add up. `AddCameraShake` override: same class with `bSingleInstance` restarts at max(new, old `ShakeScale`). `ModifyCamera` override clamps the summed offset length (3D) to `MaxSummedTranslation` every frame.
  - `URRHitCameraShake` (`RRHitCameraShake.h`, `URRCameraShake`, Blueprintable, task-417-01): hit preset, translation only. Own class so the modifier adds it to a chest, explosion or slam shake already playing; `bSingleInstance` true, so a new hit restarts at max scale.
  - `URRExplosionCameraShake` (`RRExplosionCameraShake.h`, `URRCameraShake`, Blueprintable, task-418-01): explosion preset, translation only. Own class so it adds to a hit shake already playing; `bSingleInstance` true, so a chain link restarts it at max scale.
  - `URRCameraShakeLibrary` (`RRCameraShakeLibrary.h`, `UBlueprintFunctionLibrary`).
- entry_points: `static UCameraShakeBase* URRCameraShakeLibrary::PlayCameraShake(const UObject* WorldContextObject, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr)` (BlueprintCallable, WorldContext). Starts `ShakeClass` (null = `URRCameraShake`) on the first player controller's `PlayerCameraManager` with `Scale` as amplitude multiplier, camera-local play space; returns null when no world, no local controller or camera manager, or `Scale` <= 0. Goes through the modifier. No distance falloff, no time dilation.
  - `static UCameraShakeBase* PlayCameraShakeAtLocation(const UObject* WorldContextObject, FVector Epicenter, float MaxDistance, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr)` (BlueprintCallable). Distance = 2D distance (`Dist2D`) from `Epicenter` to the camera view target (fallback: controller pawn); plays with `Scale * GetDistanceFalloffScale`. Returns null and plays nothing when distance > `MaxDistance`, `MaxDistance` <= 0, `Scale` <= 0 or no camera.
  - `static float GetDistanceFalloffScale(float Distance2D, float MaxDistance)` (BlueprintPure): `Lerp(1, EdgeFalloffScale, Distance2D / MaxDistance)`, 0 beyond `MaxDistance`.
- data: `UPROPERTY(EditDefaultsOnly)` on `URRCameraShake`, category `RogueRigs|CameraShake`, all provisional owner 2026-10-23 (chest preset): `Duration` 0.2 s, `BlendOutTime` 0.08 s (capped at `Duration`), `TranslationAmplitude` (0, 4, 4) uu camera-local (X along the view = no zoom, Y right, Z up), `Frequency` 25 cycles/s. Another preset = a subclass with other defaults, passed as `ShakeClass`.
  - Hit preset `URRHitCameraShake`, provisional owner 2026-11-02: `Duration` 0.2 s, `TranslationAmplitude` (0, 5, 5) uu, `Frequency` 25 Hz, `BlendOutTime` 0.08 s (base value). Trigger: `ARRMiner::HandleDamageReceived` plays it with `PlayCameraShake(this, 1.f, HitShakeClass)` when `Damage` > 0 and `HitShakeClass` is set, see [[sys-player-classes]]. Same strength whatever the damage; lethal hit included; a hit refused by the i-frames plays nothing.
  - Explosion preset `URRExplosionCameraShake`, provisional owner 2026-11-02: `Duration` 0.35 s, `TranslationAmplitude` (0, 10, 10) uu (= barrel strength at scale 1), `Frequency` 25 Hz, `BlendOutTime` 0.08 s. Trigger: `URRExplosionSubsystem::Explode` at each resolved explosion centre (not on fuse start), `PlayCameraShakeAtLocation(Center, Radius × ShakeMaxDistanceFactor, ShakeScale, ExplosionShakeClass)`. Strengths and max distances are tuned in `ExplosionStats.csv`: Barrel `ShakeScale` 1 (10 uu), factor 2 = 1400 uu; Kamikaze 0.7 (7 uu), factor 2 = 624 uu; falloff 1 at the centre, 0.5 at the max distance. Detail in [[sys-explosions]].
  - `UPROPERTY(EditDefaultsOnly)` on `URRCameraShakeModifier`, provisional owner 2026-11-02: `MaxSummedTranslation` 12 uu, `EdgeFalloffScale` 0.5 (scale at `MaxDistance`).
- depends_on: engine `Camera/CameraShakeBase.h`, `APlayerCameraManager`; the camera of [[sys-player]].
- implements: none (juice, owner request 2026-10-23, [[task-310-camera-shake]]).
- gotchas:
  - Perlin noise near integer inputs returns about 0; the random per-axis offsets avoid a dead start, but the peak reached in 0.2 s at 25 Hz can stay under the full 4 uu amplitude.
  - Split-screen or a non-local first controller plays nothing (single local player game).
  - `StartCameraShake` called directly on the camera manager bypasses the modifier (no cap, not stoppable through it); no project code does that. A new camera manager gets the modifier on its first shake.
  - Not seen in game yet: the chest shake is checked in the task-309 capture; mixing, cap and falloff of task-416 are not verified in play (no capture).

## Relations
- [[systems-core-index]] — parent index.
- [[sys-player]] — the miner's top-down camera the shake moves.
- [[task-310-camera-shake]] — the task that created it.
- [[task-416-camera-shake-mixing-falloff]] — added mixing, cap and the located call.
- [[task-417-camera-shake-on-damage]] — added the hit preset played on miner damage.
- [[sys-player-classes]] — `ARRMiner::HandleDamageReceived` and `HitShakeClass` that play the hit preset.
- [[task-418-camera-shake-on-explosions]] — added the explosion preset and its caller.
- [[sys-explosions]] — `URRExplosionSubsystem::PlayShake`, per-source CSV values.
- [[task-419-camera-shake-on-heavy-slam]] — heavy slam caller of the located call.
- [[sys-objectives-console-chest]] — the chest lid slam that plays it.
