---
id: task-527-01-sound-prompts
description: "Text: 3 English ElevenLabs prompts for the zone objective complete sound (chest opened, console hacked), owner generates and picks by ear"
parent: task-527-audio-zone-objective-complete
kind: task
status: todo
---
# Son à la validation d'un coffre ou d'une console: text

- goal: 3 English ElevenLabs prompts for the zone objective complete sound (chest opened, console hacked), owner generates and picks by ear
- agent: text
- depends_on: none
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
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-527-audio-zone-objective-complete]] — parent global task.
