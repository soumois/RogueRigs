---
id: task-517-01-dig-event
description: "Cpp: looping dig event with fade in/out and table line (silent until the file)"
parent: task-517-audio-dig-sound
kind: task
status: done
---
# Son quand on creuse: cpp

- goal: looping dig event with fade in/out while the player mines, and its table line (silent until the file)
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1, A2, A4 hold with the line silent if no file; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-517
- usage: included in the executor run of task-517
- runs: 1
- milestone: M2
- started: 2026-11-13T09:06:55Z
- waiting_for: none
- result: Commit 0b08afcc. A1: table line Dig (DefaultGame.ini) Volume 1.0, PitchJitter 0, MaxConcurrent 1, centred, bLoop True, LoopFadeIn 0.15, LoopFadeOut 0.15 (Q8), Sound empty = silent; new row fields bLoop/LoopFadeIn/LoopFadeOut; URRAudioSubsystem::SetLoop(Context, Event, bOn) holds one play per loop event (FadeIn on start, FadeOut on release, a restart during the fade out stops the old play through the cap); a paused world stops the loop at once (core ticker), a removed world drops it; a one-shot PlayEvent of a loop row is refused. Call site: ARRDrone::Tick, SetLoop(DigSoundEvent 'Dig', State == Dig) every tick, off in EndPlay. A2: -game L_ArenaM1 Verbose LogRRAudio with SW_Dig set for the check (02 import): rr.Audio.TouchRock (new debug command, miner against the nearest rock face) logs one Play Dig 'loop start, fade in 0.15 s', no further start while the block is mined, 'Loop stop Dig: fade out 0.15 s' then 'Loop Dig ended' when the block breaks; pause menu: 'game paused, stopped at once', restart faded in on resume; death by kamikazes while mining: stop then miner died, no restart; open L_MainMenu while mining: stop and ended; rr.Audio.Stats peak 1. A4: build green (one C4996 Cast<> warning from engine Casts.h, already present in task-516); no audio test family; RogueRigs.Dig 19/19 pass. ALERT at this closing: executor run over the light budget (58 turns, 0.92 M units of 40 / 0.6 M): cause, a new loop mechanism in the audio subsystem plus a debug command to mine on demand, the sound preparation and import done before the check, four -game runs (start/stop, pause, death, level end) and a test run on a wrong family name (RogueRigs.Drone does not exist).

## Relations
- [[task-517-audio-dig-sound]] — parent global task.
