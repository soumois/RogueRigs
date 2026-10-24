---
id: task-303-console-hack-reward
description: "Global task: When the console hack completes, three big XP chips fly out in every direction and a big green ring flash plays."
parent: tasks-done-index
kind: task
status: done
---
# Console hack reward: XP burst and big particle

- request: "Au moment où on a piraté la zone, on a de l'xp qui partent dans toutes les directions (équivalent à 3 gros) et une grosse particule." (owner, 2026-10-23)
- goal: When the console hack completes, three big XP chips fly out in every direction and a big green ring flash plays.
- acceptance: on console hack complete (not the bonus chest): 3 big XP chips (8 nuts each, 24 in all) flung in 3 evenly spread directions with the long pickup arc, collectable; a green expanding ring flash with white-gold sparks plays once; reward in the GDD; -game frames sent to the owner; committed.
- gdd_check: objective-console read (2026-10-23): it has no reward; the owner default adds it (3 big chips, a big chip = 8 basic nuts, provisional owner 2026-10-23). Particle: green ring flash with white-gold sparks (green allowed by the console exception of task-302). Runs after task-301.
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Capture Saved/ConsoleReward/ (contact_sheet_console_reward.png + 12 frames, slomo 0.1): green ring ~200 uu at 0.27 s, ~290 uu at 0.42 s, faded by 0.57 s; white-gold sparks faint, seen at 0.87-1.17 s only; 3 chips fly out, the miner (150 uu away) collected one, 2 rest NE ~290 uu and SE ~210-260 uu; no change, no commit. Owner 2026-10-24: keeps the particle as is ("Non, je garde la particule comme ça"), faint sparks accepted.

## Subtasks
1. task-303-01-console-xp-burst — cpp: Console hack XP burst
2. task-303-02-bp-hack-complete-fx — bp: NS_ConsoleHackComplete and its wiring
3. task-303-03-capture-hack-reward — cpp: Capture the console hack reward

## Relations
- [[tasks-done-index]] — parent index.
- [[task-303-01-console-xp-burst]] — step 01, cpp.
- [[task-303-02-bp-hack-complete-fx]] — step 02, bp.
- [[task-303-03-capture-hack-reward]] — step 03, cpp.
