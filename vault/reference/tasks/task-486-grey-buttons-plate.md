---
id: task-486-grey-buttons-plate
description: "Global task: cream plate look for the level-up Reroll and run-won Main Menu buttons, like the other screen buttons. Open to run or review it."
parent: tasks-to-do-index
kind: task
status: todo
---
# Cream plate look for the two grey buttons (level-up Reroll, run-won Main Menu)

- request: Owner, verbatim (French), after reading the task-485 result: "Il faut en effet retoucher les deux boutons gris ( D'ailleurs d'où vient le bouton gris Main Menu ? )". The two grey buttons are the ones task-485 reported: RerollButton of WBP_UpgradeChoice (level-up screen, owner capture Saved/Captures/task485_resA/c_lu.png) and RestartButton of the WBP_AlertObjectives end screen (run-won "EXTRACTION SUCCESSFUL", label "Main Menu", owner capture Saved/Captures/task485_resB/c_won.png). His question about the origin is answered in the plan report and in gdd_check (Origin), it needs no work. The options and defaults are Q1 to Q5 below; his answers are appended here at confirmation. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5 accepted.
- goal: The level-up Reroll button and the run-won Main Menu button look like the other screen buttons (cream rusty plate, readable dark label) instead of plain grey engine buttons, and take the standard plate hover of ui-button-hover at full strength; their behaviour and the rest of both screens stay as they are.
- acceptance: A1 On the level-up screen, Reroll is a cream rusty plate as on the run-lost and pause buttons, its label "Reroll", the ore icon and the cost are all readable on it, and no grey engine button look remains. A2 Disabled Reroll (deal in progress or not enough Duskite) is clearly greyed and shows no hover; enabled, hovered it brightens and grows as ui-button-hover Look says for plates, and returns to rest when the mouse leaves. A3 On the run-won EXTRACTION SUCCESSFUL screen, Main Menu is the same cream plate as the run-lost Main Menu (same size class and label style), with the same plate hover. A4 Nothing else moves on either screen: cards, deal, pick and card hover, title, subtitle and Duskite secured row unchanged; at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing overlaps or is clipped, at rest or hovered. A5 Clicks unchanged: Reroll rerolls and its cost updates, Main Menu returns to the main menu. A6 Build clean; both widgets compiled with warnings as errors.
- gdd_check: Read: ui-button-hover, death-screen (Look: cream rusty buttons), verification-levels; task-485-ui-button-hover (named by the request); tech docs read only because the owner asked where the grey Main Menu comes from and to size the questions (no code or asset read): wbp-alert-objectives, wbp-upgrade-choice, wbp-run-lost. Origin (owner question, answered in the plan report): RestartButton is the button of the old end panel (EndScreen) of WBP_AlertObjectives, labelled "Play again" for both RUN LOST and EXTRACTION SUCCESSFUL; task-390-02 made it read "Main Menu" after a won run; it was never styled; since task-484-05 a loss shows WBP_RunLost, so the grey button now only shows on the run-won screen. No conflict with the GDD: death-screen and ui-button-hover already call for cream rusty buttons; no gdd node specifies the run-won screen look. Open values: plate look, label style, size of both buttons, scope of the run-won screen (Q1 to Q4); defaults written in ui-button-hover by the edit. Reworking the whole run-won screen to match the run-lost screen would be a separate owner point and a separate global (Q4), not planned by default. Milestone: M2 polish of existing screens. Verification standard: two assets change and a visible layout changes, so never light; four resolutions stay the default (Q5, or owner "je valide").
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Tout par défaut")
- milestone: M2
- next_step: task-486-01-reroll-plate
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-07 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-07.
- Q1 Aspect des deux boutons. A: la plaque crème rouillée des autres boutons (Retry/Main Menu de l'écran de défaite, Resume/Exit de la pause, Continue du shop), texte brun foncé comme sur ces plaques. B: une plaque sombre avec texte blanc. Défaut: A.
- Q2 Bouton Reroll du level-up. Il garde son contenu « Reroll » + icône Duskite + coût, en texte brun foncé gras sur la plaque, même taille et même rendu que le bouton Reroll du shop ; grisé quand il est désactivé (distribution des cartes ou Duskite insuffisante), sans survol. Défaut: oui.
- Q3 Bouton Main Menu de l'écran de victoire. Même plaque et même taille que le Main Menu de l'écran de défaite (440 x 108, texte « Main Menu » 30 gras), centré à sa place actuelle sous la ligne « Duskite secured ». Défaut: oui.
- Q4 Écran de victoire. A: on ne change que le bouton, le reste de l'écran tel quel. B: en plus, refaire tout l'écran de victoire dans le style de l'écran de défaite (cadre riveté, barres de titre, récap) : ce serait une tâche séparée avec maquette d'abord, créée seulement si tu la demandes. Défaut: A.
- Q5 Vérification niveau standard : capture -game de l'écran de level-up (Reroll actif au repos, survolé, et grisé) et de l'écran de victoire (Main Menu au repos et survolé) aux 4 résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080), comparée aux captures d'avant. Tu peux demander 1920 x 1080 seulement, ou valider toi-même à l'écran (« je valide ») à la place de la capture. Défaut: les 4 résolutions.

## Subtasks
1. task-486-01-reroll-plate (vault/reference/tasks/task-486-grey-buttons-plate/task-486-01-reroll-plate.md) — bp: Level-up Reroll button of WBP_UpgradeChoice gets the cream plate look of ui-button-hover, with the standard plate hover.
2. task-486-02-end-screen-plate (vault/reference/tasks/task-486-grey-buttons-plate/task-486-02-end-screen-plate.md) — bp: Run-won Main Menu button of the WBP_AlertObjectives end screen gets the same cream plate as the run-lost Main Menu, with the standard plate hover.
3. task-486-03-capture (vault/reference/tasks/task-486-grey-buttons-plate/task-486-03-capture.md) — capture: -game capture of the level-up and run-won screens with the two buttons at rest, hovered and (Reroll) disabled, at the four resolutions, compared with the task-485 captures.
4. task-486-04-doc (vault/reference/tasks/task-486-grey-buttons-plate/task-486-04-doc.md) — doc: Doc nodes of the two changed widgets and of the hover mechanism updated.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-486-01-reroll-plate]] — step 1, bp.
- [[task-486-02-end-screen-plate]] — step 2, bp.
- [[task-486-03-capture]] — step 3, capture.
- [[task-486-04-doc]] — step 4, doc.
