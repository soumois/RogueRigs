---
id: task-551-options-graphics-divider-bolts
description: "Global task: Options Graphics divider bolts float inside the panel; use the shared bolted bar, bolts on the frame borders as on pause."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Options Graphics divider: bolts on the frame borders like the pause screen

- request: "L'écran option graphic a toujours un soucis avec les boulons ( les deux embouts de la barre du milieu sur l'image 1). Le soucis a été reglé sur tout les écrans sauf celui ( voir exemple avec l'écran pause sur l'image 2 )" Image 1 (owner screenshot, 2000 px wide): Options, Graphics tab; the vertical bar between the Display and Quality columns ends in two bolt caps that float inside the panel (about y 375 and y 885), below the top border and above the bottom border of the frame. Image 2: pause screen; its divider bolts sit on the top and bottom frame borders (about y 235 and y 945) and the bar runs between them. Relay note: no captures or tests beyond what the Executor needs to verify the result in game. Owner answer 2026-11-14 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q1, Q2 accepted.
- goal: The Options Graphics tab column divider looks and sits like the pause screen divider: the shared bolted bar, its top bolt centred on the top frame border and its bottom bolt on the bottom frame border, the bar running the full inner height between them.
- acceptance: A1 Options, Graphics tab, in game: the divider between Display and Quality has its top bolt on the top frame border and its bottom bolt on the bottom frame border, no bolt floating inside the panel, as on the pause screen; A2 the divider uses the shared bolted bar piece of the pause screen (wbp-ui-bolted-bar), not a separate texture placement; A3 the Display and Quality columns, their rows and controls keep their place and stay readable, nothing overlaps the bar; A4 Sound and Gameplay tabs, the frame, tabs, title and buttons are unchanged; A5 the pause screen divider is unchanged; A6 checked at the resolutions decided in Q2.
- gdd_check: Read options-screen, ui-shared-pieces, verification-levels, the two owner images. Conflict: ui-shared-pieces (task-549) records "the options graphics divider keeps T_LevelEnd_DividerV (a swap would be visible)"; the owner now asks for the pause look, so that line is reversed (Q1) and the inventory updated by the doc subtask. No open value needed. In milestone (UI polish of existing screens). Level standard: a widget layout change is never light (verification-levels). Edit: one Layout line in options-screen. No code read; no diagnosis written.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-14, "Tout par défaut")
- milestone: M2
- next_step: task-551-02-capture
- started: 2026-11-14T08:38:10Z
- waiting_for: none
- executor_tokens: input 66, cache write 92817, cache read 2149661, output 10450
- executor_usage: runs 1, turns 33, context start 16k peak 93k, resumes 0, 8 min, units 0.38 M, USD 1.10
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: open

## Questions
Decided 2026-11-14 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-14.
- Q1 Le séparateur de l'onglet Graphics devient la même barre boulonnée partagée que l'écran pause (boulons sur les bords haut et bas du cadre, barre sur toute la hauteur), à la place de la texture T_LevelEnd_DividerV que task-549 avait gardée exprès. Default: oui, même barre que la pause.
- Q2 Vérification en jeu : les quatre résolutions de umg-layout (règle par défaut pour un changement de mise en page), ou seulement 1920x1080 pour limiter les captures. Default: les quatre résolutions.

## Subtasks
1. task-551-01-bp-divider (vault/reference/tasks/task-551-options-graphics-divider-bolts/task-551-01-bp-divider.md) — bp: Options widget, Graphics tab: replace the column divider by the shared bolted bar, bolts on the frame borders as on the pause screen.
2. task-551-02-capture (vault/reference/tasks/task-551-options-graphics-divider-bolts/task-551-02-capture.md) — capture: In-game check of the Options Graphics tab against the pause screen, at the resolutions decided in Q2.
3. task-551-03-doc (vault/reference/tasks/task-551-options-graphics-divider-bolts/task-551-03-doc.md) — doc: Update the inventory and Blueprint docs: bolted bar now on pause, stats and options Graphics.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-551-01-bp-divider]] — step 1, bp.
- [[task-551-02-capture]] — step 2, capture.
- [[task-551-03-doc]] — step 3, doc.
