---
id: task-308-chest-lock-light
description: "Global task: Chest lock light: red blinking while waiting, orange blinking faster with hack progress in the zone, steady green once open."
parent: tasks-done-index
kind: task
status: done
---
# Bonus chest lock light states

- request: "Il faut aussi un material pour la partie rouge, il clignote à intervalle régulier. Lorsqu'on rentre dans la zone, cela passe orange (clignote toujours). Lorsque le coffre s'ouvre, cela passe vert et ça ne clignote plus. (il faut que ça glow dans toutes les couleurs)" (owner, 2026-10-23); after the game-feel video Art/Source/chest/reference/egor-chest-juice.mp4: "308 - A" (owner, 2026-10-23), A = the orange blink speeds up with hack progress.
- goal: The chest lock light glows red blinking while waiting, orange blinking faster as the hack progresses while the miner is in the zone, and steady green once open.
- acceptance: chest Idle: red glow blinking; Hacking (miner in the zone): orange glow blinking, the rate rising with hack progress (2 Hz at 0 % to 6 Hz at 100 %); Idle red keeps a steady rate; Complete: green glow, steady; back to red when the miner leaves the zone before completion; emissive glow in all three; -game frames of the three states sent to the owner; committed.
- gdd_check: objective-bonus-chest, color-code, art-style-guide read (2026-10-23). No A7 conflict: the chest is corporate, the light is a detail (colour principle 2026-10-13). Pending owner answers, defaults Q8: blink 2 Hz on/off like the console lamp; same red, orange and green values as task-302; reuse M_ConsoleLamp (task-301-04) through MI_ChestLockLight; the state drive extends the console code of task-302-01. Owner video (2026-10-23) technique A, pending owner answer, default Q17 (provisional): orange blink rate linear from 2 Hz at 0 % to 6 Hz at 100 % hack progress, updated continuously (fall back to steps 2, 3, 4, 6 Hz if the capture shows phase jumps); red Idle stays 2 Hz; leaving the zone goes back to red 2 Hz; re-entering resumes at the rate of the current progress. Runs after task-306, task-301-04 and task-302-01. M_Chest, on every chest slot since task-306-03, has no emissive input: the LockLight glow comes from MI_ChestLockLight (task-308-01).
- confirmed_by_owner: yes (owner 2026-10-23: "ok pour tous les défauts (Q1 à Q25)"; every default below applies; not launched yet)
- milestone: M2
- next_step: none
- started: 2026-10-29T17:41:31Z
- waiting_for: none
- executor_tokens: input 88, cache write 222492, cache read 1236060, output 15634
- executor_usage: runs 3, turns 38, context start 18k peak 54k, resumes 0, 29 min, units 0.48 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Not verified by capture: 5 capture launches never framed the latch (default camera too far; C1 not seen, C2-C4 ambiguous/not seen); owner declined an extra cpp rr.Debug framing subtask and checked the chest himself: 'Je valide' (validated visually by the owner, 2026-10-29). Note for the owner: orange blink rate is stepped 2/3/4/6 Hz per hack quarter, not linear. bp-bonus-chest lock_light now notes the C++ runtime override. Cost: capture ~59.9k + ~69.7k, refused cpp attempt ~18k.

## Subtasks
1. task-308-01-bp-chest-lock-light (vault/reference/tasks/task-308-chest-lock-light/task-308-01-bp-chest-lock-light.md) — bp: MI_ChestLockLight on the chest latch
2. task-308-02-chest-lock-light-drive (vault/reference/tasks/task-308-chest-lock-light/task-308-02-chest-lock-light-drive.md) — cpp: Drive the chest lock light from its state
3. task-308-03-capture-chest-lock-light (vault/reference/tasks/task-308-chest-lock-light/task-308-03-capture-chest-lock-light.md) — cpp: Capture the chest lock light states

## Relations
- [[tasks-done-index]] — parent index.
- [[task-308-01-bp-chest-lock-light]] — step 01, bp.
- [[task-308-02-chest-lock-light-drive]] — step 02, cpp.
- [[task-308-03-capture-chest-lock-light]] — step 03, cpp.
