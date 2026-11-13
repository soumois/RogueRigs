---
id: task-542-ground-particle-edges
description: "Global task: The white ground particle sprites of image 2 (boss scene) show their quad edges; the edges must not show, look otherwise kept."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Ground particles: no visible sprite edges

- request: "* Image 2 : Pourquoi voit on les bords des sprites particules au sol ?" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target).
- goal: The ground particles seen in image 2 show no square or straight sprite edge on the floor; their colour, size, timing and count are unchanged. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 In the situation of image 2 (boss arena), no ground particle shows a visible quad edge or rectangle outline at 1920 x 1080. A2 The same effect elsewhere (if reused) shows no edge either. A3 Colour, size, timing and count unchanged. A4 The cause found is written in the result (what made the edge show).
- gdd_check: Read: color-code. A defect report: not diagnosed by the Planner (which effect, why the edges show is for the Executor). No conflict, no open value. Level light: fix on an existing effect, no new look.
- verification_level: light
- family: gros
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-542-01-fix-edges
- started: open
- waiting_for: Owner answers to the batch questions Q1 to Q18.
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q2 Bords des particules au sol : défaut = on corrige pour que les bords ne se voient plus, sans changer couleur, taille, durée ni nombre ; la cause est notée dans le résultat.

## Subtasks
1. task-542-01-fix-edges (vault/reference/tasks/task-542-ground-particle-edges/task-542-01-fix-edges.md) — bp: Find the ground particle effect of image 2 and remove its visible sprite edges
2. task-542-02-capture (vault/reference/tasks/task-542-ground-particle-edges/task-542-02-capture.md) — capture: Before/after capture of the effect on the floor
3. task-542-03-doc (vault/reference/tasks/task-542-ground-particle-edges/task-542-03-doc.md) — doc: Note the fix in the effect doc node

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-542-01-fix-edges]] — step 1, bp.
- [[task-542-02-capture]] — step 2, capture.
- [[task-542-03-doc]] — step 3, doc.
