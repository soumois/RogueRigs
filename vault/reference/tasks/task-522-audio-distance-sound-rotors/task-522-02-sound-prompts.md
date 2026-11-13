---
id: task-522-02-sound-prompts
description: "Text: 3 English ElevenLabs prompts for the extractor rotor loop (four rotors, one sound, loopable), owner generates and picks by ear"
parent: task-522-audio-distance-sound-rotors
kind: task
status: done
---
# Son des hélices de l'extracteur et code générique de son à distance: text

- goal: 3 English ElevenLabs prompts for the extractor rotor loop (four rotors, one sound, loopable), owner generates and picks by ear
- agent: text
- depends_on: task-522-01-distance-sound-classes
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), sys-audio (vault/reference/tech/systems/core/sys-audio.md), Art/Source/audio/sound_manifest.json
- expected_output: see done_when
- done_when: 3 prompts given to the owner in the report (palette per audio-direction); the owner sent the kept file; it waits otherwise.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 4, cache write 11863, cache read 7166, output 1408
- usage: turns 2, context start 7k peak 12k, resumes 0, 0 min, units 0.02 M, USD 0.09
- runs: 1
- milestone: M2
- started: 2026-11-13T13:39:59Z
- waiting_for: none
- result: Done. 3 English prompts given to the owner (P1 cargo drone hum, P2 hovering transport four rotors soft thump, P3 four-rotor cargo drone whoosh); style-guide: no rule set for sound prompts (palette open), advisory wording applied to P2 and P3. Owner kept a P2 file: Video_game_hovering__#1-1794579713160.wav (ElevenLabs).

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
