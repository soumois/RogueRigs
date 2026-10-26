---
id: task-315-paid-reroll
description: "Global task: The level-up reroll costs Duskite ore: 15, plus 5 for each further reroll without a level-up, usable while the ore lasts."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Paid level-up reroll

- request: "Le reroll coûte maintenant 15 du minerai mauve pour être utilisé, chaque fois qu'on veut l'utiliser à la suite (sans augmenter de niveau), cela augmente de 5." (owner, 2026-10-26)
- goal: The level-up reroll costs Duskite ore: 15, plus 5 for each further reroll without a level-up, usable while the ore lasts.
- acceptance: reroll costs 15, 20, 25 in a row and resets to 15 when a new level-up screen opens (a chest screen keeps the count); several rerolls per screen while ore suffices; refused without enough ore; ore spent from carried ore first, then secured ore; the reroll button shows the cost and is greyed when unaffordable; -game before | after capture of the level-up screen sent to the owner; docs and xp-level-up updated; lint clean; committed.
- gdd_check: xp-level-up, run-shop, ore-economy, ore-blocks, color-code, rarities read (2026-10-26). Conflict: xp-level-up said one free reroll per screen; the owner's request replaces it. Ore = Duskite, the only ore. Owner defaults accepted 2026-10-26: Q1 unlimited rerolls while ore suffices; Q2 carried ore first, then secured ore; Q3 cost back to 15 at each level-up screen, chained level-ups included, a chest screen keeps the count; Q4 button shows the cost, greyed when unaffordable.
- confirmed_by_owner: yes (owner 2026-10-26: "ok" to questions 1-9, every default accepted; plan task-315 then task-316 confirmed)
- milestone: M2
- next_step: task-315-02-bp-reroll-button
- result: open

## Subtasks
1. task-315-01-paid-reroll — cpp: cost rule, ore spending, Blueprint API
2. task-315-02-bp-reroll-button — bp: WBP_UpgradeChoice shows the cost, greys the button
3. task-315-03-capture-reroll — cpp: capture the level-up screen in game

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-315-01-paid-reroll]] — step 01, cpp.
- [[task-315-02-bp-reroll-button]] — step 02, bp.
- [[task-315-03-capture-reroll]] — step 03, cpp.
