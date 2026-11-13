---
id: task-522-03-sound-import
description: "Bp: rotor loop sound (imported looping) kept in Art/Source/audio with a manifest entry (placeholder), imported"
parent: task-522-audio-distance-sound-rotors
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T14:27:41Z
- waiting_for: none
- result: open

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
