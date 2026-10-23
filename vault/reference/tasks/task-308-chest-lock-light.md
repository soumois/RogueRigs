---
id: task-308-chest-lock-light
description: "Global task: The chest lock light glows red blinking while waiting, orange blinking while the miner is in the zone, and steady green once open."
parent: tasks-to-do-index
kind: task
status: todo
---
# Bonus chest lock light states

- request: "Il faut aussi un material pour la partie rouge, il clignote à intervalle régulier. Lorsqu'on rentre dans la zone, cela passe orange (clignote toujours). Lorsque le coffre s'ouvre, cela passe vert et ça ne clignote plus. (il faut que ça glow dans toutes les couleurs)" (owner, 2026-10-23)
- goal: The chest lock light glows red blinking while waiting, orange blinking while the miner is in the zone, and steady green once open.
- acceptance: chest Idle: red glow blinking; Hacking (miner in the zone): orange glow blinking; Complete: green glow, steady; back to red when the miner leaves the zone before completion; emissive glow in all three; -game frames of the three states sent to the owner; committed.
- gdd_check: objective-bonus-chest, color-code, art-style-guide read (2026-10-23). No A7 conflict: the chest is corporate, the light is a detail (colour principle 2026-10-13). Pending owner answers, defaults Q8: blink 2 Hz on/off like the console lamp; same red, orange and green values as task-302; reuse M_ConsoleLamp (task-301-04) through MI_ChestLockLight; the state drive extends the console code of task-302-01. Runs after task-306, task-301-04 and task-302-01.
- confirmed_by_owner: no (planned 2026-10-23, "ne lance rien pour l'instant"; waiting for the owner's answers, the defaults below apply if he answers "ok")
- milestone: M2
- next_step: task-308-01-bp-chest-lock-light
- result: open

## Subtasks
1. task-308-01-bp-chest-lock-light — bp: MI_ChestLockLight on the chest latch
2. task-308-02-chest-lock-light-drive — cpp: Drive the chest lock light from its state
3. task-308-03-capture-chest-lock-light — cpp: Capture the chest lock light states

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-308-01-bp-chest-lock-light]] — step 01, bp.
- [[task-308-02-chest-lock-light-drive]] — step 02, cpp.
- [[task-308-03-capture-chest-lock-light]] — step 03, cpp.
