---
id: task-309-chest-opening
description: "Global task: On chest hack complete the latch swings up, the lid opens 0.5 s later, a particle plays, three big XP chips fly out."
parent: tasks-to-do-index
kind: task
status: todo
---
# Bonus chest opening: latch, lid, particle and XP burst

- request: "lorsqu'on ouvre le coffre, il faudra pivoter la partie sur l'image (avec le point de pivot en haut), puis le coffre s'ouvre après 0.5sec. Il y a une particule qui se lance et en plus de l'amélioration gagnée, il doit y avoir de l'xp lancé dans toutes les directions (environ l'équivalent de 3 gros)." (owner, 2026-10-23)
- goal: On chest hack complete the latch swings up, the lid opens 0.5 s later, a particle plays, three big XP chips fly out.
- acceptance: on chest hack complete: the latch rotates about its top edge, 0.5 s later the lid opens about its hinges and stays open; at lid opening a one-shot particle plays, 3 big XP chips (8 nuts each) fly out in evenly spread directions, the 5 ore chunks pop and the one upgrade choice is queued; spec updated; -game frames sent to the owner; committed.
- gdd_check: objective-bonus-chest read (2026-10-23). Overlaps task-307: the thrown XP replaces the 50 % direct XP (default Q7). Pending owner answers, defaults: Q7 3 big chips of 8 nuts, same burst code as the console (task-303-01); Q9 latch 90 deg outward and up in 0.2 s; Q10 lid starts 0.5 s after the latch starts, 100 deg in 0.4 s, stays open; Q11 particle, XP, ore and upgrade choice at lid opening; Q12 NS_BonusChestOpen: gold-yellow sparks and a short flash rising from the open lid. Runs after task-306, task-307 and task-303-01.
- confirmed_by_owner: no (planned 2026-10-23, "ne lance rien pour l'instant"; waiting for the owner's answers, the defaults below apply if he answers "ok")
- milestone: M2
- next_step: task-309-01-chest-opening-sequence
- result: open

## Subtasks
1. task-309-01-chest-opening-sequence — cpp: Chest opening sequence and XP burst
2. task-309-02-bp-chest-open-fx — bp: NS_BonusChestOpen and its wiring
3. task-309-03-capture-chest-opening — cpp: Capture the chest opening

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-309-01-chest-opening-sequence]] — step 01, cpp.
- [[task-309-02-bp-chest-open-fx]] — step 02, bp.
- [[task-309-03-capture-chest-opening]] — step 03, cpp.
