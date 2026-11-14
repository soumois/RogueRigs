---
id: task-545-pause-screen-bolts
description: "Global task: pause screen to owner image 10: dividers from bar to bar, bolts on every end and junction as a separate unstretched image."
parent: tasks-done-index
kind: task
status: done
---
# Pause screen: grid and bolts on the frame bars as owner image 10

- request: "* L'écran pause ( image 9 ) devrait être comme sur l'image 10 ( deux pointes autour du titre sont un peu différentes que d'habitude, mais ça n'est que pour une référence de taille ). La principale différence vient des boulons qui sont bien sur la barre du cadre. Pour être sur que les boulons ne sont pas stretch, peut être qu'il faudrait en faire une image à part plutôt que des les inclure dans l'image de la barre. ( ou alors faire que l'extremité de la barre ne soit pas stretch avec la régles des 3x3 )" [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste" = the plan and the defaults of Q10, Q11 accepted.
- goal: The pause screen matches owner image 10 (grid, bolts on the bars, title bars size), with a bolt image and divider bar pieces made reusable for the other framed screens. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 Grid as pause-menu "Grid and bolts": both vertical dividers from the top frame bar to the bottom frame bar; the horizontal divider from the first vertical divider to the right frame side across the two weapon columns. A2 A bolt on every divider end and junction, centred on the bar it meets, round and unstretched at the four resolutions. A3 Bolts are a separate image; divider bars keep unstretched ends. A4 Title side bars sized and placed as image 10, current sprite. A5 Stats, texts, buttons and behaviour unchanged. A6 Bolt and divider pieces built as shared pieces another screen can place (used by the level cleared task). A7 Any new image is labelled keep in Art/Source and listed in a vault node; style-guide verdict on it.
- gdd_check: Read: pause-menu (Look: mockup process with the owner "je valide" before building), umg-layout via relations. Conflict: the "je valide" gate: the owner image 10 is the validated target and the owner is away (Q16); written in the pause-menu edit. Open: bolt image source, decided by Q11. Level standard: layout change and new asset.
- verification_level: standard
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste")
- milestone: M2
- next_step: none
- started: 2026-11-14T03:32:00Z
- waiting_for: none
- executor_tokens: input 228, cache write 195631, cache read 12384044, output 69356
- executor_usage: runs 1, turns 109, context start 16k peak 192k, resumes 2, 179 min, units 1.83 M, USD 4.84
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Commit 1f7c96e4. New node wbp-ui-bolted-bar (both shared widgets, layout table, how to place, Art/Source/ui-frame-pieces assets listed as keep, style-guide state); wbp-pause-menu anchors table, grid_and_bolts entry and relation updated; blueprints-ui-index child added; lint clean.

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30 / Q9 : J'attends un rendu avec chatgpt, utilise l'API / ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q10 Captures des écrans (pause, fin de niveau, boutique, jauge HUD) : défaut = les quatre résolutions de umg-layout ; les 4 petites corrections se contentent de 1920 x 1080.
- Q11 Boulons de l'écran pause : défaut = une image de boulon à part, posée sur chaque extrémité et chaque jonction des séparateurs ; barres en 3-slice (bouts non étirés) ; le boulon est découpé d'un sprite existant s'il est net, sinon généré par Kie.ai (1 variante, 24 crédits max) ; pointes du titre : sprite actuel, taille et position de l'image 10.

## Subtasks
1. task-545-01-bolt-art (vault/reference/tasks/task-545-pause-screen-bolts/task-545-01-bolt-art.md) — art: Separate bolt image and unstretchable divider bar pieces, from existing sprites or Kie.ai per Q11
2. task-545-02-pause-layout (vault/reference/tasks/task-545-pause-screen-bolts/task-545-02-pause-layout.md) — bp: Rebuild the pause frame grid with the bolt and bar pieces as shared pieces
3. task-545-03-capture (vault/reference/tasks/task-545-pause-screen-bolts/task-545-03-capture.md) — capture: Pause screen captures against image 10 at four resolutions
4. task-545-04-doc (vault/reference/tasks/task-545-pause-screen-bolts/task-545-04-doc.md) — doc: Update the pause widget doc and document the shared bolt and bar pieces

## Relations
- [[tasks-done-index]] — parent index.
- [[task-545-01-bolt-art]] — step 1, art.
- [[task-545-02-pause-layout]] — step 2, bp.
- [[task-545-03-capture]] — step 3, capture.
- [[task-545-04-doc]] — step 4, doc.
