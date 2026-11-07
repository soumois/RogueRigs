---
id: task-484-death-screen
description: "Global task: Run-lost screen when the miner's health reaches 0: death sequence, appearance animation, mockup look, recap, Retry and Main Menu. Open to run or review it."
parent: tasks-done-index
kind: task
status: done
---
# Death screen with Retry and Main Menu

- request: Owner, verbatim (French): "Ok, il faudrait faire l'écran de mort lorsque les pv du joueur arrive à 0. Qu'est ce que tu proposes en animation d'apparition et comme visuel. Il faudrait un bouton Retry et main menu." The owner asked for proposals: the options and defaults are Q1 to Q10 below; his answers are appended here at confirmation. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, Q10 accepted.
- goal: When the miner's health reaches 0 the player sees a short death sequence, then a dedicated run-lost screen that animates in, in the look of the level-end and pause screens, with a short recap and two buttons: Retry (new run at level 1) and Main Menu. Every value is in death-screen.
- acceptance: A1 At 0 health: controls cut, the game runs at 30 % speed for 1 s real time, then the screen appears and gameplay is paused (death-screen Death sequence). A2 The screen animates in as death-screen Appearance (backdrop and desaturation, title drop, recap frame, buttons last, about 1.4 s); a click or key during it jumps to the end state. A3 The look follows the mockup the owner validated, built on a Canvas root with anchors (umg-layout), readable and unclipped at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080. A4 Title and subtitle come from the screen text file in the company voice, checked by the style guide; the recap shows the level reached, robots destroyed over the run and the Duskite held at death, each matching the run. A5 Retry starts a new run at level 1 through the loading screen as main-menu New Run: ore 0, cards, weapons, miner level and XP reset, full health. A6 Main Menu resets the run and opens L_MainMenu as the run-won Main Menu button. A7 Focus on Retry, Enter and gamepad confirm work, mouse cursor shown, HUD hidden behind; on a loss the old GAME OVER text and the RUN LOST / Play again panel never appear; the run-won and level-success screens and the pause menu rules are unchanged. A8 A missed shuttle shows the same screen with its own title and subtitle. A9 Build clean; automated tests that this change breaks (end screen, restart button lookups) are updated and pass.
- gdd_check: Read: gdd-index, run-index, run-structure, main-menu, pause-menu, intermission, ore-economy, miner, art-prompts-level-end-screen-mockup, art-prompts-pause-menu-mockup; tech docs read only to state what exists today (no code read): blueprints-ui-index, wbp-alert-objectives (EndScreen with RUN LOST and Play again calling RestartLevel), wbp-health-status (GAME OVER text on OnGameOver), umg-layout, verification-levels. Conflicts: main-menu says a back-to-menu button on the run-lost screen is not in scope (owner now asks for it); main-menu and run-structure name the loss button Play again (now Retry); miner says M1 death = Game Over text and level restart after 3 s (superseded). The edits update the three nodes. Open values needed: death sequence, appearance animation, look, texts, recap, Retry and Main Menu behaviour, missed-shuttle case: none existed, proposed as Q1 to Q10 and written as defaults in the new gdd node death-screen. Main menu exists (task-338). Milestone: M2 run loop, in scope (main menu, pause menu and run-won screen already built). Verification standard: new screen, new widget and mockup.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Ok, tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-07T00:57:21Z
- waiting_for: none
- executor_tokens: input 438, cache write 531695, cache read 55348828, output 176859
- executor_usage: runs 2, turns 214, context start 17k peak 477k, resumes 1, 50 min, units 7.08 M
- planning_tokens: input 42, cache write 85956, cache read 1026467, output 24255
- planning_usage: ac67649c "Plan death screen request" 1/1 (tasks 484), units 0.33 M
- result: Done. A1-A8 seen in -game captures (Saved/Captures/rl06_owner/finals_4res.png, sequence_and_buttons.png): death = 0.3 speed for 1 s real time then pause; WBP_RunLost appears (backdrop and grey level, title drop, recap, buttons, about 1.4 s, skip by click or key); mockup look at the four resolutions; CONTRACT TERMINATED and CLAIM SECURED from screen_texts.json (style guide pass); Retry = loading screen then level 1 with Duskite 0; Main Menu opens L_MainMenu; no GAME OVER, no RUN LOST panel, HUD hidden. A9: build clean, tests Player.GameOver, Integration.ChasersKillMinerGameOver, ShuttleTimeoutLoss, ConsoleExtractionWin and new DeathRunLostScreen pass. Not seen on screen: non-zero recap values (tests cover them), gamepad confirm, key skip. Executor run over the standard budget (214 turns, 7.08 M units): C++ slice across run flow, HUD, controller and tests, MCP driven by hand through a Node JSON-RPC client, captures. Commits 345d6156 to 5364acc9.

## Questions
Decided 2026-11-07 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-07.
- Q1 Enchaînement à la mort, avant l'écran. A: commandes coupées, le jeu passe au ralenti (30 %) pendant 1 s, caméra fixe, puis l'écran apparaît et le jeu se met en pause. B: pas de ralenti, l'écran apparaît tout de suite. C: comme A, plus une animation de mort du mineur (clip Mixamo à importer, une sous-tâche animation en plus). Défaut: A.
- Q2 Animation d'apparition. A « impact »: fond noir de 0 à 75 % et niveau désaturé derrière (0,6 s), le titre tombe en grand (échelle 1,4 vers 1, fondu, 0,25 s, petite secousse), le cadre de récap monte en fondu (0,3 s), les boutons arrivent en dernier (0,2 s), environ 1,4 s au total ; un clic ou une touche pendant l'animation la saute. B « signal perdu »: le flux du drone grésille (parasites, lignes de balayage, 0,5 s), « SIGNAL LOST » clignote, puis le panneau en fondu ; demande un matériau de parasites en plus. C sobre: tout apparaît en fondu en 0,5 s. Pas de son dans tous les cas. Défaut: A.
- Q3 Visuel. A: même famille que l'écran de fin de niveau et la pause (cadre riveté, barres de titre, boutons crème rouillés), titre en rouge ; d'abord une maquette Kie.ai image-to-image à partir de la maquette de fin de niveau, comme pour la pause: 1 image 16:9 1K par tour, 6 crédits, rien n'est construit avant ton « je valide » ; éléments repris des sprites existants, un nouveau sprite seulement s'il manque (24 crédits au plus). B: pas de maquette, écran construit directement avec les sprites existants (moins cher, tu découvres le look à la capture). C: écran plus dramatique et distinct (fond rouge sombre, cadre fissuré), maquette d'abord, nouveaux sprites probables. Défaut: A.
- Q4 Textes. A: titre et sous-titre dans la voix de la compagnie Kessler-Voss, rangés dans le fichier de textes d'écran comme ceux du run gagné (« CLAIM STRIPPED ») ; première proposition titre « CONTRACT TERMINATED » plus une ligne de sous-titre, vérifiés par le style guide ; titre de repli « RUN LOST ». B: garder « RUN LOST » sans sous-titre. Défaut: A.
- Q5 Récap affiché. A court: niveau atteint (« Level 2 of 4 »), robots détruits sur tout le run (total), Duskite perdue (le solde que tu avais à la mort). B aucun récap, titre et boutons seulement. C récap complet du run comme l'écran de stats (robots par type, dégâts par arme). Défaut: A.
- Q6 Retry. A: comme New Run du menu principal: run remis à zéro (Duskite 0, cartes, armes, niveau et XP, vie pleine), niveau 1, écran de chargement « DROPPING IN / Level 1 » puis fondu. B: redémarrage direct au niveau 1 sans écran de chargement (comme le « Play again » actuel). Défaut: A.
- Q7 Main Menu: comme le bouton du run gagné et Exit de la pause: run remis à zéro, menu principal ouvert directement, sans chargement ni fondu, sans confirmation. Défaut: oui.
- Q8 Divers: libellés « Retry » et « Main Menu » en anglais écrits dans le widget ; focus clavier et manette sur Retry, curseur souris visible ; HUD masqué derrière l'écran ; l'ancien texte GAME OVER et l'ancien panneau RUN LOST / Play again ne s'affichent plus à la mort ; un widget à part (WBP séparé, Canvas et ancres, que tu pourras retoucher à la main). Défaut: oui.
- Q9 L'autre défaite, la navette manquée. A: même écran, avec son propre titre et sous-titre. B: on ne touche pas à la navette manquée (elle garde l'ancien panneau). Défaut: A.
- Q10 Vérification niveau standard: captures -game de la séquence d'apparition et de l'écran final aux 4 résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080), plus Retry et Main Menu essayés en jeu. Tu peux demander 1920 x 1080 seulement. Défaut: les 4 résolutions.

## Subtasks
1. task-484-01-mockup (vault/reference/tasks/task-484-death-screen/task-484-01-mockup.md) — art: Kie.ai mockup of the death screen in the level-end and pause style, rounds until the owner says "je valide".
2. task-484-02-elements (vault/reference/tasks/task-484-death-screen/task-484-02-elements.md) — art: Map every element of the validated mockup to an existing sprite; make a new sprite only where none matches.
3. task-484-03-death-flow (vault/reference/tasks/task-484-death-screen/task-484-03-death-flow.md) — cpp: Death sequence, run-lost screen trigger for death and missed shuttle, recap values, Retry and Main Menu actions, screen texts.
4. task-484-04-death-widget (vault/reference/tasks/task-484-death-screen/task-484-04-death-widget.md) — bp: New death-screen widget from the validated mockup, with the appearance animation and the two buttons wired to C++.
5. task-484-05-hud-loss-path (vault/reference/tasks/task-484-death-screen/task-484-05-hud-loss-path.md) — bp: HUD side of a loss: HUD hidden, old GAME OVER text and RUN LOST / Play again panel no longer shown; run-won and level-success end screens unchanged.
6. task-484-06-capture (vault/reference/tasks/task-484-death-screen/task-484-06-capture.md) — capture: -game capture of the death sequence, the appearance and the final screen at the four resolutions, plus Retry and Main Menu.
7. task-484-07-doc (vault/reference/tasks/task-484-death-screen/task-484-07-doc.md) — doc: Doc nodes of the new widget, the changed HUD widgets and the run-flow system updated.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-484-01-mockup]] — step 1, art.
- [[task-484-02-elements]] — step 2, art.
- [[task-484-03-death-flow]] — step 3, cpp.
- [[task-484-04-death-widget]] — step 4, bp.
- [[task-484-05-hud-loss-path]] — step 5, bp.
- [[task-484-06-capture]] — step 6, capture.
- [[task-484-07-doc]] — step 7, doc.
