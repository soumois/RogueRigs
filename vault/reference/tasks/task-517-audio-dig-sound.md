---
id: task-517-audio-dig-sound
description: "Global task: Dig sound: one event at each dig hit of the miner on a block; import of the owner file."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Son quand on creuse

- request: « ok pour les questions.

il faut aussi ce même systéme pour la mort des ennemis.

Applique le son d'xp au minerai ramassé pour l'instant.

Il faudrait aussi le prompt pour les explosions et qui changerait aussi selon la distance, sachant que la distance se calculerait par rapport au bord de l'explosion.

Il faudrait aussi le son quand on creuse. » (owner 2026-11-13, answer to task-512/513 questions). Just before: « Je ne publierais pas le jeu avec ces sons, c'est juste pour avoir des sons placeholder qui seront remplacé plus tard. Ou est-ce que je peux avoir le son coupé, car je n'ai pas entendu le level up. » (level up: open diagnosis by the owner, not planned). Point: "Il faudrait aussi le son quand on creuse." The relay gives the prompt in the chat; this task only wires and imports.
- goal: Digging plays a sound as decided in Q7, through its own event line; the owner's generated file is imported and set on it.
- acceptance: A1 the dig event plays as in Q7 with its own table line (cap 2, ±5 % pitch, centred); A2 in a -game run with Verbose LogRRAudio digging logs the plays; A3 the owner file is imported as an SW_ asset, in sound_manifest.json marked placeholder, set on the line; A4 build green, audio tests pass; sys-audio lists the event and its call site.
- gdd_check: Read audio-direction, sys-audio. Not in the slice 1 event list: added by the owner. Open: what digs and per hit or continuous (Q7). The file comes from the owner. No code or dig gdd node read.
- verification_level: light
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-517-01-dig-event
- started: open
- waiting_for: owner answer to Q7, confirmation, and later his dig file
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q7 Creusage : un son court à chaque coup de creusage du mineur (le joueur) sur un bloc, centré comme les sons du joueur, au plus 2 à la fois, ±5 % de hauteur ; pas de son continu, l'extracteur n'est pas concerné. Default: oui.

## Subtasks
1. task-517-01-dig-event (vault/reference/tasks/task-517-audio-dig-sound/task-517-01-dig-event.md) — cpp: dig event and table line (silent until the file)
2. task-517-02-dig-import (vault/reference/tasks/task-517-audio-dig-sound/task-517-02-dig-import.md) — bp: owner dig file imported, manifest entry, set on the line; sys-audio line

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-517-01-dig-event]] — step 1, cpp.
- [[task-517-02-dig-import]] — step 2, bp.
