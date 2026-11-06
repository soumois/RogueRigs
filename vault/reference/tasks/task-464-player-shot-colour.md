---
id: task-464-player-shot-colour
description: "Global task: The burst rifle laser shots turn green, as vivid as the red enemy lasers; shotgun pellets and enemy shots keep their look."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Player shot colour

- request: Owner 2026-11-05, verbatim: "J'aimerais revoir les couleurs des tirs, cela fait sens que l'enemi tire un laser rouge, mais le soucis c'est que ça rend assez bien à l'écran et le joueur tire plus souvent." Answer to the first questions (teal / gold / scope), verbatim: "Les tirs laser pour le joueur pourrait être vert. Pour le fusil à pompe, ce sont des projectiles donc garde l'effet bullet normal ( plus tard quand il y aura plusieurs armes, on pourra faire des catégories )"
- goal: The player's laser shots, the ones seen most on screen, are green and look as good as the red enemy lasers; projectile weapons keep the normal white-gold bullet look; enemy fire and the Shooter stay immediately recognisable.
- acceptance: A1 in a -game run the burst rifle laser shots and their impact sparks are a pure glowing green, no longer white-gold, and as vivid as the red enemy lasers; A2 shotgun pellets and their impact sparks keep the normal white-gold bullet look, unchanged; A3 turret bullets unchanged (default of Q4); A4 the muzzle flash, mining laser and aim laser stay white-gold (default of Q5); A5 enemy shots and targeting lasers unchanged, red; A6 with green shots on screen the acid green Shooter is still spotted in under a second, and the green shots are not confused with heal, XP or ore pickups; A7 no gameplay value changes; A8 a capture compares before and after in the same fight and is sent to the owner; A9 color-code, readability-rules and burst-rifle state the new colours.
- gdd_check: Read color-code, readability-rules, weapons-index, burst-rifle, shotgun, turret. Owner answers to the first round: Q1 (a) enemy keeps red; Q2 green, not teal or gold; Q3 changed: the shotgun fires projectiles and keeps the normal bullet effect. Weapon categories (laser versus projectile) are a later idea of the owner, not planned here. GDD fact: burst-rifle, shotgun and turret all say "projectile" and white-gold; the owner calls the rifle shots "laser", so only the rifle changes; the turret fires "turret bullets", hence Q4. Conflict: readability-rules reserves acid green for enemies (Shooter main colour); decision asked in Q6, default a pure green (0.1, 1, 0.2) as the heal number, never acid green, plus a check that the Shooter is still spotted in under a second. The heal number and console lamps already use green as a small detail; no other conflict. The muzzle flash NS_MuzzleFlash is shared by every miner weapon and white-gold: Q5. Edits (color-code rows, readability-rules line, burst-rifle vfx_sfx) are applied on confirmation with the defaults. No open value needed. No code read. Rule versus rendering: look only.
- confirmed_by_owner: no (plan and questions handed back 2026-11-06)
- milestone: M2
- next_step: task-464-01-rifle-laser-green
- started: open
- waiting_for: owner answers to Q4 to Q6
- result: open

## Questions
Asked 2026-11-06, each with a default; open until the owner answers.
- Q4 Les tourelles tirent des "balles" dans le GDD, comme le fusil à pompe. Elles gardent l'effet bullet normal blanc-or ? Default: oui, seules les tirs laser du fusil passent au vert.
- Q5 Le flash à la bouche du canon est commun à toutes les armes du mineur (blanc-or). On le laisse blanc-or, et seuls le tir laser et ses étincelles d'impact passent au vert ? Default: oui.
- Q6 Le vert acide est réservé aux ennemis (le Shooter). Pour le laser du joueur je propose un vert pur et saturé, environ le vert des chiffres de soin, jamais le vert-jaune acide du Shooter, et on vérifie à la capture que le Shooter se repère toujours en moins d'une seconde. Default: ok.

## Subtasks
1. task-464-01-rifle-laser-green (vault/reference/tasks/task-464-player-shot-colour/task-464-01-rifle-laser-green.md) — bp: Burst rifle laser shot and its impact sparks in pure glowing green, as vivid as the red enemy laser; other shots untouched
2. task-464-02-doc (vault/reference/tasks/task-464-player-shot-colour/task-464-02-doc.md) — doc: Update the bp-* and sys-* nodes of the recoloured assets and check color-code matches the result
3. task-464-03-capture (vault/reference/tasks/task-464-player-shot-colour/task-464-03-capture.md) — capture: Before/after capture of a fight with rifle, shotgun, turret and enemy shots, Shooter present, sent to the owner

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-464-01-rifle-laser-green]] — step 1, bp.
- [[task-464-02-doc]] — step 2, doc.
- [[task-464-03-capture]] — step 3, capture.
