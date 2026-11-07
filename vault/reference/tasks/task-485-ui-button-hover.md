---
id: task-485-ui-button-hover
description: "Global task: Visible mouse hover effect on all clickable buttons of the main menu, pause, run-lost, stats, shop, level-up and run-won screens. Open to run or review it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Hover effect on every button of every screen

- request: Owner, verbatim (French): "il faudrait des effets de hover lorsqu'on passe au dessus des boutons dans toutes les interfaces". The options and defaults are Q1 to Q7 below; his answers are appended here at confirmation. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7 accepted.
- goal: Every clickable button of every game screen reacts visibly when the mouse passes over it and returns to normal when it leaves, with one consistent effect defined in ui-button-hover; layouts, click behaviour and the existing level-up card hover stay as they are.
- acceptance: A1 On each button listed in ui-button-hover Scope (main menu New Run and Exit; pause Resume and Exit; run-lost Retry and Main Menu; stats Continue; shop Reroll, Continue and the five Buy buttons; level-up Reroll; run-won end screen button), a frame with the mouse over it differs visibly from a frame without, as ui-button-hover Look says (plates brighter and grown to 1.05, main menu text orange and grown), and it returns to the rest look when the mouse leaves. A2 The effect eases in and out over about 0.1 s and works on paused screens (pause, level-up, stats, shop, run-lost). A3 Disabled buttons (Options, greyed Reroll and Buy) show no hover effect; keyboard or gamepad focus alone shows no hover look; pressed look unchanged. A4 No layout change: at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing moves, overlaps or is clipped at rest or when a button is hovered. A5 Clicks still do what they did (start run, resume, retry, continue, reroll, buy, main menu, exit); level-up cards keep their own hover; shop purchase effects and the run-lost appearance still play. A6 Build clean; compiled with warnings as errors.
- gdd_check: Read: gdd-index, art-audio-index, color-code, audio-direction, main-menu, death-screen; tech docs read only to inventory buttons and their current hover (no code or asset read): blueprints-ui-index, wbp-main-menu (text buttons already orange on hover and focus), wbp-pause-menu, wbp-run-lost, wbp-intermission, wbp-shop-screen (plates with hover tint 1.3, never seen on screen in any capture), wbp-shop-card (Buy hover tint 1.35, 'may be capped'), wbp-upgrade-choice (cards already have an eased hover, Reroll button plain), wbp-alert-objectives (RestartButton of the run-won end screen), umg-layout, verification-levels. Inventory: 8 widgets with buttons (MainMenu, PauseMenu, RunLost, Intermission, ShopScreen, ShopCard, UpgradeChoice, AlertObjectives); WBP_Loading, HUD panels and the others have no clickable button. No conflict with the GDD; the only existing hover rule is main-menu orange, kept. Open values (none existed): look, scale, colour, timing, scope of cards, disabled, focus, sound, capture size: proposed as Q1 to Q7, written as defaults in the new gdd node ui-button-hover (status open until answered). Uncommitted owner edits: git status at planning shows no modified .uasset (the two WBP of the shop handover are no longer pending); umg-layout still applies if any appear at execution. Milestone: polish of existing M2 screens, no new system. Verification standard: new behaviour on 8 widgets; not light because several assets change and the four-resolution check stays (Q7). Plan: one global (one owner point), one bp subtask per main asset, a representative slice (pause menu) checked first because whether the current hover tints show at all is uncertain.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Tout par défaut")
- milestone: M2
- next_step: task-485-02-slice-capture
- started: 2026-11-07T02:19:47Z
- waiting_for: none
- executor_tokens: input 86, cache write 114217, cache read 3494484, output 37329
- executor_usage: runs 1, turns 43, context start 18k peak 126k, resumes 0, 11 min, units 0.68 M
- planning_tokens: input 34, cache write 111833, cache read 957049, output 19112
- planning_usage: a4e7f869 "Plan button hover effects" 1/1 (tasks 485), units 0.33 M
- result: open

## Questions
Decided 2026-11-07 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-07.
- Q1 Effet sur les boutons à plaque crème (Resume, Exit, Retry, Main Menu, Continue, Reroll, Buy, bouton de fin de run). A: la plaque s'éclaircit nettement (environ +25 % de luminosité, même teinte) et le bouton grossit à 1,05, transition 0,1 s à l'entrée et à la sortie. B: éclaircissement seul, sans grossissement. C: grossissement seul. D: A plus un léger contour lumineux autour de la plaque. Défaut: A.
- Q2 Couleur du survol des plaques. A: même teinte crème, juste plus claire ; le texte garde sa couleur brun foncé. B: reflet orange (l'orange du menu principal) sur la plaque. C: le texte passe en orange. Défaut: A.
- Q3 Menu principal (New Run, Exit: texte seul, déjà orange au survol et au focus). A: on garde l'orange et on ajoute le même grossissement 1,05 en 0,1 s. B: on n'y touche pas. Défaut: A.
- Q4 Cartes. Les cartes de level-up ont déjà leur survol (grossissement, lueur, reflet): défaut, on n'y touche pas, seul le bouton Reroll de cet écran reçoit l'effet. Les cartes du shop: défaut, seul le bouton Buy de chaque carte reçoit l'effet, pas la carte entière. Défaut: oui aux deux.
- Q5 Boutons désactivés et focus. Un bouton grisé (Options, Reroll ou Buy quand il manque de la Duskite) ne réagit pas au survol ; le focus clavier ou manette seul ne montre pas l'effet de survol (le menu principal garde son orange au focus) ; l'aspect au clic ne change pas. Défaut: oui.
- Q6 Son au survol. A: aucun son (aucun son d'interface n'existe, la palette sonore est prévue en M3). B: un petit son de survol libre de droits, trouvé et noté par l'agent son (une sous-tâche en plus). Défaut: A.
- Q7 Vérification niveau standard: d'abord l'écran de pause seul (tranche) capturé en 1920 x 1080 survolé et non survolé, ensuite tous les écrans capturés survolés et non survolés aux 4 résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080). Tu peux demander 1920 x 1080 seulement pour la capture finale, ou valider toi-même à l'écran (« je valide ») à la place d'une capture. Défaut: les 4 résolutions.

## Subtasks
1. task-485-01-pause-slice (vault/reference/tasks/task-485-ui-button-hover/task-485-01-pause-slice.md) — bp: Representative slice: hover effect of ui-button-hover on WBP_PauseMenu Resume and Exit, built so the other screens can reuse the same mechanism.
2. task-485-02-slice-capture (vault/reference/tasks/task-485-ui-button-hover/task-485-02-slice-capture.md) — capture: Check the pause menu slice: hovered and not hovered frames at 1920 x 1080, sent to the owner.
3. task-485-03-run-lost (vault/reference/tasks/task-485-ui-button-hover/task-485-03-run-lost.md) — bp: Same hover on WBP_RunLost Retry and Main Menu.
4. task-485-04-intermission (vault/reference/tasks/task-485-ui-button-hover/task-485-04-intermission.md) — bp: Same hover on WBP_Intermission Continue.
5. task-485-05-shop-screen (vault/reference/tasks/task-485-ui-button-hover/task-485-05-shop-screen.md) — bp: Same hover on WBP_ShopScreen Reroll and Continue.
6. task-485-06-shop-card (vault/reference/tasks/task-485-ui-button-hover/task-485-06-shop-card.md) — bp: Same hover on the Buy button of WBP_ShopCard, the card body unchanged.
7. task-485-07-upgrade-reroll (vault/reference/tasks/task-485-ui-button-hover/task-485-07-upgrade-reroll.md) — bp: Same hover on the WBP_UpgradeChoice Reroll button; the cards keep their own hover.
8. task-485-08-end-screen (vault/reference/tasks/task-485-ui-button-hover/task-485-08-end-screen.md) — bp: Same hover on the WBP_AlertObjectives end screen button (run-won Main Menu).
9. task-485-09-main-menu (vault/reference/tasks/task-485-ui-button-hover/task-485-09-main-menu.md) — bp: Main menu New Run and Exit: keep the orange and add the growth of ui-button-hover.
10. task-485-10-capture (vault/reference/tasks/task-485-ui-button-hover/task-485-10-capture.md) — capture: -game capture of every screen with its buttons at rest and hovered, at the four resolutions.
11. task-485-11-doc (vault/reference/tasks/task-485-ui-button-hover/task-485-11-doc.md) — doc: Doc nodes of the eight changed widgets and of any shared mechanism updated.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-485-01-pause-slice]] — step 1, bp.
- [[task-485-02-slice-capture]] — step 2, capture.
- [[task-485-03-run-lost]] — step 3, bp.
- [[task-485-04-intermission]] — step 4, bp.
- [[task-485-05-shop-screen]] — step 5, bp.
- [[task-485-06-shop-card]] — step 6, bp.
- [[task-485-07-upgrade-reroll]] — step 7, bp.
- [[task-485-08-end-screen]] — step 8, bp.
- [[task-485-09-main-menu]] — step 9, bp.
- [[task-485-10-capture]] — step 10, capture.
- [[task-485-11-doc]] — step 11, doc.
