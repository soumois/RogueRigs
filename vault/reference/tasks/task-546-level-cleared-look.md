---
id: task-546-level-cleared-look
description: "Global task: stats screen to owner image 6: vertical divider from a bolt junction to the bottom bar, shared unstretched bolts."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Level cleared screen: dividers and bolts as owner image 6

- request: "* L'écran de fin de niveau ( image 5 ), je voudrais qu'elle ressemble à l'image 6" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q12 accepted.
- goal: The level cleared (stats) screen matches owner image 6, using the bolt and bar pieces of the pause screen task. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 The vertical divider starts on a bolt junction in the middle of the horizontal divider and ends on the bottom frame bar with a bolt on it. A2 Horizontal divider end bolts on the frame sides, all bolts round and unstretched at the four resolutions, the shared bolt image. A3 Frame, title, subtitle, Total Duskite band, columns and button proportions as image 6. A4 Texts, values and behaviour unchanged; the level 4 variant takes the same layout.
- gdd_check: Read: intermission (stats screen look from task-381 mockup). No conflict: image 6 refines the same look; edit to intermission at confirmation. Depends on the pause screen task for the shared bolt. No open value (Q12). Level standard: layout change.
- verification_level: standard
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: task-546-01-stats-layout
- started: 2026-11-14T03:52:08Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q12 Écran de fin de niveau : défaut = mêmes boulons et barres que l'écran pause (image à part), séparateur vertical qui part d'une jonction boulonnée au milieu du séparateur horizontal et finit sur la barre du bas ; le reste ajusté sur l'image 6 ; la variante du niveau 4 suit.

## Subtasks
1. task-546-01-stats-layout (vault/reference/tasks/task-546-level-cleared-look/task-546-01-stats-layout.md) — bp: Adjust the stats screen frame dividers and bolts to image 6 with the shared pieces
2. task-546-02-capture (vault/reference/tasks/task-546-level-cleared-look/task-546-02-capture.md) — capture: Stats screen captures against image 6 at four resolutions
3. task-546-03-doc (vault/reference/tasks/task-546-level-cleared-look/task-546-03-doc.md) — doc: Update the stats screen widget doc

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-546-01-stats-layout]] — step 1, bp.
- [[task-546-02-capture]] — step 2, capture.
- [[task-546-03-doc]] — step 3, doc.
