---
id: task-543-wall-debris-less-white
description: "Global task: Rock destruction debris take the rock colours (lilac and violets) instead of near white, no glow."
parent: tasks-done-index
kind: task
status: done
---
# Wall destruction particles: rock colours instead of near white

- request: "* Image 3 : Les particules de destructions de murs sont fort blanc" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q3 accepted.
- goal: The debris thrown when a wall block breaks read as rock pieces in the rock colours, not as white flashes. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 Breaking rock throws debris in lilac (top) and dark or mid violet (sides) tones; none near white, no bloom halo. A2 Amount, size, speed and duration unchanged. A3 Ore rock and other effects unchanged. A4 One capture of a block breaking, before and after.
- gdd_check: Read: color-code (breakable rock row). No colour rule for debris yet: decided by Q3, row added to color-code at confirmation. No open value. Level light: colour setting on an existing effect.
- verification_level: light
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: none
- started: 2026-11-13T23:32:36Z
- waiting_for: none
- executor_tokens: input 242, cache write 211743, cache read 16189851, output 62562
- executor_usage: runs 1, turns 119, context start 16k peak 224k, resumes 2, 43 min, units 2.20 M, USD 5.55
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. A1 rock debris (drone break and explosion) in lilac, mid and dark violet, none near white, no bloom (RRBlockColors::RockDebrisTones, provisional; NS_DigDebris three User colours with a per-particle HLSL). A2 count, size, speed, lifetime unchanged. A3 ore, heal ore, barrel debris and NS_DigImpact unchanged. A4 before/after: Saved/Captures/task543-after3/sheet_dig.png and sheet_bar.png. Tests RogueRigs.Dig.DigDebris + RogueRigs.Explosions.WallDebris pass. Note: after a barrel flash the violet rock debris has low contrast on the dark floor. Budget: over the light budget (about 120 turns, 2.2 M units, under 4x): Niagara HLSL edit not compiled by the MCP tool until a further stack edit, first tones too pale, three capture rounds.

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q3 Débris des murs : défaut = couleurs de la roche (fragments lilas comme le dessus, violets foncés et moyens comme les côtés), plus de blanc presque pur ni de halo ; quantité, taille et durée inchangées.

## Subtasks
1. task-543-01-debris-colour (vault/reference/tasks/task-543-wall-debris-less-white/task-543-01-debris-colour.md) — bp: Retint the rock destruction debris to the rock colours, no glow
2. task-543-02-capture (vault/reference/tasks/task-543-wall-debris-less-white/task-543-02-capture.md) — capture: Before/after capture of a wall block breaking
3. task-543-03-doc (vault/reference/tasks/task-543-wall-debris-less-white/task-543-03-doc.md) — doc: Update the debris effect doc node

## Relations
- [[tasks-done-index]] — parent index.
- [[task-543-01-debris-colour]] — step 1, bp.
- [[task-543-02-capture]] — step 2, capture.
- [[task-543-03-doc]] — step 3, doc.
