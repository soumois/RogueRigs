---
id: task-511-02-sound-candidates
description: "Sound: Find three CC0 candidates per event against the palette and get the owner's pick."
parent: task-511-audio-system-base-sounds
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-511
- usage: included in the executor run of task-511
- runs: 1
- milestone: M2
- started: 2026-11-13T07:31:34Z
- waiting_for: none
- result: Owner picks 2026-11-13 (series 1 and 2 dropped; he generated sounds with ElevenLabs Sound Effects and found the level-up sound himself; files Saved/Scratch/audio-candidates/<event>/7_*): tir 7_elevenlabs_Arcade_video_game_la_3, impact 7_elevenlabs_Arcade_video_game_bu_2, mort-ennemi 7_elevenlabs_Arcade_video_game_ro_1, degat-joueur 7_elevenlabs_Video_game_player_hu_3, ramassage-xp 7_elevenlabs_Video_game_item_pick_4, montee-niveau 7_owner_Level_Up_Sound_Effect_Woofle_Tests_a_Game_Stinger.mp3 (cut to the first low sound, about 0.6 s), survol 7_elevenlabs_Video_game_menu_butt_2-1794555992785, clic 7_elevenlabs_Video_game_menu_butt_2-1794556032654. This departs from Q3 (CC0 only): ElevenLabs licence per the owner's subscription, level-up origin and licence unknown, both to confirm by the owner. Q2 palette not restated by him ("plus orienté JV", metallic impact "trop aigu", "ça part vite dans l'aigu"). ALERT budget standard: 104 turns, 3.07 M units at this closing; cause: two candidate series (Kenney, then OpenGameArt) with downloads and measurements, then the owner's own ElevenLabs series, plus the pause and resume.

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
