---
id: task-527-02-sound-import
description: "Bp: objective complete sound kept in Art/Source/audio with a manifest entry (placeholder), imported"
parent: task-527-audio-zone-objective-complete
kind: task
status: in-progress
---
# Son à la validation d'un coffre ou d'une console: bp

- goal: objective complete sound kept in Art/Source/audio with a manifest entry (placeholder), imported
- agent: bp
- depends_on: task-527-01-sound-prompts
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
- started: 2026-11-13T16:15:45Z
- waiting_for: none
- result: open

## Relations
- [[task-527-audio-zone-objective-complete]] — parent global task.
