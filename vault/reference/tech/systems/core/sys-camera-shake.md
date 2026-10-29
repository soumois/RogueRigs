---
id: sys-camera-shake
description: "Shared camera shake: C++ Perlin translation shake, chest preset values, PlayCameraShake call any actor uses on the local player camera."
parent: systems-core-index
kind: doc
---
# Camera shake

- responsibility: one light camera shake any actor can play on the local player's top-down camera through one static call. Caller: the bonus chest at the end of its lid hold (`ARRBonusChest`, task-309-01, [[sys-objectives-console-chest]]). Heavy slam and explosions do not shake the camera (owner default Q21, 2026-10-23).
- main_classes (all in `Source/RogueRigs/Camera/`):
  - `URRCameraShake` (`RRCameraShake.h`, `UCameraShakeBase`, Blueprintable): holds the tunables, creates a `URRCameraShakePattern` as root pattern in its constructor (no Blueprint asset), `bSingleInstance` true (a new call restarts the shake, no stacking).
  - `URRCameraShakePattern` (`RRCameraShakePattern.h`, `UCameraShakePattern`): translation-only Perlin noise (`FMath::PerlinNoise1D` per axis, random start offset per play), timing through `FCameraShakeState`, blend weight applied to the offset; no rotation, no FOV. Written in the project because the engine Perlin pattern lives in the `EngineCameras` plugin (not a module dependency).
  - `URRCameraShakeLibrary` (`RRCameraShakeLibrary.h`, `UBlueprintFunctionLibrary`).
- entry_points: `static UCameraShakeBase* URRCameraShakeLibrary::PlayCameraShake(const UObject* WorldContextObject, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr)` (BlueprintCallable, WorldContext). Starts `ShakeClass` (null = `URRCameraShake`) on the first player controller's `PlayerCameraManager` with `Scale` as amplitude multiplier, camera-local play space; returns null when no world, no local controller or camera manager, or `Scale` <= 0. No distance falloff, no time dilation.
- data: `UPROPERTY(EditDefaultsOnly)` on `URRCameraShake`, category `RogueRigs|CameraShake`, all provisional owner 2026-10-23 (chest preset): `Duration` 0.2 s, `BlendOutTime` 0.08 s (capped at `Duration`), `TranslationAmplitude` (0, 4, 4) uu camera-local (X along the view = no zoom, Y right, Z up), `Frequency` 25 cycles/s. Another preset = a subclass with other defaults, passed as `ShakeClass`.
- depends_on: engine `Camera/CameraShakeBase.h`, `APlayerCameraManager`; the camera of [[sys-player]].
- implements: none (juice, owner request 2026-10-23, [[task-310-camera-shake]]).
- gotchas:
  - Perlin noise near integer inputs returns about 0; the random per-axis offsets avoid a dead start, but the peak reached in 0.2 s at 25 Hz can stay under the full 4 uu amplitude.
  - Split-screen or a non-local first controller plays nothing (single local player game).
  - Not seen in game yet: the shake is checked in the task-309 capture.

## Relations
- [[systems-core-index]] — parent index.
- [[sys-player]] — the miner's top-down camera the shake moves.
- [[task-310-camera-shake]] — the task that created it.
- [[sys-objectives-console-chest]] — the chest lid slam that plays it.
