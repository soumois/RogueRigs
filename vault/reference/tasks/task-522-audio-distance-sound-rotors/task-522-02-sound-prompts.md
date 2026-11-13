---
id: task-522-02-sound-prompts
description: "Text: 3 English ElevenLabs prompts for the extractor rotor loop (four rotors, one sound, loopable), owner generates and picks by ear"
parent: task-522-audio-distance-sound-rotors
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T13:39:59Z
- waiting_for: none
- result: open

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
