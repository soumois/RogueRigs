---
id: task-518-01-level-up-audible
description: "Cpp: level up sound audible in game: cause found and fixed"
parent: task-518-audio-level-up-not-heard
kind: task
status: done
---
# Son du level up inaudible en jeu: cpp

- goal: level up sound audible in game: cause found and fixed
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini, Art/Source/audio/SW_LevelUp.wav
- expected_output: see done_when
- done_when: A1 to A4 of the global hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-518
- usage: included in the executor run of task-518
- runs: 1
- milestone: M2
- started: 2026-11-13T09:20:31Z
- waiting_for: none
- result: A1 cause (-game runs, Verbose LogRRAudio, game mix recorded with the new rr.Audio.Record): the LevelUp play was neither stopped, cut by the pause nor on a silenced path; it played in full in the output (correlation 0.91 with the file) but at 0.35 of the file level, like every centred play, and 93 % of SW_LevelUp is below 200 Hz (79 % below 100 Hz): above 200 Hz it sat 2 to 20 dB under the shots and the XP pickups that start in the same frame, so it was masked and too quiet. Fix: LevelUp row Volume 1.0 -> 2.8 = the level of the file played alone (provisional, by ear). A2 after run: Play LevelUp at a real level up, volume 2.800, class SC_Effects, no cap stop; recorded at gain 0.94 to 0.99 of the file over its full 0.69 s (+8.5 dB in the mix); a play without pause logs End after 1.70 s (sound 0.69 s); the real level-up play logs no End line because the choice pause starts before the engine reports it (known UI-row gotcha), the recording shows it whole. 6 clipped samples in a busy moment. A3 build green; no audio test family exists, build only. A4 sys-audio updated (levels, rr.Audio.Record, End lines, unfocused window silent). Unheard by the agent: owner to check by ear. Listening clips (temporary): Saved/Scratch/task-518/. Commit a2e61012.

## Relations
- [[task-518-audio-level-up-not-heard]] — parent global task.
