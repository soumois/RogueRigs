---
id: task-549-ui-shared-patterns
description: "Global task: Audit of the UI widgets and UI build scripts after this batch; repeated patterns factored into shared pieces with no visible change, documented."
parent: tasks-done-index
kind: task
status: done
---
# UI: generalise the patterns and scripts used again and again

- request: "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser." [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q15 accepted.
- goal: Patterns repeated across the game screens (frames, title bars, plate buttons, bolts and dividers, entrance, layout scripts) live in shared pieces, every screen looking and behaving as before. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 An inventory of repeated UI patterns and UI build scripts (screen, pattern, where) is written in a tech node. A2 Each pattern used on 3 or more screens, or a script used in 3 or more tasks, is factored into one shared piece or tool; the rest is listed with the reason it is kept. A3 No visible change: before/after captures of main menu, pause, stats, shop, death, options and run-won screens at 1920 x 1080 match. A4 Existing tests pass; build clean. A5 No new feature or look.
- gdd_check: Read: ui-button-hover (sys-button-hover already shared), death-screen, pause-menu, intermission, run-shop, main-menu. No conflict, no open value. Placed last so it sees the shared bolt, bar and entrance pieces of this batch. Scope set by Q15. Level standard: refactor across systems.
- verification_level: standard
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: none
- started: 2026-11-14T04:37:35Z
- waiting_for: none
- executor_tokens: input 132, cache write 162199, cache read 7502798, output 42288
- executor_usage: runs 1, turns 65, context start 17k peak 175k, resumes 1, 22 min, units 1.16 M, USD 3.16
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. 149401e4: new node ui-shared-pieces under tech-index (A1: shared pieces and tools, inventory table screen / pattern / where, kept-per-screen list with reasons); tool-capture gains ui-screens.js; sys-button-hover and sys-screen-entrance point to UI/RRUIDebugCommands.cpp. Lint clean (warnings only, none on these nodes).

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q15 Généralisation : défaut = dernière tâche du lot ; inventaire des motifs et scripts UI répétés ; on factorise ce qui sert sur 3 écrans ou plus (ou un script repris dans 3 tâches ou plus), sans aucun changement visible (captures avant/après à 1920 x 1080 de chaque écran) ; le reste est listé avec la raison.

## Subtasks
1. task-549-01-factor (vault/reference/tasks/task-549-ui-shared-patterns/task-549-01-factor.md) — cpp: Inventory and factor the repeated UI patterns and build scripts, screens unchanged
2. task-549-02-capture (vault/reference/tasks/task-549-ui-shared-patterns/task-549-02-capture.md) — capture: Before/after captures of every game screen at 1920 x 1080
3. task-549-03-doc (vault/reference/tasks/task-549-ui-shared-patterns/task-549-03-doc.md) — doc: Write the UI shared pieces node with the inventory

## Relations
- [[tasks-done-index]] — parent index.
- [[task-549-01-factor]] — step 1, cpp.
- [[task-549-02-capture]] — step 2, capture.
- [[task-549-03-doc]] — step 3, doc.
