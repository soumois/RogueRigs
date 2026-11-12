---
id: task-509-options-old-workflow-on-master
description: "Global task: Replaces the new-workflow options screen on master with the old-workflow one from exp/old-workflow-options (code, Blueprints, assets, images, docs), renumbers its task to 510, then pushes master."
parent: tasks-done-index
kind: task
status: done
---
# Écran Options : version de l'ancien workflow sur master, puis push

- request: Message 1 : "Ok, garde le résultat de l'ancien workflow. Ce qu'on peut garder en tout cas, c'est le routeur au départ pour les modifications de valeur( de ne pas faire appel au planneur pour ça ), mais il semble que l'ancien workflow a l'air de mieux fonctionner ( plus rapide et meilleur résultat )." Message 2, en réponse au tableau des points du relais : "Garde le visuel et push sur master, retirer les points à retirer ( garder les images comme l'ancien workflow )". Lecture du relais : garder la porte de prévisualisation visuelle ; mettre sur master l'écran Options de l'ancien workflow avec ses images, puis pousser master ; retirer les trois points marqués "à retirer". Faits connus du propriétaire : master porte la version nouveau workflow (tâches 504, 505, 506) ; la version ancien workflow est dans le worktree C:\Workspace\RogueRigs-old, branche exp/old-workflow-options, coupée de c77dc729, dernier commit e0968216, une seule tâche task-500-options-screen (11 sous-tâches) dont l'id entre en collision avec task-500 à task-503 de master. Le propriétaire n'a rien testé en jeu sur l'ancienne version ; l'ancien executor n'a vérifié ni l'expiration du compte à rebours ni les bascules en combat, et a eu deux incidents (L_ArenaM1 supprimé puis restauré, un crash audio). Owner answer 2026-11-12 (relayed by the main session, verbatim): "Ok pour tout" = the plan and the defaults of Q2, Q3, Q4 accepted.
- goal: Master holds the old-workflow options screen instead of the new-workflow one, loses nothing else it has, and is pushed to origin.
- acceptance: A1 master contains the old-workflow options screen as on exp/old-workflow-options at e0968216: its code, Blueprints, assets, images in Art/Source and vault docs (gdd options-screen, sys-options, wbp-options, main menu, pause menu and damage-numbers docs, art prompt node); A2 the new-workflow version is gone: no URRGameUserSettings / URROptionsSettings / RROptionsWidget code, WBP_Options, sound classes or sprites from tasks 504 to 506 remain unless the old version uses the same asset, and no gdd or tech node still describes them; A3 nothing else of master is lost: tasks 500 to 503, 507 and 508, the workflow nodes and scripts, cost-log entries of both histories; L_ArenaM1 and the other maps are identical to master before the merge apart from what the old version changes on purpose; A4 the id collision is resolved: the old task is task-510-options-screen with renumbered subtasks and links, listed in tasks-done-index with its original result; A5 the editor build succeeds and the automation tests pass; the game starts without the audio crash; A6 capture of the Options screen opened from the main menu and from the pause menu at the four resolutions of umg-layout, matching the old-version captures; A7 the countdown expiry and toggles during combat are listed as unverified in the result; A8 lint clean and master pushed to origin.
- gdd_check: Read: none beyond the file list of the branch diff (git diff --stat c77dc729 e0968216 -- vault), one targeted read to know which nodes the old version brings: gdd options-screen replaces master gdd options-menu; tech sys-options replaces sys-game-settings and sys-options-screen; wbp-options, wbp-main-menu, wbp-pause-menu, sys-main-menu, sys-pause-menu, sys-damage-numbers, damage-numbers, ui-button-hover, art-prompts-options-screen-mockup come from the branch. The decided values are those of the old-version gdd node; no conflict, no open value. Ids: 508 is the workflow task, 509 this one, 510 reserved for the imported old task.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-12, "Ok pour tout")
- milestone: M2
- next_step: none
- started: 2026-11-12T13:30:58Z
- waiting_for: none
- executor_tokens: input 170, cache write 290452, cache read 7952627, output 35295
- executor_usage: runs 1, turns 80, context start 16k peak 154k, resumes 6, 64 min, units 1.33 M
- planning_tokens: input 8, cache write 21578, cache read 106665, output 6155
- planning_usage: a5862d1d "Plan screen swap and rule rollback" 1/2 (tasks 508, 509), units 0.07 M
- result: Done. 1cf36a89. A1 old-workflow options screen on master (code, Blueprints, sprites, sound classes, Art/Source images from e0968216 in 1abc6245; vault docs options-screen, sys-options, wbp-options, menu, hover, damage-numbers, art-prompt nodes). A2 the 504-506 code, assets and docs (options-menu, sys-game-settings, sys-options-screen) removed, no gdd or tech node names them; tasks 504, 505, 506 carry 'remplacée par task-510-options-screen'. A3 maps and Config identical to master, tasks 500-503, 507, 508 and both cost histories kept. A4 old task imported as task-510-options-screen. A5 build exit 0, RogueRigs.DamageNumbers and Pickups.OreNumber 11/11, game launch without crash. A6 captures at four resolutions from the main menu and the pause menu match the old ones (task-509-02, Saved/Captures/t509_sheets). A7 not verified: the keep-or-revert countdown reaching 0, a real window mode change, the gameplay toggles during combat, real mouse clicks. A8 lint clean; master pushed to origin by the relay after the owner signed in to GitHub (12717257..811dcc71, origin/master == master); the first push by the executor failed without credentials. The closing commit is pushed by the relay.

## Questions
Decided 2026-11-12 (owner "Ok pour tout"): every default below is accepted; values provisional, owner 2026-11-12.
- Q2 Collision d'ids : l'ancienne task-500-options-screen devient task-510-options-screen (sous-tâches et liens renumérotés), rangée dans les tâches faites avec son résultat d'origine. Défaut : oui.
- Q3 Tâches 504, 505, 506 du nouveau workflow : leurs nœuds restent comme historique dans les tâches faites, avec une ligne "remplacée par task-510" ; leur code, leurs assets et leurs docs (gdd options-menu, sys-game-settings, sys-options-screen) sont retirés. Défaut : oui.
- Q4 Vérification après fusion : build, tests automatiques, et capture de l'écran Options ouvert depuis le menu principal et depuis la pause aux quatre résolutions ; l'expiration du compte à rebours et les bascules en combat restent listées non vérifiées (pas de test ajouté). Défaut : oui.

## Subtasks
1. task-509-01-merge-old-options (vault/reference/tasks/task-509-options-old-workflow-on-master/task-509-01-merge-old-options.md) — executor: bring the old-workflow options screen onto master, remove the new-workflow one, renumber the old task to 510
2. task-509-02-options-capture (vault/reference/tasks/task-509-options-old-workflow-on-master/task-509-02-options-capture.md) — capture: capture the merged Options screen from main menu and pause at the four resolutions
3. task-509-03-options-doc-push (vault/reference/tasks/task-509-options-old-workflow-on-master/task-509-03-options-doc-push.md) — doc: vault docs of the old version in place, new-version docs removed, 504 to 506 marked superseded, lint, push master

## Relations
- [[tasks-done-index]] — parent index.
- [[task-509-01-merge-old-options]] — step 1, executor.
- [[task-509-02-options-capture]] — step 2, capture.
- [[task-509-03-options-doc-push]] — step 3, doc.
