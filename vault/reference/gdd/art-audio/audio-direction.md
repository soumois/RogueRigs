---
id: audio-direction
description: "Sound direction: openly licensed sound effects scored against the style guide, music per alert level via the Suno pipeline."
parent: art-audio-index
kind: spec
status: provisional
---
# Audio direction

- Sound effects: openly licensed sounds, found and scored against the style guide by the sound agent; listed in `sound_manifest.json`.
- Music: tracks per alert level, produced through the owner's existing Suno / kie.ai pipeline.
- Sound palette (provisional, owner 2026-11-12, task-511 Q2): industrial, dry, metallic, little reverb; every candidate is scored against it.
- Sources (provisional, owner 2026-11-12, task-511 Q3): CC0 only; Kenney packs first, then OpenGameArt and freesound filtered CC0; Sonniss GDC excluded (not CC0, archive size). Author, link and licence of each kept sound in `sound_manifest.json`.
- Event list, slice 1 (provisional, owner 2026-11-12, task-511 Q1): player shot (one sound shared by all player weapons), player bullet impact (enemy or block), enemy death (all types; explosions not included), damage taken by the player, XP pickup, level up, button hover, button click (every enabled button on every screen). Adding or splitting an event is a line in the event table. Other events (slice 2) and music (slice 3): open.
- Mix rules (provisional, owner 2026-11-12, task-511 Q5): every effect plays through SC_Effects under SC_Master; at most 4 shots, 4 impacts and 3 XP pickups at once, the oldest stopped; ±5 % random pitch on shots, impacts and pickups; world sounds (shot, impact, death) positioned on screen, UI and player sounds centred; UI sounds also play while the game is paused.
- Selection (provisional, owner 2026-11-12, task-511 Q4): 3 candidates per event, the owner picks by ear; rejected candidates are temporary (`Saved/Scratch/`), kept sounds have their source in `Art/Source/audio/`.
- Milestone: M3.

## Relations
- [[art-audio-index]] — parent index.
- [[level-flow]] — the alert bar that drives music.
