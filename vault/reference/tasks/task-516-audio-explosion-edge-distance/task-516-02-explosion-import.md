---
id: task-516-02-explosion-import
description: "Bp: owner explosion file imported, manifest updated (placeholder marks)"
parent: task-516-audio-explosion-edge-distance
kind: task
status: todo
---
# Son des explosions selon la distance au bord de l'explosion: bp

- goal: owner explosion file imported, manifest updated (placeholder marks)
- agent: bp
- depends_on: task-516-01-explosion-event
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), Art/Source/audio/sound_manifest.json, Saved/Scratch/audio-candidates/explosion/7_elevenlabs_Video_game_explosion_3-1794558943414.wav (owner file, « Pour l'explosion », 2026-11-13, ElevenLabs; prompt: « Video game explosion, single short deep boom with a heavy low-end punch and a quick rumbling tail, powerful but rounded, bass and low-mid frequencies, not shrill, clean and dry, no reverb, no echo, no background noise »)
- expected_output: see done_when
- done_when: A3 holds; the owner file is the Saved/Scratch input (moved to Art/Source/audio as kept, prompt in the manifest); commit.
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
- [[task-516-audio-explosion-edge-distance]] — parent global task.
