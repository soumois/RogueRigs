---
id: task-529-gdd-audio-options-sync
description: "Global task: Bring the audio-direction and options-screen gdd nodes in line with tasks 520 to 527 as built; documentation only."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# GDD audio-direction et options-screen alignés sur ce qui est construit

- request: Owner 2026-11-13, relayed by the main session (family comportement, documentation only), verbatim: « fais la tâche pour audio-direction et options-screen ». Relay context: the planner of task-520 to task-528 listed gdd edits for audio-direction and options-screen, executors reported them not made; bring both nodes in line with what was decided and built (Effects slider preview, distance sound base class with explosion child, rotor loop, robot impact near reduction, objective complete one-shot, tabs fix if relevant), note that the palette replacement is open and the new sounds are placeholders; no game run, no capture.
- goal: audio-direction and options-screen state the decisions of task-520 to task-527 as built, with no stale sentence and no invented value (Q1, Q2).
- acceptance: A1 audio-direction holds the lines of task-520 (Effects preview: shot sound, centred, 5 % steps, 0.15 s, paused screens, Master and Music silent), task-522 (generic distance sound, explosion child from the radius edge, one rotor loop for the four rotors, 800 / 2150 uu), task-523 (robot impact 0.6 up to 150 uu, full at 900 uu, wall unchanged), task-527 (one-shot at chest and console validation, 200 / 1075 uu), each value equal to sys-options, sys-audio and sys-audio-distance-sounds; A2 audio-direction description no longer says music per alert level, Sources names the sounds added since as ElevenLabs files, Placeholders names the rotor loop and objective complete sound and says the replacement palette is open; A3 options-screen no longer says the game has no sound, refers to audio-direction for the Effects preview, and states the tab size rule (Q2); A4 no value invented, any mismatch with a sys node reported, lint clean.
- gdd_check: Read audio-direction, options-screen, task-520, task-521, task-522, task-523, task-527, sys-options, sys-audio, sys-audio-distance-sounds. Finding: the four decision lines (task-520, 522, 523, 527) and the options-screen Effects sentence were already applied by task-confirm.js at the confirmation commit fe6973e6; their values match the sys nodes. The "not edited" remarks in the results of task-520 and task-523 are wrong for these two nodes. Remaining stale text: options-screen "The game has no sound yet"; audio-direction description (music per alert level, while one in-game track for now, task-519); Sources counts "7 of the 8" without the sounds added since; Placeholders does not name the new sounds. Tab size (task-521) not in options-screen: asked Q2. Open, kept open: replacement sound palette (line Sound palette), music per alert level, licence and author of the level-up sound. No conflict. Edits: the five text edits of this plan, applied at confirmation. No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-529-01-doc-check
- started: open
- waiting_for: owner answers to Q1, Q2
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q1 Constat : les lignes de task-520, 522, 523 et 527 sont déjà dans audio-direction et options-screen depuis ta confirmation (commit fe6973e6), avec les mêmes valeurs que les nœuds sys-*. Il reste quatre phrases périmées, corrigées à ta confirmation : options-screen « The game has no sound yet » ; la description d'audio-direction (« musique par niveau d'alerte » alors qu'il y a une seule piste en jeu) ; Sources (« 7 des 8 sons » sans les sons ajoutés depuis : explosion, creusage, hélices, validation d'objectif, tous tes fichiers ElevenLabs) ; Placeholders (citer les hélices et la validation d'objectif comme placeholders, palette de remplacement toujours open). Puis une sous-tâche doc vérifie les deux nœuds contre les sys-* et signale tout écart, sans lancer le jeu. Default: oui.
- Q2 Onglets (task-521) : ajouter dans options-screen la règle « les trois onglets gardent la même taille, sélectionnés ou non ». Default: oui.

## Subtasks
1. task-529-01-doc-check (vault/reference/tasks/task-529-gdd-audio-options-sync/task-529-01-doc-check.md) — doc: audio-direction and options-screen checked against the sys nodes after the confirmation edits

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-529-01-doc-check]] — step 1, doc.
