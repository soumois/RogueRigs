---
id: task-522-03-sound-import
description: "Bp: rotor loop sound (imported looping) kept in Art/Source/audio with a manifest entry (placeholder), imported"
parent: task-522-audio-distance-sound-rotors
kind: task
status: done
---
# Son des hélices de l'extracteur et code générique de son à distance: bp

- goal: rotor loop sound (imported looping) kept in Art/Source/audio with a manifest entry (placeholder), imported
- agent: bp
- depends_on: task-522-02-sound-prompts
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), Art/Source/audio/sound_manifest.json
- expected_output: see done_when
- done_when: Source file and manifest entry committed, asset imported under /Game/RogueRigs/Audio/SFX; commit.
- editor_state: editor closed for the import commandlet: ask the owner to close it. Editor note 2026-11-13: the owner was in PIE and the unreal MCP server refused connections (ECONNREFUSED) in the main session.
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-522
- usage: included in the executor run of task-522
- runs: 1
- milestone: M2
- started: 2026-11-13T14:27:41Z
- waiting_for: none
- result: Done (e35d9c9d). Owner file (P2, variant 1, 4.48 s stereo) kept as Art/Source/audio/originals/elevenlabs_Video_game_hovering_1-1794579713160.wav; prepared Art/Source/audio/SW_ExtractorRotors.wav: mono 48 kHz, last 100 ms equal-power crossfaded into the start (seam step 0.0018 vs typical 0.0014), loudest 50 ms at -14 dBFS, 4.38 s; manifest entry ExtractorRotors (placeholder); imported by import_sounds.py --loop as /Game/RogueRigs/Audio/SFX/SW_ExtractorRotors (looping=True). Import by commandlet, editor closed, no editor/MCP needed.

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
