---
id: task-548-shop-frame-layout
description: "Global task: The shop card panel gets a frame image made for its ratio (no stretched button frame) and the layout of owner image 8."
parent: tasks-to-do-index
kind: task
status: todo
---
# Spare Parts shop: own panel frame, layout as owner image 8

- request: "* L'écran ou on achète des amélioration entre deux niveaux ( voir image 7 ) , j'aimerais que tu l'ajustes comme sur l'image 8 et en plus régler le soucis du cadre qui semble étiré. Il semble que ça soit le même cadre que le bouton, mais il n'est pas prévu pour cette taille et ratio, peut être faudrait il une nouvelle image." [Same message, general points:] "* Si tu vois des habitudes ou des scripts souvent utiliser pour l'UI essaye de les généraliser. * Pose les questions et une fois répondu, je ne serais plus là et je m'attends à ce que les tâches soient faites, sauf si un agent a fait plus de 4 fois son budget ( met alors la tâche en pause et passe à la prochaine ). je ne veux pas de blocage car il faut sauvegarder dans unreal ou autre chose du même type." Owner images copied (temporary) to Saved/Scratch/owner-ui-batch-2026-11-13/ (1 main menu, 2 boss scene ground particles, 3 wall destruction, 4 HUD, 5 and 6 level cleared now and target, 7 and 8 shop now and target, 9 and 10 pause now and target). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30
Q9 : J'attends un rendu avec chatgpt, utilise l'API

ok pour le reste" = the plan and the defaults of Q14 accepted.
- goal: The shop screen matches owner image 8 with a card panel frame made for its size and ratio, nothing stretched. Batch rule (owner 2026-11-13, Q16): if the Executor passes 4 times the budget of this global's verification level (turns or units), it stops, writes where it stands and the overrun in `result`, sets the global `blocked`, and the relay goes on with the next global of the batch; the owner is away, so no subtask waits for him (no owner go, no wait-answers on a taste question: the owner target image is the reference) and the Unreal editor is opened, saved and closed by the Executor itself.
- acceptance: A1 The card panel uses its own frame image (not the button plate frame), 9-slice, corners, bolts and side pieces round and unstretched at the four resolutions. A2 Inner background darker and uniform, cards with a margin to the frame, proportions of panel, cards, prices, BUY, REROLL and CONTINUE as image 8. A3 Rules, texts, purchase effects and states (greyed, NEED n MORE, SOLD) unchanged. A4 The new image is keep in Art/Source, listed in a vault node, style-guide verdict pass, credits reported.
- gdd_check: Read: run-shop (look task-348, fit task-351 to 355 with cards 330 x 470, anchors task-356). Conflict: image 8 may change the card size: the 330 x 470 rule yields to image 8 (edit at confirmation). Open: frame image source, decided by Q14. Level standard: new asset and layout change.
- verification_level: standard
- family: gros
- confirmed_by_owner: yes (2026-11-13, "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30
Q9 : J'attends un rendu avec chatgpt, utilise l'API

ok pour le reste")
- milestone: M2
- next_step: task-548-01-frame-art
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "Q8 en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30
Q9 : J'attends un rendu avec chatgpt, utilise l'API

ok pour le reste"): every default below is accepted; values provisional, owner 2026-11-13.
- Q14 Cadre de la boutique : défaut = une nouvelle image de cadre faite pour ce format, générée par Kie.ai en image-to-image à partir de ton image 8 et du cadre actuel, 1 variante par tour, 2 tours max (48 crédits max), vérifiée par le style-guide, découpée en 9-slice ; la taille des cartes suit l'image 8 (la règle 330 x 470 cède).

## Subtasks
1. task-548-01-frame-art (vault/reference/tasks/task-548-shop-frame-layout/task-548-01-frame-art.md) — art: New shop panel frame image for its ratio, 9-slice ready, per Q14
2. task-548-02-shop-layout (vault/reference/tasks/task-548-shop-frame-layout/task-548-02-shop-layout.md) — bp: Apply the new frame and the image 8 layout to the shop screen
3. task-548-03-capture (vault/reference/tasks/task-548-shop-frame-layout/task-548-03-capture.md) — capture: Shop screen captures against image 8 at four resolutions
4. task-548-04-doc (vault/reference/tasks/task-548-shop-frame-layout/task-548-04-doc.md) — doc: Update the shop widget doc and list the new frame art

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-548-01-frame-art]] — step 1, art.
- [[task-548-02-shop-layout]] — step 2, bp.
- [[task-548-03-capture]] — step 3, capture.
- [[task-548-04-doc]] — step 4, doc.
