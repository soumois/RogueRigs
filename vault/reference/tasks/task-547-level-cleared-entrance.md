---
id: task-547-level-cleared-entrance
description: "Global task: the death screen entrance becomes one shared effect, used by the level cleared screen; death screen unchanged."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Level cleared screen appears with the death screen entrance effect

- request: "* L'écran de fin de niveau devrait apparaitre comme celui de mort ( l'effet d'entrée, il marche très bien ! )" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q13 accepted.
- goal: The level cleared screen appears with the same entrance as the death screen, from one shared effect used by both. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 On extraction the stats screen plays the death screen entrance: backdrop 0 to 75 % with the level desaturated (0.6 s), title drop 1.4 to 1 with fade and short shake (0.25 s), frame slide up with fade (0.3 s), button last (0.2 s); about 1.4 s, real time. A2 A click or key during it skips to the end state; the NEXT LEVEL button is not triggered by the skip. A3 The level 4 variant does the same. A4 One shared effect drives both screens; the death screen entrance unchanged. A5 Existing tests pass.
- gdd_check: Read: death-screen (Appearance Q2 timings), intermission. No conflict; edit to intermission at confirmation. No open value (Q13). Shared effect answers the owner generalisation point for this pattern. Level standard: new behaviour on a screen.
- verification_level: standard
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: task-547-03-capture
- started: 2026-11-14T03:59:58Z
- waiting_for: none
- executor_tokens: input 174, cache write 169924, cache read 9284209, output 58111
- executor_usage: runs 1, turns 85, context start 16k peak 183k, resumes 2, 18 min, units 1.43 M, USD 3.87
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: open

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q13 Entrée de l'écran de fin de niveau : défaut = exactement celle de l'écran de mort (fond qui s'assombrit avec le niveau désaturé, titre qui tombe avec petite secousse, cadre qui glisse, bouton en dernier, clic ou touche pour passer), aussi pour la variante du niveau 4 ; un seul effet partagé par les deux écrans ; pas appliqué à la pause ni à la boutique.

## Subtasks
1. task-547-01-shared-entrance (vault/reference/tasks/task-547-level-cleared-entrance/task-547-01-shared-entrance.md) — cpp: Extract the death screen entrance into one shared effect, death screen unchanged
2. task-547-02-stats-entrance (vault/reference/tasks/task-547-level-cleared-entrance/task-547-02-stats-entrance.md) — bp: Wire the shared entrance to the stats screen and its level 4 variant
3. task-547-03-capture (vault/reference/tasks/task-547-level-cleared-entrance/task-547-03-capture.md) — capture: Entrance sequence frames of the stats screen and the death screen
4. task-547-04-doc (vault/reference/tasks/task-547-level-cleared-entrance/task-547-04-doc.md) — doc: Document the shared entrance effect

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-547-01-shared-entrance]] — step 1, cpp.
- [[task-547-02-stats-entrance]] — step 2, bp.
- [[task-547-03-capture]] — step 3, capture.
- [[task-547-04-doc]] — step 4, doc.
