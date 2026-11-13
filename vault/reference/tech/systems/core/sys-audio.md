---
id: sys-audio
description: "Sound effects as built: URRAudioSubsystem, event table in DefaultGame.ini, call sites, caps, button sounds, how to add a sound, rr.Audio commands."
parent: systems-core-index
kind: doc
---
# Audio (sound events)

- responsibility: plays the sound of a named event from one data table, with the mix rules of [[audio-direction]] (task-511, commits db961979, a6285d70, f10a5ab6). Adding or swapping a sound is one table line; a new event is one line plus one call.
- main_classes: `Audio/RRAudioSettings.h/.cpp`: `URRAudioSettings` (config object) and `FRRSoundEventRow`. `Audio/RRAudioSubsystem.h/.cpp`: `URRAudioSubsystem` (GameInstance subsystem) and a Slate input pre-processor. `Audio/RRAudioDebugCommands.cpp` (not shipping).
- entry_points: `URRAudioSubsystem::PlayEvent(Context, Event, Variant)` (centred) and `PlayEventAt(Context, Event, Location, Variant)` (placed). Lookup: `Event.Variant` row first, else its parent name (`Shot.Shotgun` falls back to `Shot`). No row or no sound: silent, logged once at Verbose.
- call_sites: `Shot` in `ARRWeapon::FireOnce` and `ARRShotgun::FireOnce` (one per volley) as `Shot.<StatsRowName>` (`ShotSoundEvent`); `Impact` in `ARRProjectile::PlayImpactSpark` (`ImpactSoundEvent`); `EnemyDeath` in `ARREnemy::HandleDeath` (not a kamikaze's own fuse); `PlayerHit` in `ARRMiner::HandleDamageReceived`; `XPPickup` in `ARRPickup::Collect`; `LevelUp` in `URRLevelUpComponent::HandleLevelUp`. Turret, turret bullet and robot bullet set None: silent (owner confirmation open).
- buttons: a real-time ticker plays `UIHover` when the enabled Slate `SButton` under the cursor (game window) changes; the pre-processor plays `UIClick` on a left press over one. Disabled = the button or a parent disabled. Every screen, paused ones included, no per-widget code; keyboard or gamepad activation plays nothing.
- data: `Config/DefaultGame.ini` `[/Script/RogueRigs.RRAudioSettings]`, one `+Events=(...)` line per event: `Event`, `Sound`, `Volume`, `PitchJitter` (0.05 = ±5 %), `MaxConcurrent` (0 = no cap; the oldest play stops), `bSpatial` (placed: spatialized, no distance fade), `bUI` (plays while paused). Values per [[audio-direction]]. Assets `/Game/RogueRigs/Audio/SFX/SW_*`; sources, originals and licences in `Art/Source/audio/` and `Art/Source/audio/sound_manifest.json`. Every play uses the Effects class of `URROptionsSettings` ([[sys-options]]), so the Effects and Master sliders set it.
- add_a_sound: put the prepared WAV in `Art/Source/audio/`, add its entry to `sound_manifest.json`, import with `Tools/unreal/import_sounds.py` (PythonScript commandlet, editor closed, usage in the script header), then add or edit the table line. New event: also one `PlayEvent`/`PlayEventAt` call at its moment.
- debug: `rr.Audio.Play <Event>`; `rr.Audio.Stats [reset]` (per row plays, cap, peak, cap stops; device volumes of Effects and Music); `rr.Debug.Click [down|up]` (a press and release in one frame do not activate a UMG button: send them apart); per-play lines with `log LogRRAudio Verbose`.
- depends_on: [[sys-options]] (sound classes and volume mix), [[sys-button-hover]] (same buttons, separate mechanism).
- implements: [[audio-direction]]
- gotchas: on a paused world, finished UI plays may still count as active (UI rows have no cap). No audio under `-nullrhi`.

## Relations
- [[systems-core-index]] — parent index.
- [[audio-direction]] — the event list, sources and mix rules it applies.
- [[sys-options]] — the volume sliders and sound classes it plays through.
- [[sys-button-hover]] — the hover look of the same buttons.
- [[task-511-audio-system-base-sounds]] — the task that built it.
