---
id: task-384-level-end-subtitle-lines
description: "Global task: the two thin lines beside the stats screen subtitle no longer cross a long subtitle (level 4 run-won text)."
parent: tasks-done-index
kind: task
status: cancelled
---
# Level-end stats screen: subtitle lines fit any subtitle

- request: Open defect seen by task-381-03 (capture `Saved/LevelEndScreen/task-381/L4_1920x1080_v2.png`): on level 4 the subtitle "Last rock stripped. Here is the damage report." is longer than the 600 px gap between SubtitleLineL and SubtitleLineR (fixed at x -300 and +300) and the two lines cross it; level 1 is clear. Raised by the main session with the owner answer of task-383 (2026-10-30), not requested by the owner: Q5 asks whether to fix it and how; a "non" means this task is cancelled. Separate owner point from task-383 (one point, one global). Not in scope: subtitle text, other widgets. Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q5, Q6 accepted.
- goal: On every stats screen (levels 1 to 4) the two thin lines beside the subtitle stay outside the subtitle text, per Q5.
- acceptance: WBP_Intermission: SubtitleLineL and SubtitleLineR placed per Q5 (default: by OnRecapShown after the subtitle is set, from the subtitle desired width, 20 slate units from each end of the text, length 140 kept); nothing else changed; compiled with warnings as errors, saved; capture C1 to C3 seen (Q6); wbp-intermission doc updated; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[intermission]] (look sentence: "subtitle" only, the lines are a widget detail; no gdd change), [[wbp-intermission]] (SubtitleLineL (0.5,0.21) align (1,0.5) (-300,0,140,2), SubtitleLineR align (0,0.5) (300,0,140,2); Subtitle (0.5,0.21) auto size; verified: level 4 crossing defect), [[art-prompts-level-end-screen-mockup]] (the thin lines beside the subtitle are plain UMG lines, task-380 Q2), [[umg-layout]] (no Row to centre elements on a text line: so the default places the lines from the text width, not a HorizontalBox). Conflicts: none. Open values: none. Depends on task-383 (same asset, one agent at a time). Scope M2.
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: none
- started: open
- waiting_for: none
- result: Owner 2026-10-30: "Non, annule"; owner validated the screen as is, subtitle lines not fixed.

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): every default below is accepted; values provisional, owner 2026-10-30.
- Q5 Défaut vu par la capture de task-381 : au niveau 4, le sous-titre long (« Last rock stripped. Here is the damage report. ») traverse les deux filets fins, fixés à ±300 px du centre. On le corrige (tâche séparée task-384, après task-383) ? Default: oui, les filets suivent la longueur du texte : le Blueprint les place à 20 px de chaque bout du sous-titre, quel que soit le texte (niveaux 1 à 4), longueur inchangée. Autres choix : filets fixes plus loin (±420 px, plus d'écart au niveau 1) ; un seul filet sous le sous-titre ; ou « non » = pas de tâche.
- Q6 Vérification : capture -game niveau 1 et niveau 4 en 1920 x 1080 et 1280 x 720. Default: oui.

## Subtasks
1. task-384-01-subtitle-lines-widget (vault/reference/tasks/task-384-level-end-subtitle-lines/task-384-01-subtitle-lines-widget.md) — bp: WBP_Intermission: SubtitleLineL and SubtitleLineR placed outside the subtitle text whatever its length
2. task-384-02-subtitle-lines-capture (vault/reference/tasks/task-384-level-end-subtitle-lines/task-384-02-subtitle-lines-capture.md) — capture: -game captures of levels 1 and 4 at 1920 x 1080 and 1280 x 720: lines outside the subtitle
3. task-384-03-doc-subtitle-lines (vault/reference/tasks/task-384-level-end-subtitle-lines/task-384-03-doc-subtitle-lines.md) — doc: update the wbp-intermission doc: subtitle line placement, graph, capture result

## Relations
- [[tasks-done-index]] — parent index.
- [[task-384-01-subtitle-lines-widget]] — step 1, bp.
- [[task-384-02-subtitle-lines-capture]] — step 2, capture.
- [[task-384-03-doc-subtitle-lines]] — step 3, doc.
- [[wbp-intermission]] — the widget changed here.
- [[task-381-level-end-screen-rebuild]] — the rebuild whose capture saw the defect.
- [[task-383-level-end-total-ore]] — previous task on the same widget.
