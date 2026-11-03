---
id: task-424-pause-menu-stats
description: "Global task: In-run pause menu with miner stats (global and per weapon), Resume and Exit to main menu, look from an owner-validated ChatGPT mockup."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Pause menu: miner stats screen with Resume and Exit

- request: Owner, 2026-11-03, verbatim (relayed by the main session, in English): "Pause menu: stats screen with Resume and Exit. The pause menu should show the miner's characteristics, per weapon and global, and have two buttons: Resume, and Exit (back to the main menu). For the look, use ChatGPT: give it a reference image of the menu style we already have, and ask it for an example of what this screen would look like. Show me that example for validation. Once I validate it, retrieve the elements from ChatGPT and build the screen from them." Main session notes: no file path given; the reference image is to be found in the repo; the ChatGPT step is done through the browser; order reference image -> ChatGPT mockup -> owner validation gate -> extract elements -> build UMG (C++ logic, thin Blueprint) -> doc; no unrequested tests, bot runs or screenshots. Owner answer 2026-11-03 (relayed by the main session, verbatim): "Ok, all defaults" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8 accepted.
- goal: During a run, Escape (or P, or gamepad Start) pauses the game and opens WBP_PauseMenu, drawn after the owner-validated ChatGPT mockup, listing the miner global stats and one block per owned weapon; Resume unpauses, Exit returns to L_MainMenu with the run reset.
- acceptance: Mockup made with ChatGPT from the reference image and validated by the owner (his words in task-424-01 result) before any element extraction; elements retrieved and saved as PNGs (or reuse of existing sprites recorded) in the mockup node; C++ pause flow and stats snapshot built per [[pause-menu]] (open keys, blocked states, content, Resume, Exit as the run-won Main Menu path), build clean, existing tests of the touched families still pass, no new automated test; WBP_PauseMenu built on a Root Canvas per [[umg-layout]] after the mockup, compiled with warnings as errors; capture checklist C1 to C8 of task-424-06 seen; doc nodes written; gdd pause-menu and main-menu record the decisions; lint clean; committed by path.
- gdd_check: Read 2026-11-03: [[pause-menu]] (created by this plan, status open: proposed defaults Q1 to Q8), [[player-stats]] (approved: miner stats Max health, Move speed, Pickup radius, XP gain, Mining speed, Luck; the sheet shows the sum of global weapon bonuses: damage, fire rate, reload speed, crit chance, crit damage), [[main-menu]] (provisional; run-won "Main Menu" resets the run and opens L_MainMenu directly), [[intermission]] (existing stats screen look from the ChatGPT level-end mockup), [[art-prompts-level-end-screen-mockup]] (reference images, reused sprites T_LevelEnd_Frame, dividers, T_Shop_TitleBar, T_Shop_ButtonContinue, T_Icon_Ore; sheet too small to cut, sprites regenerated through Kie.ai; 3x3 method), [[umg-layout]] (rule), [[sys-gas-core]] (URRPlayerAttributeSet, URRHealthAttributeSet, URRMovementAttributeSet on the miner; URRWeaponAttributeSet per weapon: Damage, FireRate, ReloadSpeed, CritChance, CritDamage, MagazineSize, Ammo, Pierce, present in Source/RogueRigs/Weapons although the doc says "not yet"). No pause menu exists today (no pause input in IMC_Miner, no WBP). Conflict: [[main-menu]] says "Not in scope: a back to menu button ... in pause": the request supersedes it (Q1). Open values: opening keys and blocked states (Q2), exact content and format (Q3), reference image and ChatGPT route (Q4), validation loop (Q5), element extraction and art budget (Q6), verification (Q7), layout (Q8); the look itself stays open until the owner validates the mockup. Repo 2026-11-03: WBP_ShopScreen and WBP_ShopCard clean (not touched here); no task in progress (task-314 todo, unrelated). Scope: M2 UI.
- confirmed_by_owner: yes (2026-11-03, "Ok, all defaults")
- milestone: M2
- next_step: task-424-01-pause-menu-chatgpt-mockup
- started: 2026-11-03T06:07:26Z
- waiting_for: task-424-01 owner mockup validation
- result: open

## Questions
Decided 2026-11-03 (owner "Ok, all defaults"): every default below is accepted; values provisional, owner 2026-11-03.
- Q1 Conflit GDD : le nœud main-menu dit « pas de bouton retour au menu en pause ». Default: ta demande le remplace ; Exit agit comme le bouton « Main Menu » de fin de run gagnée : run réinitialisée (minerai à 0), L_MainMenu ouvert directement, sans écran de chargement ni fondu, sans fenêtre de confirmation.
- Q2 Ouverture : Default: Échap ou Start manette pendant le jeu met en pause et ouvre le menu, la même touche ou Resume le ferme ; P fait la même chose (Échap arrête le Play In Editor). Inaccessible quand un autre écran met déjà le jeu en pause (cartes de level-up, écrans stats et cartes d'intermission, chargement, game over, run gagnée) et dans le menu principal. Curseur affiché, focus clavier sur Resume.
- Q3 Contenu : Default: bloc « global » = les 6 stats du mineur à leur valeur actuelle (vie max en points ; vitesse, rayon de ramassage, gain d'XP, vitesse de minage en bonus % sur la base ; chance en points) puis les bonus globaux d'armes pris (dégâts %, cadence %, rechargement %, chance et dégâts critiques en points) ; puis un bloc par arme possédée (dégâts, cadence, rechargement, chance critique, dégâts critiques, chargeur, perforation, valeurs actuelles de l'arme), emplacements verrouillés non affichés. Textes en anglais écrits dans le widget comme le menu principal, titre « PAUSED », boutons « Resume » et « Exit ».
- Q4 Référence et ChatGPT : Default: image de référence = la maquette ChatGPT de fin de niveau que tu as validée (Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp, l'écran de stats le plus proche) ; l'agent art écrit le prompt en français (contenu de Q3, mêmes éléments de style) et le soumet à ChatGPT dans le navigateur sur ton compte ; si l'agent n'a pas accès au navigateur, l'Executor s'arrête et te donne le prompt et l'image à coller, tu ramènes la réponse.
- Q5 Validation : Default: l'Executor te montre la maquette et attend ; toute remarque = un nouveau tour dans la même conversation ChatGPT ; rien n'est extrait ni construit avant « je valide », sauf la logique C++ (pause, stats, boutons) qui ne dépend pas du look et peut tourner pendant l'attente.
- Q6 Éléments : Default: après validation, l'agent demande à ChatGPT, dans la même conversation, la planche des éléments de l'écran ; les sprites existants sont réutilisés quand l'élément correspond (cadre et séparateurs de fin de niveau, barres de titre, bouton Continue, icône minerai) ; un élément nouveau est découpé dans la planche si sa résolution suffit, sinon redessiné via Kie.ai GPT Image 2 à partir de la planche (2 variantes, 6 crédits chacune, 36 crédits au plus), cadres en 3x3 ; l'agent choisit d'après la maquette, tu les vois dans la capture finale.
- Q7 Vérification : Default: capture -game par l'agent capture aux quatre résolutions de la règle (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080) du menu ouvert en cours de run avec deux armes possédées, comparée à la maquette validée, plus Resume et Exit déclenchés par une commande de debug rr.Debug.PauseMenu (mêmes fonctions que les boutons) ; aucun nouveau test automatisé.
- Q8 Disposition (provisoire, tu retouches à la main) : Default: WBP_PauseMenu dans Content/RogueRigs/UI/, Root Canvas et ancres, positions en fractions d'écran mesurées sur la maquette validée, fond noir semi-transparent sur l'arène en pause comme l'écran de stats.

## Subtasks
1. task-424-01-pause-menu-chatgpt-mockup (vault/reference/tasks/task-424-pause-menu-stats/task-424-01-pause-menu-chatgpt-mockup.md) — art: reference image + French prompt, ChatGPT mockup in the browser, owner validation gate
2. task-424-02-pause-menu-elements (vault/reference/tasks/task-424-pause-menu-stats/task-424-02-pause-menu-elements.md) — art: retrieve the screen elements from ChatGPT (asset sheet), map reuse versus new, make the new PNGs
3. task-424-03-pause-menu-cpp (vault/reference/tasks/task-424-pause-menu-stats/task-424-03-pause-menu-cpp.md) — cpp: pause input and flow, stats snapshot (global and per weapon), widget base with Resume and Exit, rr.Debug.PauseMenu
4. task-424-04-pause-menu-sprites-import (vault/reference/tasks/task-424-pause-menu-stats/task-424-04-pause-menu-sprites-import.md) — bp: import the new T_Pause_* PNGs as UI textures in /Game/RogueRigs/UI/PauseMenu
5. task-424-05-pause-menu-widget (vault/reference/tasks/task-424-pause-menu-stats/task-424-05-pause-menu-widget.md) — bp: WBP_PauseMenu on the C++ base, built after the validated mockup on a Root Canvas with anchors
6. task-424-06-pause-menu-capture (vault/reference/tasks/task-424-pause-menu-stats/task-424-06-pause-menu-capture.md) — capture: -game captures of the pause menu at four resolutions against the validated mockup, Resume and Exit by debug command
7. task-424-07-doc-pause-menu (vault/reference/tasks/task-424-pause-menu-stats/task-424-07-doc-pause-menu.md) — doc: write sys-pause-menu and wbp-pause-menu, link them in their indexes, update touched docs

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-424-01-pause-menu-chatgpt-mockup]] — step 1, art.
- [[task-424-02-pause-menu-elements]] — step 2, art.
- [[task-424-03-pause-menu-cpp]] — step 3, cpp.
- [[task-424-04-pause-menu-sprites-import]] — step 4, bp.
- [[task-424-05-pause-menu-widget]] — step 5, bp.
- [[task-424-06-pause-menu-capture]] — step 6, capture.
- [[task-424-07-doc-pause-menu]] — step 7, doc.
- [[pause-menu]] — the spec this task builds; holds the decided values.
- [[main-menu]] — Exit destination; its pause out-of-scope line is replaced (Q1).
- [[art-prompts-level-end-screen-mockup]] — the style reference and the sprites that can be reused.
- [[umg-layout]] — the layout rule of the widget.
