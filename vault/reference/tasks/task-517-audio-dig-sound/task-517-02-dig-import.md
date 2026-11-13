---
id: task-517-02-dig-import
description: "Bp: owner dig file imported, manifest entry, set on the line; sys-audio line"
parent: task-517-audio-dig-sound
kind: task
status: done
---
# Son quand on creuse: bp

- goal: owner dig file imported, manifest entry, set on the line; sys-audio line
- agent: bp
- depends_on: task-517-01-dig-event
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), Art/Source/audio/sound_manifest.json, Saved/Scratch/audio-candidates/minage-laser/7_elevenlabs_Mining_laser_running_4-1794559181588.wav (owner file, « Le laser », 2026-11-13, ElevenLabs, prompt unknown: write "prompt unknown" in the manifest)
- expected_output: see done_when
- done_when: A3 holds and sys-audio updated; the owner file is the Saved/Scratch input (moved to Art/Source/audio as kept, a loop: trim so it loops without a click if needed); commit.
- editor_state: editor closed
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-517
- usage: included in the executor run of task-517
- runs: 1
- milestone: M2
- started: 2026-11-13T09:18:20Z
- waiting_for: none
- result: Commit 794134aa. A3: owner file (Saved/Scratch input, ElevenLabs variant 4, 2.88 s stereo 48 kHz) moved to Art/Source/audio/originals/ as kept; prepared into Art/Source/audio/SW_Dig.wav (mono 48 kHz 16-bit, steady sound so no silence trim, last 100 ms crossfaded equal power into the first 100 ms: 2.780 s loop without a click at the seam, loudest 50 ms at -14 dBFS per the manifest rule, peak -4.3 dBFS); Tools/unreal/import_sounds.py gained --loop (sets Looping); imported editor closed as /Game/RogueRigs/Audio/SFX/SW_Dig (2.780 s, 1 channel, looping True); Dig line of DefaultGame.ini points to it. sound_manifest.json: Dig entry, prompt unknown, licence to confirm, status placeholder, à remplacer avant publication. sys-audio: Dig call site (ARRDrone::Tick, EndPlay), loop rows (bLoop, LoopFadeIn, LoopFadeOut, SetLoop, pause and world rules), import --loop, rr.Audio.TouchRock, relation to task-517; lint clean. -game check with slomo 0.2: one Play Dig, rr.Audio.Stats 3.17 s later still active 1 played 1 (loop held past its 2.78 s length), then Loop stop and ended at the block break. Not heard by the executor: loudness against the other sounds and the seam are for the owner's ear. Budget: over the light budget since 01 (see 01 result).

## Relations
- [[task-517-audio-dig-sound]] — parent global task.
