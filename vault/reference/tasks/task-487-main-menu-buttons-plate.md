---
id: task-487-main-menu-buttons-plate
description: "Global task: rework the main menu New Run, Options and Exit buttons so they fit the menu look instead of bare floating text. Open to run or review it."
parent: tasks-to-do-index
kind: task
status: todo
---
# Main menu buttons that fit the screen (plate buttons)

- request: Owner, verbatim (French), 2026-11-06: "Il faudrait retravailler les boutons du main menu pour que ça s'incorpore mieux et aussi le menu de chargement beaucoup trop basique." Two separate owner points, two globals: task-487 (main menu buttons) and task-488 (loading screen). This global covers "retravailler les boutons du main menu pour que ça s'incorpore mieux". Options and defaults are Q1 to Q4; his answers are appended here at confirmation. Owner answer 2026-11-07, verbatim: "Menu q1- montre moi une image du b je suis curieux. Le reste par défaut": Q2, Q3, Q4 accepted at their defaults; Q1 still open, waiting for the option B preview image of [[task-489-main-menu-b-preview]]. If he then picks B, Q3 (orange outline focus) and the gdd edits of the defaults file must be revised by the Planner before confirmation. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Prends la solution a" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: The three main menu buttons look like part of the game UI (cream rusty plates as on the other screens, Q1) instead of bare white text, with the plate hover and a visible keyboard focus; behaviour, title, background and gradient unchanged.
- acceptance: A1 New Run, Options and Exit are cream rusty plates of one size, dark brown labels with the exact words "New Run", "Options", "Exit", readable, stacked under the unchanged "ROGUE RIGS" title in the left column; no bare text button and no grey engine button look remains. A2 New Run and Exit hovered by the mouse brighten and grow as the plate hover of ui-button-hover and return to rest when the mouse leaves; Options is a greyed plate with no hover and does nothing. A3 At start the keyboard focus is on New Run and shows as a thin orange outline around its plate; Enter starts a run. A4 Clicks unchanged: New Run starts a run behind the loading screen, Exit quits. A5 Background image, gradient and title unchanged; at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing overlaps or is clipped, at rest or hovered. A6 Build clean; widget compiled with warnings as errors.
- gdd_check: Read: main-menu, ui-button-hover, umg-layout, verification-levels; tech doc wbp-main-menu read because Q1 depends on what the buttons are now (bare Roboto Bold 40 white text turning orange on focus, in a VerticalBox, grow 1.05 on hover since task-485). Conflicts: main-menu Layout and ui-button-hover define the main menu buttons as orange text buttons; the request changes their look, so both nodes are edited with the defaults (edits in the spec). Rules versus rendering: behaviour (New Run, Options disabled, Exit, focus on New Run, Enter) unchanged. Open values: plate look, focus look, scope (Q1 to Q3). umg-layout applies (Canvas and anchors; the current MenuColumn is a VerticalBox, the Executor decides). Milestone: M2 polish of existing screens. Verification standard: one widget layout change, never light; four resolutions stay the default (Q4).
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Prends la solution a")
- milestone: M2
- next_step: task-487-01-menu-plates
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-07 (owner "Prends la solution a"): every default below is accepted; values provisional, owner 2026-11-07.
- Q1 Aspect des boutons du menu principal. A: la plaque crème rouillée des autres écrans (Retry/Main Menu de la défaite, Resume/Exit de la pause, Continue du shop), texte brun foncé, les trois de même taille sous le titre. B: une plaque métal sombre semi-transparente, texte blanc, liseré orange. C: garder le texte seul mais poser un panneau riveté sombre derrière toute la colonne (titre + boutons). Défaut: A.
- Q2 Options reste une plaque grisée qui ne fait rien ; titre « ROGUE RIGS », image de fond et dégradé de gauche inchangés. Défaut: oui.
- Q3 Focus clavier (New Run au démarrage, Entrée lance la partie) : avec une plaque, le texte orange ne se lit plus bien ; le focus se voit par un fin contour orange autour de la plaque, et le survol souris donne l'effet plaque (plus clair, x1,05). Défaut: oui.
- Q4 Vérification niveau standard : capture -game du menu (au repos, New Run survolé, Options survolé sans effet) aux 4 résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080), comparée à la capture d'avant (task-485). Tu peux demander 1920 x 1080 seulement, ou valider toi-même (« je valide »). Défaut: les 4 résolutions.

## Subtasks
1. task-487-01-menu-plates (vault/reference/tasks/task-487-main-menu-buttons-plate/task-487-01-menu-plates.md) — bp: WBP_MainMenu New Run, Options and Exit become plate buttons as main-menu Layout says, with the plate hover and the focus outline.
2. task-487-02-capture (vault/reference/tasks/task-487-main-menu-buttons-plate/task-487-02-capture.md) — capture: -game capture of the main menu at rest and hovered at the four resolutions, compared with the task-485 capture.
3. task-487-03-doc (vault/reference/tasks/task-487-main-menu-buttons-plate/task-487-03-doc.md) — doc: Doc nodes of the main menu widget and of the hover mechanism updated.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-487-01-menu-plates]] — step 1, bp.
- [[task-487-02-capture]] — step 2, capture.
- [[task-487-03-doc]] — step 3, doc.
