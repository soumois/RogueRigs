---
id: task-541-hud-turret-count-removed
description: "Global task: The turret count text above the health bar goes; the launcher charges stay shown in its weapon slot."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# HUD: remove the "Turrets: 2 / 2" text above the health bar

- request: "* Image 4 : On voit encore "Turrets 2/2" au dessus de la vie, il faut retirer ça, on a l'information" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target).
- goal: No turret count text above the health bar; the turret launcher charges remain readable in its weapon slot. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 With the turret launcher owned, no "Turrets: N / M" text appears anywhere on the HUD in play. A2 The small grey marker under that text is gone too if it belongs to the same display (Q4). A3 The launcher slot still shows its charges as before. A4 One -game capture at 1920 x 1080 before and after.
- gdd_check: Read: turret (charges = magazine; no HUD text rule). No conflict, no open value. The HUD text is not in the GDD; nothing to edit there. Level light: removal on an existing widget.
- verification_level: light
- family: gros
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-541-01-remove-text
- started: open
- waiting_for: Owner answers to the batch questions Q1 to Q18.
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q4 Texte "Turrets: 2 / 2" : défaut = retiré, avec le petit repère gris juste dessous s'il fait partie du même affichage ; les charges restent visibles sous l'icône du lance-tourelles.

## Subtasks
1. task-541-01-remove-text (vault/reference/tasks/task-541-hud-turret-count-removed/task-541-01-remove-text.md) — bp: Remove the turret count text (and its marker per Q4) from the HUD
2. task-541-02-capture (vault/reference/tasks/task-541-hud-turret-count-removed/task-541-02-capture.md) — capture: Before/after HUD capture with the turret launcher owned at 1920 x 1080
3. task-541-03-doc (vault/reference/tasks/task-541-hud-turret-count-removed/task-541-03-doc.md) — doc: Update the HUD doc node for the removed text

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-541-01-remove-text]] — step 1, bp.
- [[task-541-02-capture]] — step 2, capture.
- [[task-541-03-doc]] — step 3, doc.
