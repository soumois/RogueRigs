---
id: task-511-03-sound-import
description: "Bp: Import the 8 picked sounds and fill the event table."
parent: task-511-audio-system-base-sounds
kind: task
status: done
---
# Audio system and the first 8 sound effects: bp

- goal: Import the 8 picked sounds and fill the event table.
- agent: bp
- depends_on: task-511-02-sound-candidates
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Content/RogueRigs/Audio/
- expected_output: see done_when
- done_when: A4 holds; the temporary test sounds are gone from the table; a -game run log shows A2 and A3 with the kept sounds.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-511
- usage: included in the executor run of task-511
- runs: 1
- milestone: M2
- started: 2026-11-13T08:02:09Z
- waiting_for: none
- result: Done (f10a5ab6). 8 kept sounds (7 ElevenLabs by the owner, 1 level-up found by the owner) prepared mono 48 kHz, silence trimmed, levels equalized (loudest 50 ms at -14 dBFS): Art/Source/audio/SW_*.wav, originals in Art/Source/audio/originals, sound_manifest.json (prompts; licence ElevenLabs per the owner's subscription and level-up author/licence unknown, both to confirm by the owner). Level-up cut at 0.69 s (next sound starts at 0.70 s, high band -52 -> -23 dB), 60 ms fade-out. Imported headless by new Tools/unreal/import_sounds.py into /Game/RogueRigs/Audio/SFX/SW_*; table points to them, engine test sounds gone. -game logs Saved/Captures/task-511-03-sounds and -levelup: all 8 play their SW_ asset via SC_Effects; peaks Shot 4/4, Impact 4/4, XPPickup 3/3; hover and click on paused screens; device volumes Effects 0 -> 0 / Music 1, Music 0 -> Effects 1, Master 0 -> both 0; options restored (volumes 100, pause on focus loss 1). Rejected candidates stay in Saved/Scratch/audio-candidates. ALERT budget standard: 129 turns, 3.93 M units at this closing; cause: candidate series and owner pick rounds of 02, then two import tries (script API fix) and three -game runs (one paused by window focus loss, one ended by the miner's death).

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
