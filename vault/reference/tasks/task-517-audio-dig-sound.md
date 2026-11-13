---
id: task-517-audio-dig-sound
description: "Global task: Laser mining sound: one looping sound while the player digs, fade in and out; import of the owner file."
parent: tasks-done-index
kind: task
status: done
---
# Son quand on creuse

- request: « ok pour les questions. Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8: 0.15 in et out
Q9 : oui" = the plan and the defaults of Q8 accepted.

il faut aussi ce même systéme pour la mort des ennemis.

Applique le son d'xp au minerai ramassé pour l'instant.

Il faudrait aussi le prompt pour les explosions et qui changerait aussi selon la distance, sachant que la distance se calculerait par rapport au bord de l'explosion.

Il faudrait aussi le son quand on creuse. » (owner 2026-11-13, answer to task-512/513 questions). Just before: « Je ne publierais pas le jeu avec ces sons, c'est juste pour avoir des sons placeholder qui seront remplacé plus tard. Ou est-ce que je peux avoir le son coupé, car je n'ai pas entendu le level up. » (level up: open diagnosis by the owner, not planned). Point: "Il faudrait aussi le son quand on creuse." The relay gives the prompt in the chat; this task only wires and imports. Owner answer to Q7 (2026-11-13): « Pour le son de creuser : C'est du minage laser, là on entend que c'est une foreuse. On aura qu'un seul son, on fera juste un fade in/out. Il n'y a que le joueur qui peut creuser dans le jeu »
- goal: While the player is mining, one laser mining sound plays in a loop, fading in when mining starts and out when it stops, through its own event line; the owner's generated file is imported and set on it.
- acceptance: A1 the dig event has its own table line: one looping play at most, centred, fade in and fade out durations of Q8 read from the line; A2 in a -game run with Verbose LogRRAudio, starting to mine logs one start, continued mining starts nothing more, stopping logs the fade out and the sound ends; no sound is left playing after death, pause or level end; A3 the owner file is imported as an SW_ asset, in sound_manifest.json marked placeholder, set on the line; A4 build green, audio tests pass; sys-audio lists the event and its call site.
- gdd_check: Read audio-direction, sys-audio. Not in the slice 1 event list: added by the owner. Q7 decided by the owner: only the player mines (laser mining), one looping sound with fade in and out. Q8 decided by the owner: fade 0.15 s in and out. The file came from the owner (« Le laser »): Saved/Scratch/audio-candidates/minage-laser/7_elevenlabs_Mining_laser_running_4-1794559181588.wav, his prompt unknown. No code or dig gdd node read.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "Q8: 0.15 in et out
Q9 : oui")
- milestone: M2
- next_step: none
- started: 2026-11-13T09:06:55Z
- waiting_for: none
- executor_tokens: input 140, cache write 151935, cache read 7354422, output 46828
- executor_usage: runs 1, turns 70, context start 17k peak 165k, resumes 0, 13 min, units 1.16 M, USD 3.17
- planning_tokens: input 10, cache write 20096, cache read 213155, output 4963
- planning_usage: aec6de57 "Plan shot and wall-impact sound changes" 1/8 (tasks 512, 513, 514, 515, 516, 517, 518, 519), units 0.07 M, USD 0.24
- result: Done. Commit 794134aa. A3: owner file (Saved/Scratch input, ElevenLabs variant 4, 2.88 s stereo 48 kHz) moved to Art/Source/audio/originals/ as kept; prepared into Art/Source/audio/SW_Dig.wav (mono 48 kHz 16-bit, steady sound so no silence trim, last 100 ms crossfaded equal power into the first 100 ms: 2.780 s loop without a click at the seam, loudest 50 ms at -14 dBFS per the manifest rule, peak -4.3 dBFS); Tools/unreal/import_sounds.py gained --loop (sets Looping); imported editor closed as /Game/RogueRigs/Audio/SFX/SW_Dig (2.780 s, 1 channel, looping True); Dig line of DefaultGame.ini points to it. sound_manifest.json: Dig entry, prompt unknown, licence to confirm, status placeholder, à remplacer avant publication. sys-audio: Dig call site (ARRDrone::Tick, EndPlay), loop rows (bLoop, LoopFadeIn, LoopFadeOut, SetLoop, pause and world rules), import --loop, rr.Audio.TouchRock, relation to task-517; lint clean. -game check with slomo 0.2: one Play Dig, rr.Audio.Stats 3.17 s later still active 1 played 1 (loop held past its 2.78 s length), then Loop stop and ended at the block break. Not heard by the executor: loudness against the other sounds and the seam are for the owner's ear. Budget: over the light budget since 01 (see 01 result).

## Questions
Decided 2026-11-13 (owner "Q8: 0.15 in et out
Q9 : oui"): the defaults of Q8 are accepted; values provisional, owner 2026-11-13.
- Q7 Creusage : un son court à chaque coup de creusage du mineur (le joueur) sur un bloc, centré comme les sons du joueur, au plus 2 à la fois, ±5 % de hauteur ; pas de son continu, l'extracteur n'est pas concerné. Default: oui. Decided (owner 2026-11-13): default refused; laser mining, one looping sound with fade in/out, only the player mines.
- Q8 Durées de fondu du son de minage : Default: 0.2 s en entrée, 0.3 s en sortie, sans variation de hauteur, réglables dans la table. Owner answer 2026-11-13: « Q8: 0.15 in et out » -> Default replaced: 0.15 s in, 0.15 s out, no pitch jitter, set in the table.

## Subtasks
1. task-517-01-dig-event (vault/reference/tasks/task-517-audio-dig-sound/task-517-01-dig-event.md) — cpp: looping dig event with fade in/out and table line (silent until the file)
2. task-517-02-dig-import (vault/reference/tasks/task-517-audio-dig-sound/task-517-02-dig-import.md) — bp: owner dig file imported, manifest entry, set on the line; sys-audio line

## Relations
- [[tasks-done-index]] — parent index.
- [[task-517-01-dig-event]] — step 1, cpp.
- [[task-517-02-dig-import]] — step 2, bp.
