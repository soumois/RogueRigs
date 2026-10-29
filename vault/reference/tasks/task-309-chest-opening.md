---
id: task-309-chest-opening
description: "Global task: Chest opening: latch swings, lid slams open with overshoot, freeze and shake, inner light, ground ring, three popping XP chips, sparks."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Bonus chest opening: latch, lid slam, light and XP burst

- request: "lorsqu'on ouvre le coffre, il faudra pivoter la partie sur l'image (avec le point de pivot en haut), puis le coffre s'ouvre après 0.5sec. Il y a une particule qui se lance et en plus de l'amélioration gagnée, il doit y avoir de l'xp lancé dans toutes les directions (environ l'équivalent de 3 gros)." (owner, 2026-10-23); after the game-feel video Art/Source/chest/reference/egor-chest-juice.mp4: "309 - D E F G et H" (owner, 2026-10-23; B and C refused: no chest wobble during the hack, no squash or stretch of the body).
- goal: On chest hack complete the latch swings up, 0.5 s later the lid slams open with an overshoot, the chest freezes an instant and the camera shakes lightly, an inner light with turning rays and a ground ring play, three big XP chips pop out and sparks float up.
- acceptance: on chest hack complete: the latch rotates about its top edge; 0.5 s later the lid opens past its target, holds an instant (chest only, no global slow-down), settles and stays open (D, E); a light camera shake follows (E, task-310); a ground ring plays at the slam (G); the interior glows with a point light and slowly turning rays (F); 3 big XP chips (8 nuts each) pop out with a scale overshoot in evenly spread directions, the 5 ore chunks pop and the one upgrade choice is queued (H); small sparks float up from the open chest for 2 to 3 s and fade (H); the lid turns about its hinge pins and opens up and backward, no hinge-block stub stays on the body back wall, the latch hasp does not dangle once open and no dark recess shows behind the latch (owner 2026-10-24); NS_HeavySlamImpact unchanged for the heavy; spec updated; -game frames sent to the owner; committed.
- gdd_check: objective-bonus-chest read (2026-10-23). Overlaps task-307: the thrown XP replaces the 50 % direct XP (default Q7). Pending owner answers, defaults: Q7 3 big chips of 8 nuts, same burst code as the console (task-303-01); Q9 latch 90 deg outward and up in 0.2 s; Q10 lid starts 0.5 s after the latch starts, 100 deg in 0.4 s, stays open; Q11 particle, XP, ore and upgrade choice at lid opening; Q12 replaced by Q22 and Q25 (no separate flash). Owner video techniques (2026-10-23), color-code, art-style-guide, kessler-voss-logo and bp-heavy-slam-impact read; colour check: company yellow on the corporate chest is a detail (colour principle 2026-10-13), A7 not involved, no conflict; the company yellow has no linear value in the vault. Provisional defaults: Q18 lid to 115 deg in 0.4 s then back to 100 deg in 0.15 s; Q19 chest-only freeze 0.06 s at the 115 deg peak, ring at the freeze start, shake at its end; Q22 point light, emissive Interior slot, 4 rays turning about 30 deg/s, light and rays fade out after 3 s, interior stays lit; Q23 company yellow sampled from the chest logo plate; Q24 NS_HeavySlamImpact gains DustColor and ChipCount at the heavy values, chest ring radius 200 uu, faint yellow, no chips; Q25 chips scale 0 to 1.3 to 1.0 in 0.25 s (chest only), about 20 yellow sparks rising for 2.5 s. Runs after task-306, task-307, task-310 and task-303-01.
- confirmed_by_owner: yes (owner 2026-10-23: "ok pour tous les défauts (Q1 à Q25)"; every default below applies; not launched yet)
- owner_2026-10-24: "ok : je garde la commande debug, j'accepte les 3864 tris, le pivot et la pose ouverte se règlent dans 309, et ok pour tous les défauts de la tâche 314." Pivot and open pose moved here from task-306: new 3d step task-309-07 (runs first: lid origin on the hinge pins, no hinge stubs on the body, latch hasp not dangling, recess behind the latch filled, opening signs reported); task-309-01 uses those signs; task-309-05 reimports the three meshes and moves the Lid and Latch components to the new pivots; task-309-06 checks the open pose. Task-314-01 cuts the logo into the lid after task-309-07.
- milestone: M2
- next_step: task-309-07-chest-hinge-recut
- started: 2026-10-29T18:12:12Z
- waiting_for: owner go on the 309-07 renders (Art/Source/chest/clean/renders/cut_open*.png); decide lid hover when open
- result: open

## Subtasks
1. task-309-01-chest-opening-sequence (vault/reference/tasks/task-309-chest-opening/task-309-01-chest-opening-sequence.md) — cpp: Chest opening sequence, slam, light drive and XP burst
2. task-309-02-bp-chest-interior-material (vault/reference/tasks/task-309-chest-opening/task-309-02-bp-chest-interior-material.md) — bp: MI_ChestInterior on the Interior slot
3. task-309-03-bp-slam-ring-params (vault/reference/tasks/task-309-chest-opening/task-309-03-bp-slam-ring-params.md) — bp: DustColor and ChipCount on NS_HeavySlamImpact
4. task-309-04-bp-chest-open-fx (vault/reference/tasks/task-309-chest-opening/task-309-04-bp-chest-open-fx.md) — bp: NS_BonusChestOpen, turning rays and floating sparks
5. task-309-05-bp-chest-open-wiring (vault/reference/tasks/task-309-chest-opening/task-309-05-bp-chest-open-wiring.md) — bp: BP_BonusChest opening wiring
6. task-309-06-capture-chest-opening (vault/reference/tasks/task-309-chest-opening/task-309-06-capture-chest-opening.md) — cpp: Capture the chest opening
7. task-309-07-chest-hinge-recut (vault/reference/tasks/task-309-chest-opening/task-309-07-chest-hinge-recut.md) — 3d: Lid pivot on the hinge pins and open-pose mesh fixes (runs first)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-309-01-chest-opening-sequence]] — step 01, cpp.
- [[task-309-02-bp-chest-interior-material]] — step 02, bp.
- [[task-309-03-bp-slam-ring-params]] — step 03, bp.
- [[task-309-04-bp-chest-open-fx]] — step 04, bp.
- [[task-309-05-bp-chest-open-wiring]] — step 05, bp.
- [[task-309-06-capture-chest-opening]] — step 06, cpp.
- [[task-309-07-chest-hinge-recut]] — step 07, 3d, runs first.
