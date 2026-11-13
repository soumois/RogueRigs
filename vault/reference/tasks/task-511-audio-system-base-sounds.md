---
id: task-511-audio-system-base-sounds
description: "Global task: Data-driven C++ audio system (event -> sound table) on the existing sound classes, with owner-picked CC0 sounds for 8 events."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Audio system and the first 8 sound effects

- request: Owner 2026-11-12, verbatim: (1) "J'aimerais commencer à rajouter les sons, qu'est-ce que tu me proposes pour faire ça vite ?" The main session proposed three slices and asked "Je lance le planner sur la tranche 1 (socle audio + sons de base) ?"; (2) owner: "Oui, lance". Slice 1 as proposed by the main session and accepted by that "Oui, lance": a C++ audio system with an event -> sound table so that adding a sound later is one data line, not code; wired to the existing sound classes (SC_Master, SC_Music, SC_Effects of the options screen); 8 events: shot, impact, enemy death, damage taken by the player, XP pickup, level up, button hover, button click; CC0 sounds (examples cited: Kenney, Sonniss GDC, freesound filtered CC0), two or three candidates per event, the owner picks by ear; mix: limit simultaneous shots and impacts. Out of scope: slice 2 (chest, shop, extractor, turret, shield, heal, level end, death) and slice 3 (music per alert level). The owner did not decide the event list, palette or mix rules himself: they are asked as Q1-Q6. He wants it fast; no capture or test asked; no sound file given. Owner answer 2026-11-12 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- goal: In a run and in the menus, the 8 events of audio-direction slice 1 each play the sound the owner picked, mixed per audio-direction, and a new event or sound is added as one line of data.
- acceptance: A1 One event table maps each of the 8 events to its sound and its mix settings; adding an event sound or swapping a sound needs no code change, only the table (and the call site for a new event); an event with no sound stays silent without error. A2 Each event of audio-direction slice 1 fires at its game moment with its scope (Q1), including UI hover and click in paused screens; a -game run log shows every one of the 8 playing its asset. A3 Mix rules of audio-direction hold: all 8 through SC_Effects under SC_Master; Effects or Master at 0 silences them, Music does not; under sustained fire the simultaneous shots, impacts and XP pickups never exceed their caps (log). A4 Three CC0 candidates per event shown to the owner as Q4 says; the 8 kept sounds are the ones he named, imported, with source files in Art/Source/audio/ and author, link and licence in sound_manifest.json; rejected files stay in Saved/Scratch/. A5 Build green, existing tests pass, tests a change breaks updated, none added. A6 Doc nodes created or updated, audio-direction filled where the work fixed a value, the list of what was checked only by log given to the owner, lint clean, committed.
- gdd_check: Read: art-audio-index, audio-direction (provisional; palette, mix rules, event list open; effects = openly licensed sounds scored against the style guide by the sound agent, listed in sound_manifest.json; M3), milestones (sound is M3 content; M3 ended with the 2026-11-09 submission, so post-submission scope, requested by the owner), agents-index (agent-sound not written: the Executor finds and scores the sounds itself against the palette of Q2, no audio style guide exists), verification-levels. No code read. No conflict. Open values the work needs: event list, palette, mix rules, sources, listening method: Q1-Q6 with defaults; the confirmation writes them into audio-direction (provisional). Not in any slice listed by the main session: explosions (kamikaze, barrel), enemy shots, turret shots; left out unless the owner adds them.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-12, "ok")
- milestone: M2
- next_step: task-511-04-audio-doc
- started: 2026-11-13T08:02:08Z
- waiting_for: none
- executor_tokens: input 260, cache write 940109, cache read 22031525, output 109763
- executor_usage: runs 1, turns 129, context start 16k peak 308k, resumes 3, 888 min, units 3.93 M, USD 11.30
- planning_tokens: input 26, cache write 54769, cache read 468394, output 14795
- planning_usage: a23f106d "Plan audio tranche 1" 1/1 (tasks 511), units 0.19 M, USD 0.66
- result: open

## Questions
Decided 2026-11-12 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-12.
- Q1 Les 8 événements et leur portée : tir = armes du joueur seulement, un son partagé par toutes les armes (un son par arme se fera plus tard en une ligne de table) ; impact = balle du joueur qui touche un ennemi ou un bloc ; mort d'ennemi = un son pour tous les types, les explosions (kamikaze, baril) ne sont pas incluses ; dégât reçu par le joueur ; ramassage d'XP ; montée de niveau ; survol et clic = tous les boutons de tous les écrans, rien sur un bouton grisé. Défaut : comme écrit.
- Q2 Palette : industriel, sec, métallique, peu de réverbération ; chaque candidat est noté contre elle avant de t'être proposé. Défaut : celle-ci.
- Q3 Sources : CC0 uniquement. Packs Kenney d'abord (téléchargement direct, sans compte), puis OpenGameArt filtré CC0 et freesound filtré CC0 (freesound demande un compte : si un candidat en a besoin, je te le signale au lieu de le prendre). Sonniss GDC écarté : licence libre de droits mais pas CC0, et des archives de plusieurs dizaines de Go. Auteur, lien et licence de chaque son gardé dans sound_manifest.json. Défaut : comme écrit.
- Q4 Écoute : 3 candidats par événement (24 fichiers) dans Saved/Scratch/audio-candidates/<événement>/, numérotés, avec une liste (source, durée, note palette). Tu réponds par exemple « tir 2, impact 1, ... » ou « tir : autre série ». Le système C++ est construit avant, donc tu écoutes pendant qu'il est prêt. Non retenus = temporaires (Scratch) ; retenus = gardés, source dans Art/Source/audio/, importés dans Content/RogueRigs/Audio/. Défaut : comme écrit.
- Q5 Mix : tous les effets passent par SC_Effects (sous SC_Master), donc les curseurs Effects et Master des options les règlent ; au plus 4 tirs, 4 impacts et 3 ramassages d'XP en même temps, le plus ancien est coupé ; hauteur aléatoire ±5 % sur tirs, impacts et ramassages ; les sons du monde (tir, impact, mort) sont placés selon leur position à l'écran, les sons d'interface et du joueur sont centrés ; les sons d'interface jouent aussi quand le jeu est en pause (pause, montée de niveau, options). Défaut : comme écrit.
- Q6 Vérification, niveau standard : l'agent n'entend rien, il vérifie par le journal d'une partie -game (chaque événement joue son son, les limites du Q5 tiennent sous un tir nourri, Effects à 0 coupe tout) ; pas de capture, rien de visible ne change ; à la fin tu écoutes en jeu, avec la liste de ce qui n'a été vérifié que par le journal. Défaut : comme écrit.

## Subtasks
1. task-511-01-audio-system (vault/reference/tasks/task-511-audio-system-base-sounds/task-511-01-audio-system.md) — cpp: Audio system with the event table, wired to the sound classes and to the 8 events, mix rules applied.
2. task-511-02-sound-candidates (vault/reference/tasks/task-511-audio-system-base-sounds/task-511-02-sound-candidates.md) — sound: Find three CC0 candidates per event against the palette and get the owner's pick.
3. task-511-03-sound-import (vault/reference/tasks/task-511-audio-system-base-sounds/task-511-03-sound-import.md) — bp: Import the 8 picked sounds and fill the event table.
4. task-511-04-audio-doc (vault/reference/tasks/task-511-audio-system-base-sounds/task-511-04-audio-doc.md) — doc: Document the audio system, its event table and how to add a sound.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-511-01-audio-system]] — step 1, cpp.
- [[task-511-02-sound-candidates]] — step 2, sound.
- [[task-511-03-sound-import]] — step 3, bp.
- [[task-511-04-audio-doc]] — step 4, doc.
