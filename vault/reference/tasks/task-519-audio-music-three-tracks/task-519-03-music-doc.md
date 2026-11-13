---
id: task-519-03-music-doc
description: "Doc: sys-audio: music part, how to swap a track"
parent: task-519-audio-music-three-tracks
kind: task
status: done
---
# Musiques : menu principal, chargement, en jeu: doc

- goal: sys-audio: music part, how to swap a track
- agent: doc
- depends_on: task-519-02-music-playback
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: see done_when
- done_when: A5 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-519
- usage: included in the executor run of task-519
- runs: 1
- milestone: M2
- started: 2026-11-13T09:50:29Z
- waiting_for: none
- result: New node sys-music (vault/reference/tech/systems/core/sys-music.md, own node: sys-audio already over 1000 words): URRMusicSubsystem, track rules, Q12/Q13 behaviour, +Music lines and fades, how to swap a track (import_sounds.py --loop, manifest music entry, +Music line), rr.Audio.Music, measured levels and the warm New Run limit. sys-audio gets a music field and relation; systems-core-index, sys-options, sys-run-transition back-links. Lint clean (no warning on these nodes).

## Relations
- [[task-519-audio-music-three-tracks]] — parent global task.
