---
id: task-519-02-music-playback
description: "Cpp: music per screen, looped, levels and fades in config, through the Music class"
parent: task-519-audio-music-three-tracks
kind: task
status: done
---
# Musiques : menu principal, chargement, en jeu: cpp

- goal: music per screen, looped, levels and fades in config, through the Music class
- agent: cpp
- depends_on: task-519-01-music-import
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A2 to A5 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-519
- usage: included in the executor run of task-519
- runs: 1
- milestone: M2
- started: 2026-11-13T09:38:20Z
- waiting_for: none
- result: Commit a92783fa. URRMusicSubsystem (Audio/RRMusicSubsystem.*): core ticker picks MainMenu (ARRMainMenuGameMode world), Loading (IsLevelLoading; none during the 0.3 s fade out of a prepared Continue, no loading screen there), InGame (other game worlds, restarted at each OnLevelReady or new world, Q13); fade out old 1 s / fade in new 1 s (MusicFadeIn/Out); plays are UI sounds persisting across travel through SC_Music, so pause, level-up, stats, shop, run-lost screens keep the in-game track (Q12). Config rows +Music=(Track,Sound,Volume): MainMenu 1.0, Loading 1.0, InGame 0.5 (Q10). rr.Audio.Music [seek <s>]; Log lines per track change and loop. Build green (build only: no audio test family). Three -game runs with rr.Audio.Record and cross-correlation against the mp3: menu at -3 dB of the file, in game at -9 dB (6 dB quieter), loading heard on Retry and a cold New Run; in game continuous through pause, level-up, stats, shop, run lost; restarts at 0 after Continue and Retry; menu after Exit and run-lost Main Menu; loop seen at 163.2 s (InGame) and 170.0 s (MainMenu). Findings: on a warm New Run (about 4 s, 2 s of it a blocked map load) the loading track (first second silent in the file) is barely heard and the menu track holds its level through the blocking load; level-up (cheat, 1 level) about 7-8 dB above the in-game music in that second.

## Relations
- [[task-519-audio-music-three-tracks]] — parent global task.
