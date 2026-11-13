---
id: sys-music
description: "Music as built: URRMusicSubsystem, one looped track per screen (menu, loading, in game), levels and fades in DefaultGame.ini, how to swap a track."
parent: systems-core-index
kind: doc
---
# Music

- responsibility: plays one looped track per screen through the Music sound class, changes it with fades, restarts the in-game track at each level ([[task-519-audio-music-three-tracks]], commit a92783fa).
- main_classes: `Audio/RRMusicSubsystem.h/.cpp`: `URRMusicSubsystem` (GameInstance subsystem, core ticker that runs while paused). `FRRMusicTrackRow` and the music fields of `URRAudioSettings` in `Audio/RRAudioSettings.h`.
- entry_points: none to call. Each frame it picks the wanted track: `Loading` while `URRRunTransitionSubsystem::IsLevelLoading` (New Run, run-lost Retry, Continue with the loading screen); none during the screen fade out of a Continue whose next level is already prepared (no loading screen there); `MainMenu` in a world of `ARRMainMenuGameMode`; `InGame` in any other game world. `InGame` starts again from its start on each `OnLevelReady` or new world (Q13).
- screens: pause, level-up, stats, shop, run won and run lost screens keep the in-game track unchanged (Q12): the plays are UI sounds (never paused by the game) and persist across map travels (`CreateSound2D` with `bPersistAcrossLevelTransition`).
- data: `Config/DefaultGame.ini` `[/Script/RogueRigs.RRAudioSettings]`: one `+Music=(Track,Sound,Volume)` line per track and `MusicFadeIn`, `MusicFadeOut` (s). A change fades the old play out and starts the new track from its start, faded in. Values Q10, Q11 (provisional, owner 2026-11-13): `MainMenu` 1.0, `Loading` 1.0, `InGame` 0.5; fades 1.0 s. Every play uses `MusicSoundClass` of `URROptionsSettings` ([[sys-options]]), so the Music and Master sliders set it. Assets `/Game/RogueRigs/Audio/Music/SW_Music*`; owner Suno mp3, kept byte for byte, placeholders, in `Art/Source/audio/` and the `music` section of `sound_manifest.json`.
- swap_a_track: put the file (mp3 or WAV, as provided or prepared) in `Art/Source/audio/` under the asset name, update its `music` entry in `sound_manifest.json`, import with `Tools/unreal/import_sounds.py /Game/RogueRigs/Audio/Music --loop <file>` (editor closed; same name replaces the asset), then edit the `Sound` or `Volume` of its `+Music` line. A new screen track also needs its rule in `URRMusicSubsystem::GetWantedTrack`.
- debug: track changes and loops are `Log` lines of `LogRRAudio` (`Music <old> -> <new>`, `Music <track> looped`). `rr.Audio.Music [seek <s>]` logs the state; `seek` restarts the current track at that second without fade (hear the loop point). `rr.Audio.Record` ([[sys-audio]]) measures the mix.
- depends_on: [[sys-options]] (sound classes), [[sys-audio]] (settings object, log category, recording), the run transition ([[sys-run-transition]]).
- implements: [[audio-direction]] (music: one in-game track for now, owner 2026-11-13; alert-level tracks stay open).
- gotchas: measured with `rr.Audio.Record`: the menu track reaches the output at -3 dB of the file, the in-game one at -9 dB. A warm New Run loads in about 4 s, 2 s of it a blocking map load with no audio update: the menu track holds its level through it and the loading track (first second silent in the file) is barely heard; a cold load (10 s) lets it play. While the game is paused the playback percent stops updating: the `looped` line and the percent of `rr.Audio.Music` come late, the sound itself goes on.

## Relations
- [[systems-core-index]] — parent index.
- [[sys-audio]] — sound effects of the same audio system.
- [[sys-options]] — the Music and Master sliders it plays under.
- [[sys-run-transition]] — loading phase and level ready events it follows.
- [[audio-direction]] — the music direction it implements.
- [[task-519-audio-music-three-tracks]] — the task that built it.
