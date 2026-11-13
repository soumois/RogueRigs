---
id: task-516-02-explosion-import
description: "Bp: owner explosion file imported, manifest updated (placeholder marks)"
parent: task-516-audio-explosion-edge-distance
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-516
- usage: included in the executor run of task-516
- runs: 1
- milestone: M2
- started: 2026-11-13T09:04:04Z
- waiting_for: none
- result: Commit 9a5bee3c. A3: owner file (Saved/Scratch input, ElevenLabs variant 3, 1.0 s stereo) moved to Art/Source/audio/originals/ as kept; prepared per the manifest rules into Art/Source/audio/SW_Explosion.wav (mono 48 kHz 16-bit, trimmed 0-0.930 s, 10 ms fade-out, loudest 50 ms at -14 dBFS, peak -4.1 dBFS); imported by Tools/unreal/import_sounds.py (editor closed) as /Game/RogueRigs/Audio/SFX/SW_Explosion (0.930 s, 1 channel); Explosion line of DefaultGame.ini points to it. sound_manifest.json: Explosion entry with origin, prompt, licence to confirm; top-level status and a status field on every kept sound: placeholder, à remplacer avant publication. -game check: real barrel explosion 300 uu beyond the edge logs SW_Explosion at volume 0.721.

## Relations
- [[task-516-audio-explosion-edge-distance]] — parent global task.
