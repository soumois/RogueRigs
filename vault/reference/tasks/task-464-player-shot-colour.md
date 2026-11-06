---
id: task-464-player-shot-colour
description: "Global task: Player laser shots and muzzle flashes (rifle, turrets) turn a varied green from the owner reference; shotgun and enemy shots unchanged."
parent: tasks-to-do-index
kind: task
status: todo
---
# Player shot colour

- request: Owner 2026-11-05, verbatim: "J'aimerais revoir les couleurs des tirs, cela fait sens que l'enemi tire un laser rouge, mais le soucis c'est que ça rend assez bien à l'écran et le joueur tire plus souvent." Answer to Q1-Q3, verbatim: "Les tirs laser pour le joueur pourrait être vert. Pour le fusil à pompe, ce sont des projectiles donc garde l'effet bullet normal ( plus tard quand il y aura plusieurs armes, on pourra faire des catégories )" Answer to Q4-Q6, verbatim: "Q4 - La tourelle est laser aussi / Q5 - Il va falloir ajuster la couleur, mais mettre de la variation, car actuellement le muzzle, c'est juste qu'une seule couleur. Dans ce style là ( c'est un exemple ) / Q6 - La couleur verte sur l'image semble bien pour l'instant / Ne lance pas la tâche", with a style example image (kept as Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp: 8-frame muzzle flash, small green ring at the muzzle, spiky star burst with a pale white-green core, saturated green spikes, long thin forward streak, green shards, dark green glow, ends in shards only). The owner said not to launch the task: plan only. Answer to Q8, verbatim: "Q8 - Tu peux rajouter le muzzle sur la tourelle aussi" (Q7 and Q9 not answered yet). "Ne lance pas la tâche" still holds. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q7, Q9, Q10 accepted.
- goal: The player's laser shots, the ones seen most, are green and look as good as the red enemy lasers, with several tones of green rather than one flat colour; projectile weapons keep the white-gold bullet look; enemy fire and the Shooter stay immediately recognisable.
- acceptance: A1 in a -game run the burst rifle laser shots and their impact sparks are the green of the owner reference, with tone variation (pale core, bright green, darker edge), as vivid as the red enemy lasers, no longer white-gold; A2 turret shots and their impact sparks have the same green look; A3 the burst rifle muzzle flash is green with several tones in the style of the reference (hot pale core, spiky burst, forward streak, shards, dark green glow, grows then fades into shards), no longer one flat colour; the reference is a style example, not an asset to copy; A4 shotgun pellets, their impact sparks and the shotgun muzzle flash keep today's white-gold look (default of Q7); A5 each turret shot shows a muzzle flash at the turret barrel with the same varied green look as the rifle flash, sized to the turret (default of Q10); A6 mining laser and aim laser white-gold; A7 enemy shots and targeting lasers unchanged, red; A8 with green shots on screen the acid green Shooter is still spotted in under a second, and green shots are not confused with heal, XP or ore pickups; A9 no gameplay value changes (turrets stay projectiles in the rules); A10 a capture compares before and after in the same fight and is sent to the owner; A11 color-code, readability-rules, burst-rifle and turret state the new look.
- gdd_check: Read color-code, readability-rules, weapons-index, burst-rifle, shotgun, turret. Decided by the owner: enemy keeps red; player laser shots green; the shotgun fires projectiles and keeps the normal bullet effect; turrets are laser too; the muzzle flash gets tone variation in the style of the example image; the green of the image is right for now (provisional). Weapon categories (laser versus projectile) are a later idea, not planned. Turret: the GDD said "projectile" and "turret bullets"; it stays a projectile in the rules (rule versus rendering), shown as a laser. Muzzle flash: one shared effect today, white-gold, one per shot of every miner weapon, none for turrets (burst-rifle node); hence Q7 (shotgun flash); Q8 answered: turrets get a muzzle flash too, hence Q10 (one per turret shot, sized to the turret). Conflict: acid green is reserved for enemies (Shooter); the green of the image is a bright, slightly yellowish green, so A8 checks the Shooter still stands out; the owner accepted that green for now. Image stored as keep in Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp (Q9). Edits (color-code rows, readability-rules line, burst-rifle vfx_sfx, turret projectile and vfx_sfx) are applied on confirmation with the defaults. No open value needed. No code read.
- confirmed_by_owner: yes (2026-11-06, "Ok, tout par défaut")
- milestone: M2
- next_step: task-464-01-rifle-laser-green
- started: open
- waiting_for: none
- executor_tokens: none (no executor run)
- executor_usage: none
- planning_tokens: input 68, cache write 78624, cache read 1704871, output 34609
- planning_usage: ac60e613 "Plan shot colour review" 1/1 (tasks 464), units 0.44 M
- result: open

## Questions
Decided 2026-11-06 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-06.
- Q7 Le flash au canon est aujourd'hui le même pour toutes les armes du mineur. Le fusil à pompe (projectiles) garde son flash blanc-or actuel, et seul le flash du fusil laser passe au vert avec variation ? Default: oui, flash du fusil à pompe inchangé.
- Q9 Ton image d'exemple est gardée comme référence de style ("keep") dans Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp et citée dans le code couleur. Default: oui, keep.
- Q10 Flash des tourelles : un flash par tir de tourelle, au bout de son canon, même effet vert que le fusil mis à la taille de la tourelle. Default: oui.

## Subtasks
1. task-464-01-rifle-laser-green (vault/reference/tasks/task-464-player-shot-colour/task-464-01-rifle-laser-green.md) — bp: Burst rifle laser shot and its impact sparks in the varied green of the owner reference, as vivid as the red enemy laser (representative slice)
2. task-464-02-turret-laser-green (vault/reference/tasks/task-464-player-shot-colour/task-464-02-turret-laser-green.md) — bp: Turret shots and their impact sparks in the same green look as the rifle laser
3. task-464-03-muzzle-flash-green (vault/reference/tasks/task-464-player-shot-colour/task-464-03-muzzle-flash-green.md) — bp: Burst rifle muzzle flash green with several tones in the style of the owner reference; shotgun flash stays white-gold
4. task-464-04-turret-muzzle-flash (vault/reference/tasks/task-464-player-shot-colour/task-464-04-turret-muzzle-flash.md) — bp: Turrets show the green muzzle flash of task-464-03 at their barrel on each shot, sized to the turret
5. task-464-05-doc (vault/reference/tasks/task-464-player-shot-colour/task-464-05-doc.md) — doc: Update the bp-* and sys-* nodes of the changed effects and check the gdd nodes match the result
6. task-464-06-capture (vault/reference/tasks/task-464-player-shot-colour/task-464-06-capture.md) — capture: Before/after capture of a fight with rifle, turret, shotgun and enemy shots and flashes, Shooter present, sent to the owner

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-464-01-rifle-laser-green]] — step 1, bp.
- [[task-464-02-turret-laser-green]] — step 2, bp.
- [[task-464-03-muzzle-flash-green]] — step 3, bp.
- [[task-464-04-turret-muzzle-flash]] — step 4, bp.
- [[task-464-05-doc]] — step 5, doc.
- [[task-464-06-capture]] — step 6, capture.
