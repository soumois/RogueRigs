---
id: task-527-01-sound-prompts
description: "Text: 3 English ElevenLabs prompts for the zone objective complete sound (chest opened, console hacked), owner generates and picks by ear"
parent: task-527-audio-zone-objective-complete
kind: task
status: done
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
- model: opus
- tokens: input 8, cache write 14143, cache read 19166, output 2411
- usage: turns 3, context start 7k peak 14k, resumes 1 (message x1), 1 min, units 0.03 M, USD 0.12
- runs: 1
- milestone: M2
- started: 2026-11-13T16:15:33Z
- waiting_for: none
- result: 3 prompts P1..P3 given (style-guide pass x3 after one revision); owner sent Video_game_objective_#2-1794586486206.wav (ElevenLabs variant 2; name prefix 'Video game objective' matches P1 and P3, both start so)

## Relations
- [[task-527-audio-zone-objective-complete]] — parent global task.
