---
id: task-307-chest-one-upgrade
description: "Global task: Opening the bonus chest gives exactly one upgrade choice: its direct XP grant no longer triggers a second level-up."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Bonus chest gives one upgrade, not two

- request: "Actuellement, on gagne deux niveaux lorsqu'on ouvre le coffre, Il faut une amélioration, pas deux." (owner, 2026-10-23)
- goal: Opening the bonus chest gives exactly one upgrade choice: its direct XP grant no longer triggers a second level-up.
- acceptance: on chest reward the miner gets exactly one upgrade choice (+3 luck) and no level-up from the reward itself, whatever the XP bar fill; ore chunks unchanged; spec objective-bonus-chest and tech doc updated; only tests this change breaks are updated; committed.
- gdd_check: objective-bonus-chest and sys-objectives-console-chest read, RRBonusChest.cpp checked (2026-10-23). Cause: GrantReward calls AddXP(0.5 x level need) then QueueBonusChoice; when the bar is already at least half full (less with the XPGain stat) the XP adds a normal level-up, hence two choices. Spec change needed (reward XP 50 % of level need). Pending owner answer, default Q6: drop the direct XP grant (XPNeedFraction default 0, property kept); the XP of the chest becomes the thrown chips of task-309 (Q7), collected like any XP.
- confirmed_by_owner: yes (owner 2026-10-23: "ok pour tous les défauts (Q1 à Q25)"; every default below applies; launched 2026-10-24)
- milestone: M2
- next_step: task-307-01-chest-one-upgrade
- result: open

## Subtasks
1. task-307-01-chest-one-upgrade — cpp: Drop the chest direct XP grant

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-307-01-chest-one-upgrade]] — step 01, cpp.
