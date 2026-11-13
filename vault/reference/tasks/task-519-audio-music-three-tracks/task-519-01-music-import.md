---
id: task-519-01-music-import
description: "Bp: three tracks kept as provided in Art/Source/audio with manifest entries, imported"
parent: task-519-audio-music-three-tracks
kind: task
status: in-progress
---
# Musiques : menu principal, chargement, en jeu: bp

- goal: three tracks kept as provided in Art/Source/audio with manifest entries, imported
- agent: bp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Art/Source/audio/sound_manifest.json, Saved/Scratch/audio-candidates/musique/MainMenu.mp3, Saved/Scratch/audio-candidates/musique/Loading.mp3, Saved/Scratch/audio-candidates/musique/in-game.mp3
- expected_output: see done_when
- done_when: A1 holds; commit.
- editor_state: editor closed
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T09:36:52Z
- waiting_for: none
- result: open

## Relations
- [[task-519-audio-music-three-tracks]] — parent global task.
