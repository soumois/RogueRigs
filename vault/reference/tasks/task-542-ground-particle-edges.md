---
id: task-542-ground-particle-edges
description: "Global task: The white ground particle sprites of image 2 (boss scene) show their quad edges; the edges must not show, look otherwise kept."
parent: tasks-done-index
kind: task
status: done
---
# Ground particles: no visible sprite edges

- request: "* Image 2 : Pourquoi voit on les bords des sprites particules au sol ?" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q2 accepted.
- goal: The ground particles seen in image 2 show no square or straight sprite edge on the floor; their colour, size, timing and count are unchanged. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 In the situation of image 2 (boss arena), no ground particle shows a visible quad edge or rectangle outline at 1920 x 1080. A2 The same effect elsewhere (if reused) shows no edge either. A3 Colour, size, timing and count unchanged. A4 The cause found is written in the result (what made the edge show).
- gdd_check: Read: color-code. A defect report: not diagnosed by the Planner (which effect, why the edges show is for the Executor). No conflict, no open value. Level light: fix on an existing effect, no new look.
- verification_level: light
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: none
- started: 2026-11-13T23:15:05Z
- waiting_for: none
- executor_tokens: input 174, cache write 147582, cache read 8468613, output 46886
- executor_usage: runs 1, turns 87, context start 16k peak 160k, resumes 0, 17 min, units 1.27 M, USD 3.37
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. A1 seen: no quad edge on the ground particles of the arrival extractor takeoff at 1920x1080 (Saved/Captures/task542-after2/after_04.png; before Saved/Captures/task542-before/before_02.png; crops Saved/Scratch/task542/crop-compare.png, temporary). A2: the fix is in the shared material M_ExtractorGroundWind, so all four MI_GroundWind_* and both extractors (arrival, exit) get it. A3: only the UV wrap changed (saturate instead of frac), identical inside the sprite; same puffs in the frames. A4 cause: the particles are NS_ExtractorGroundWind (cancelled task-466 partial state), not the NS_ExtractorRotorWash GroundBlast; its Custom node used frac(UV), which jumps at the sprite border inside a 2x2 pixel quad, so the UV derivative exploded and the GPU sampled the smallest mip (beige, non-zero alpha): dotted lines on every quad edge (80bd4cad). Doc bp-extractor-ground-wind (5490c561). ALERT: executor run 87 turns, 1.27 M units, over the light budget (40 turns, 0.6 M), under 4x: cause the first diagnosis went to the documented but unreferenced M_ExtractorBlastWind (edit, captures, zero-alpha test, then revert) before the asset registry showed the real effect.

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q2 Bords des particules au sol : défaut = on corrige pour que les bords ne se voient plus, sans changer couleur, taille, durée ni nombre ; la cause est notée dans le résultat.

## Subtasks
1. task-542-01-fix-edges (vault/reference/tasks/task-542-ground-particle-edges/task-542-01-fix-edges.md) — bp: Find the ground particle effect of image 2 and remove its visible sprite edges
2. task-542-02-capture (vault/reference/tasks/task-542-ground-particle-edges/task-542-02-capture.md) — capture: Before/after capture of the effect on the floor
3. task-542-03-doc (vault/reference/tasks/task-542-ground-particle-edges/task-542-03-doc.md) — doc: Note the fix in the effect doc node

## Relations
- [[tasks-done-index]] — parent index.
- [[task-542-01-fix-edges]] — step 1, bp.
- [[task-542-02-capture]] — step 2, capture.
- [[task-542-03-doc]] — step 3, doc.
