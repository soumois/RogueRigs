---
id: task-464-player-shot-colour
description: "Global task: Player shots, fired far more often, get a colour as vivid as the red enemy lasers; enemy shots stay red."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Player shot colour

- request: Owner 2026-11-05, verbatim: "J'aimerais revoir les couleurs des tirs, cela fait sens que l'enemi tire un laser rouge, mais le soucis c'est que ça rend assez bien à l'écran et le joueur tire plus souvent."
- goal: The shots the player sees most (his own) read and look as good on screen as the red enemy lasers, while enemy fire stays immediately recognisable as enemy fire.
- acceptance: A1 every player shot (burst rifle, shotgun wave, turret shots) and its impact sparks show the chosen colour (default: saturated glowing teal) in a -game run, no longer white-gold; A2 enemy shots and targeting lasers are unchanged (red moving lines); A3 in a fight, player shots and enemy shots are told apart at a glance, and player shots are not confused with XP pickups, heal pickups or ore pickups; A4 mining laser and aim laser stay white-gold; A5 no gameplay value changes (damage, rate, speed, spread, hit rules); A6 a capture compares before and after in the same fight and is sent to the owner; A7 color-code states the new player shot colour.
- gdd_check: Read color-code, readability-rules, weapons-index. Today: player shots, impact sparks, mining laser, aim laser white-gold; enemy shots and targeting lasers red. Conflict: option (b) of Q1 (player red) contradicts readability-rules "red means enemy fire or healing" and color-code; options (a) and (c) do not. Default teal is the player-side colour (miner, turrets, drone) and near the cyan XP nuts: accepted as moving line versus static floor object, same logic as red shots versus red heal pickups. The color-code edit (split the white-gold row) is applied on confirmation with the defaults. No open value needed; the exact tint, glow and thickness are the Executor's choice within A1 to A4. No code read. Rule versus rendering: look only, no gameplay change.
- confirmed_by_owner: no (plan and questions handed back 2026-11-06)
- milestone: M2
- next_step: task-464-01-rifle-shot
- started: open
- waiting_for: owner answers to Q1 to Q3
- result: open

## Questions
Asked 2026-11-06, each with a default; open until the owner answers.
- Q1 Je comprends : le rouge des lasers ennemis rend bien, et comme le joueur tire beaucoup plus souvent, ce sont ses tirs (blanc-or aujourd'hui) qui méritent un rendu aussi vif. Choix : (a) l'ennemi garde le rouge, les tirs du joueur prennent une nouvelle couleur vive ; (b) échange : le joueur tire rouge et l'ennemi prend une autre couleur (contredit la règle "rouge = tir ennemi ou soin") ; (c) on garde les couleurs mais le blanc-or devient plus lumineux et épais. Default: (a).
- Q2 Si (a), quelle couleur pour les tirs du joueur ? teal saturé lumineux (la couleur du camp joueur : mineur, tourelles, drone ; proche du cyan des écrous XP, mais une ligne qui bouge contre un objet posé au sol), ou or/ambre saturé. Default: teal saturé lumineux.
- Q3 Portée : les tirs de toutes les armes du joueur, tourelles comprises, et leurs étincelles d'impact changent ; le laser de minage et le laser de visée restent blanc-or ; une capture avant/après en combat t'est envoyée. Default: oui.

## Subtasks
1. task-464-01-rifle-shot (vault/reference/tasks/task-464-player-shot-colour/task-464-01-rifle-shot.md) — bp: Burst rifle shot and its impact sparks in the chosen colour, as vivid as the red enemy laser (representative slice)
2. task-464-02-other-shots (vault/reference/tasks/task-464-player-shot-colour/task-464-02-other-shots.md) — bp: Shotgun wave and turret shots (and their impact sparks) in the same colour as the rifle shot
3. task-464-03-doc (vault/reference/tasks/task-464-player-shot-colour/task-464-03-doc.md) — doc: Update the bp-* and sys-* nodes of the recoloured assets and check color-code matches the result
4. task-464-04-capture (vault/reference/tasks/task-464-player-shot-colour/task-464-04-capture.md) — capture: Before/after capture of a fight with player and enemy shots, sent to the owner

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-464-01-rifle-shot]] — step 1, bp.
- [[task-464-02-other-shots]] — step 2, bp.
- [[task-464-03-doc]] — step 3, doc.
- [[task-464-04-capture]] — step 4, capture.
