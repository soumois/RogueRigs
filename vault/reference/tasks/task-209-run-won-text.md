---
id: task-209-run-won-text
description: "Global task: Run won text at level 4."
parent: tasks-done-index
kind: task
status: done
---
# Run won text at level 4

- request: "Oui, crée une tâche pour corriger les 3 défauts : [...] texte de victoire au niveau 4." (owner, 2026-10-17)
- goal: the level 4 end screen shows a run-won text written by the text agent instead of EXTRACTION SUCCESSFUL.
- acceptance: the run-won text passes the style-guide review (or the owner keeps a deviation) and the owner sees it before integration; WBP_AlertObjectives shows it when the run is won at level 4 and keeps EXTRACTION SUCCESSFUL nowhere else changed; capture sent to the owner; lint clean; committed.
- gdd_check: [[run-structure]] (win at level 4), [[extraction]], [[dialogue-tone]] T7 (English on screen). Owner 2026-10-17: the text agent proposes, style-guide reviews, the owner sees it before integration. Owner chose CLAIM STRIPPED with its subtitle and option A: validator and loader accept a screen texts file, widgets read it through the text content subsystem.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: none
- result: Done. Rendered -game capture after a full 4-level run (levels 1-3 through the intermission, level 4 real extraction): end screen CLAIM STRIPPED / Gone before the paperwork landed. / Ore secured: 120 / Play again; missed-shuttle loss still RUN LOST with no subtitle; sent to the owner. Observed: a Kessler-Voss subtitle draws over the end screens (207 only silences intermission and loading), dimmed stale HUD behind the end screen.

## Subtasks
1. task-209-01-run-won-copy — text
2. task-209-02-run-won-review — style-guide
3. task-209-03-screen-texts-code — cpp
4. task-209-04-run-won-bp — bp
5. task-209-05-capture — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-206-level-to-level]] — the transition this fixes.
- [[task-209-01-run-won-copy]] — step 1, text.
- [[task-209-02-run-won-review]] — step 2, style-guide.
- [[task-209-03-screen-texts-code]] — step 3, cpp.
- [[task-209-04-run-won-bp]] — step 4, bp.
- [[task-209-05-capture]] — step 5, cpp.
