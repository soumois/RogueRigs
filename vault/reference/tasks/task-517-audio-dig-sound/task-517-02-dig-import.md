---
id: task-517-02-dig-import
description: "Bp: owner dig file imported, manifest entry, set on the line; sys-audio line"
parent: task-517-audio-dig-sound
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-517-audio-dig-sound]] — parent global task.
