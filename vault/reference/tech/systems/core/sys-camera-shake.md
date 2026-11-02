---
id: sys-camera-shake
description: "Shared camera shake: Perlin translation shake, additive mixing with 12 uu cap, PlayCameraShake and located PlayCameraShakeAtLocation with distance falloff."
parent: systems-core-index
kind: doc
---
# Camera shake

- responsibility: one light camera shake any actor can play on the local player's top-down camera through one static call. Caller: the bonus chest at the end of its lid hold (`ARRBonusChest`, task-309-01, [[sys-objectives-console-chest]]). Located callers (damage, explosions, heavy slam) are tasks [[task-417-camera-shake-on-damage]], [[task-418-camera-shake-on-explosions]], [[task-419-camera-shake-on-heavy-slam]]. Shakes of different classes add up; the sum is capped (task-416).
- main_classes (all in `Source/RogueRigs/Camera/`):
  - `URRCameraShake` (`RRCameraShake.h`, `UCameraShakeBase`, Blueprintable): holds the tunables, creates a `URRCameraShakePattern` as root pattern in its constructor (no Blueprint asset), `bSingleInstance` true (see the modifier for same-class restarts).
  - `URRCameraShakePattern` (`RRCameraShakePattern.h`, `UCameraShakePattern`): translation-only Perlin noise (`FMath::PerlinNoise1D` per axis, random start offset per play), timing through `FCameraShakeState`, blend weight applied to the offset; no rotation, no FOV. Written in the project because the engine Perlin pattern lives in the `EngineCameras` plugin (not a module dependency).
  - `URRCameraShakeModifier` (`RRCameraShakeModifier.h`, `UCameraModifier_CameraShake`): added on first use to the first local camera manager. Different shake classes add up. `AddCameraShake` override: same class with `bSingleInstance` restarts at max(new, old `ShakeScale`). `ModifyCamera` override clamps the summed offset length (3D) to `MaxSummedTranslation` every frame.
  - `URRCameraShakeLibrary` (`RRCameraShakeLibrary.h`, `UBlueprintFunctionLibrary`).
- entry_points: `static UCameraShakeBase* URRCameraShakeLibrary::PlayCameraShake(const UObject* WorldContextObject, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr)` (BlueprintCallable, WorldContext). Starts `ShakeClass` (null = `URRCameraShake`) on the first player controller's `PlayerCameraManager` with `Scale` as amplitude multiplier, camera-local play space; returns null when no world, no local controller or camera manager, or `Scale` <= 0. Goes through the modifier. No distance falloff, no time dilation.
  - `static UCameraShakeBase* PlayCameraShakeAtLocation(const UObject* WorldContextObject, FVector Epicenter, float MaxDistance, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr)` (BlueprintCallable). Distance = 2D distance (`Dist2D`) from `Epicenter` to the camera view target (fallback: controller pawn); plays with `Scale * GetDistanceFalloffScale`. Returns null and plays nothing when distance > `MaxDistance`, `MaxDistance` <= 0, `Scale` <= 0 or no camera.
  - `static float GetDistanceFalloffScale(float Distance2D, float MaxDistance)` (BlueprintPure): `Lerp(1, EdgeFalloffScale, Distance2D / MaxDistance)`, 0 beyond `MaxDistance`.
- data: `UPROPERTY(EditDefaultsOnly)` on `URRCameraShake`, category `RogueRigs|CameraShake`, all provisional owner 2026-10-23 (chest preset): `Duration` 0.2 s, `BlendOutTime` 0.08 s (capped at `Duration`), `TranslationAmplitude` (0, 4, 4) uu camera-local (X along the view = no zoom, Y right, Z up), `Frequency` 25 cycles/s. Another preset = a subclass with other defaults, passed as `ShakeClass`.
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
- [[task-417-camera-shake-on-damage]] — damage caller of the located call.
- [[task-418-camera-shake-on-explosions]] — explosion caller of the located call.
- [[task-419-camera-shake-on-heavy-slam]] — heavy slam caller of the located call.
- [[sys-objectives-console-chest]] — the chest lid slam that plays it.
