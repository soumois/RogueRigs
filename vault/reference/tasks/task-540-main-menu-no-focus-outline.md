---
id: task-540-main-menu-no-focus-outline
description: "Global task: Remove the orange keyboard-focus outline drawn around the focused main menu button; focus and Enter unchanged."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Main menu: no orange focus outline around New Run

- request: "Correction : * Image 1 : Pas besoin d'avoir le rectangle orange dans le main menu" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q1, Q16, Q17, Q18 accepted.
- goal: The main menu shows no orange rectangle around any button, while keyboard focus and mouse hover keep working. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 At menu open and after keyboard navigation, no orange outline is drawn around New Run, Options or Exit. A2 Focus still starts on New Run and Enter starts a run. A3 Mouse hover plate effect unchanged. A4 One -game capture at 1920 x 1080 before and after.
- gdd_check: Read: main-menu (Layout: focus outline orange (1, 0.35, 0.1) decided task-487), ui-button-hover (Focus section). Conflict: both nodes describe the outline; the owner removes it: edits applied at confirmation. No open value. In scope. Level light: one setting on an existing widget, no layout change.
- verification_level: light
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: task-540-03-doc
- started: 2026-11-13T22:55:53Z
- waiting_for: none
- executor_tokens: input 128, cache write 119599, cache read 4732147, output 31047
- executor_usage: runs 1, turns 63, context start 16k peak 120k, resumes 1, 11 min, units 0.78 M, USD 2.17
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: open

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Menu principal : défaut = plus aucun contour de focus ; le focus clavier reste sur New Run (Entrée lance la partie), le hover souris est inchangé.
- Q16 Règle de budget : la tâche task.md dit qu'un dépassement n'arrête jamais l'exécuteur ; ta règle est plus récente. Défaut : pour ce lot seulement, au-delà de 4 fois le budget de son niveau (tours ou unités), l'exécuteur s'arrête, note où il en est, met la tâche en `blocked`, et le relais passe à la tâche suivante. La règle générale n'est pas modifiée (une tâche à part si tu la veux permanente). Aucune tâche du lot n'attend ton feu vert : tes images cibles 6, 8 et 10 servent de référence validée.
- Q17 Images : défaut = tes images cibles 6, 8 et 10 gardées ("keep") dans Art/Source/ comme références des écrans ; les autres (1 à 5, 7, 9) restent temporaires dans Saved/Scratch/owner-ui-batch-2026-11-13/.
- Q18 Niveaux de vérification : défaut = léger pour les 4 petites corrections (menu, texte Turrets, bords des particules, débris de murs), standard pour les 6 autres. Un mot (leger, standard, exploratoire) + la tâche les change.

## Subtasks
1. task-540-01-focus-outline-off (vault/reference/tasks/task-540-main-menu-no-focus-outline/task-540-01-focus-outline-off.md) — bp: Remove the orange focus outline of the main menu buttons, focus and Enter kept
2. task-540-02-capture (vault/reference/tasks/task-540-main-menu-no-focus-outline/task-540-02-capture.md) — capture: Before/after capture of the main menu at 1920 x 1080
3. task-540-03-doc (vault/reference/tasks/task-540-main-menu-no-focus-outline/task-540-03-doc.md) — doc: Update the menu widget doc for the removed focus outline

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-540-01-focus-outline-off]] — step 1, bp.
- [[task-540-02-capture]] — step 2, capture.
- [[task-540-03-doc]] — step 3, doc.
