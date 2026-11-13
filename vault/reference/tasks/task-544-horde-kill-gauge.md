---
id: task-544-horde-kill-gauge
description: "Global task: a mark at half of C starts a kill gauge under the alert bar; each full gauge brings a horde."
parent: tasks-to-do-index
kind: task
status: todo
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde

- request: "* Mettre le premier "!" à la moitié entre 0 et C, une fois que c'est fait on ne déclenche pas la vague. Juste une petit jauge vient s'ajouter à la barre en dessous et se remplit quand on tue des ennemis. Une fois pleine, une vague arrive. Elle reste pleine jusqu'à ce que la vague soit passée, puis se vide et on recommence à remplir cette jauge jusqu'à la prochaine vague." [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30
Q9 : J'attends un rendu avec chatgpt, utilise l'API

ok pour le reste" = the plan and the defaults of Q5, Q6, Q7, Q8, Q9 accepted.
- goal: Hordes are triggered by a kill gauge under the alert bar, started when the bar reaches a single "!" placed at half the C position, as written in level-flow and enemies-spawn-rules. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 One "!" shows on the alert bar at half the C position (20 % in M1) from the level start; no other horde "!" on the bar. A2 The bar reaching it starts no horde: the "!" goes and the horde gauge appears under the alert bar. A3 The gauge fills with robot kills (all kill sources): 70 for the first horde, 150 for each next (values in data, provisional). A4 Full gauge: the horde comes with its lull, banner and front as before; the gauge stays full, kills not counted, until the horde is over (all its robots destroyed, or 30 s after its front ends, whichever first), then empties and fills again; repeats until extraction. A5 The gauge fills while the bar is paused by the console. A6 Overtime (Q8 answer): from 3:30 of level time each kill counts 2 in the gauge, from 4:30 it counts 3; the 30 s overtime horde timer is removed, hordes come only from the gauge; each horde triggered from 3:30 adds 20 % of base health to robots spawned after it (additive: +20 %, +40 %, ..., on top of threat; values in data); the wave interval halving at each overtime minute is unchanged; no fixed-time horde remains. A7 Gauge look (Q9 answer): from the GPT Image 2 render of task-544-02 (brief: thin red fill on a dark track, about a third of the alert bar width, centred under it, small "!" at its left, pulsing while full); if 02 recorded a gap (no key, tool failure), the brief look built directly; laid out per umg-layout. A8 Existing tests pass; only expectations the change breaks are updated.
- gdd_check: Read: level-flow (M1: one "!" per horde, task-525; kills add nothing to the bar before the first horde front), enemies-spawn-rules (two hordes at fixed times, task-524; lull, banner, front), difficulty (overtime hordes every 30 s from 3:30). Conflicts: fixed horde times (task-524) and one "!" per horde (task-525) are replaced by the gauge: edits to level-flow and enemies-spawn-rules applied at confirmation. Kept unchanged: kills add nothing to the bar before the first horde front; horde composition, lull and banner; overtime. Open values the work needs, set by Q5 to Q7 defaults and the Q8, Q9 answers: kill counts, end of a horde, gauge during pause and overtime, gauge look. Q8 answer conflicts with difficulty overtime (a horde every 30 s): resolved by the Planner as the most coherent reading, the gauge replaces the 30 s timer and speeds up instead (x2 from 3:30, x3 from 4:30); "pv +20 % à chaque vague après 3:30" read as each horde (gauge wave) from 3:30, additive on base health, robots spawned afterwards only (same rule as threat); the regular wave interval halving is kept. Edits to difficulty and level-flow. Q9: the project image API is GPT Image 2 (ChatGPT image model) through Kie.ai (agent-art, kie-image-workflow); a render subtask (art) is added before the widget, with a no-wait fallback. No code read. Level standard: changed behaviour and new HUD element.
- verification_level: standard
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30
Q9 : J'attends un rendu avec chatgpt, utilise l'API

ok pour le reste")
- milestone: M2
- next_step: task-544-01-gauge-logic
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30
Q9 : J'attends un rendu avec chatgpt, utilise l'API

ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q5 Lecture : défaut = un seul "!" sur la barre, à la moitié de la position de C (20 %) ; quand la barre l'atteint, aucune vague, le "!" disparaît et la jauge apparaît sous la barre ; ensuite toutes les vagues viennent de la jauge (plus d'autre "!" sur la barre, plus de vague à heure fixe).
- Q6 Taille de la jauge : défaut = 70 robots tués pour la première vague, 150 pour chaque suivante (le rythme de kills monte avec le temps), tous les kills comptent (tourelles comprises), provisoire.
- Q7 "Vague passée" : défaut = tous les robots de la horde détruits, ou 30 s après la fin de son front si certains vivent encore ; les kills pendant que la jauge est pleine ne comptent pas pour la suivante.
- Q8 Cas limites : défaut = la jauge se remplit même quand la barre est en pause (console) ; accalmie de 5 s et bannière "HORDE EN APPROCHE !" inchangées ; en overtime (3:30) la jauge est cachée et les hordes toutes les 30 s restent ; la règle "les kills ne remplissent pas la barre avant la première horde" reste. Réponse du owner : "en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30" ; le reste du défaut tient ; le minuteur de 30 s disparaît (décision du Planner, voir gdd_check).
- Q9 Look de la jauge : défaut = fine barre rouge (le rouge du "!") sur fond sombre, environ un tiers de la largeur de la barre d'alerte, centrée juste dessous, un petit "!" à gauche, qui pulse quand elle est pleine. Réponse du owner : "J'attends un rendu avec chatgpt, utilise l'API" : rendu GPT Image 2 par l'API du projet (task-544-02), le défaut sert de brief et de repli.

## Subtasks
1. task-544-01-gauge-logic (vault/reference/tasks/task-544-horde-kill-gauge/task-544-01-gauge-logic.md) — cpp: Single "!" at half of C, kill gauge state and horde trigger replacing the fixed horde times, overtime multipliers and health step
2. task-544-02-gauge-render (vault/reference/tasks/task-544-horde-kill-gauge/task-544-02-gauge-render.md) — art: GPT Image 2 render of the gauge in the HUD through the project image API, then its elements (no-wait fallback)
3. task-544-03-gauge-widget (vault/reference/tasks/task-544-horde-kill-gauge/task-544-03-gauge-widget.md) — bp: Horde gauge widget under the alert bar and the moved "!"
4. task-544-04-capture (vault/reference/tasks/task-544-horde-kill-gauge/task-544-04-capture.md) — capture: Captures of the "!" and the gauge through one horde cycle, four resolutions
5. task-544-05-doc (vault/reference/tasks/task-544-horde-kill-gauge/task-544-05-doc.md) — doc: Document the horde gauge in the system and widget nodes

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-544-01-gauge-logic]] — step 1, cpp.
- [[task-544-02-gauge-render]] — step 2, art.
- [[task-544-03-gauge-widget]] — step 3, bp.
- [[task-544-04-capture]] — step 4, capture.
- [[task-544-05-doc]] — step 5, doc.
