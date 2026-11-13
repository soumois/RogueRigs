---
id: task-511-02-sound-candidates
description: "Sound: Find three CC0 candidates per event against the palette and get the owner's pick."
parent: task-511-audio-system-base-sounds
kind: task
status: wait-answers
---
# Audio system and the first 8 sound effects: sound

- goal: Find three CC0 candidates per event against the palette and get the owner's pick.
- agent: sound
- depends_on: task-511-01-audio-system
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: Saved/Scratch/audio-candidates/<event>/ with 24 numbered files and one list (source, licence, duration, palette note); the owner's pick per event recorded in this node.
- done_when: The owner named one candidate per event (wait-answers until then; a request for another series gives a new series for that event only).
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T07:31:34Z
- waiting_for: owner pick by ear on series 2 (4-6) for impact, mort-ennemi, montee-niveau (or keep 2), survol, clic: Saved/Scratch/audio-candidates/ecoute.html
- result: Series 1 (Kenney, 24 files): owner 2026-11-13 kept tir 1 (laserSmall_001), degat-joueur 3 (error_004), ramassage-xp 2 (select_007); montee-niveau 2 (confirmation_002) kept for now but to review; impact, mort-ennemi, survol, clic refused, owner: "trouver une base de son plus orienté JV". This moves the palette of Q2 (industrial, dry, metallic) toward a video-game style; not decided by the executor, reported for audio-direction. Series 2 (numbers 4-6) for impact, mort-ennemi, montee-niveau, survol, clic from OpenGameArt CC0 packs (Juhani Junkala 512 retro SFX; rubberduck 50 retro synth SFX); listening page Saved/Scratch/audio-candidates/ecoute.html, list liste.md.

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
