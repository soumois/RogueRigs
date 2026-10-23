---
id: task-294-kamikaze-split-damage
description: "Global task: Kamikaze explosion deals 40 to robots and 30 to the miner; barrels and terrain rule unchanged."
parent: tasks-to-do-index
kind: task
status: todo
---
# Kamikaze explosion 40 to robots, 30 to the miner

- request: "Passez les dégâts de l'explosion à 40, mais prévoir des dégâts différents sur le joueur (garder 30)" (owner, 2026-10-23)
- goal: Explosion rows get a separate miner damage; Kamikaze row: 40 to robots, 30 to the miner; barrel rows keep one value for both; no-one-shot rule, radius and terrain rule unchanged (provisional, owner 2026-10-23).
- acceptance: a kamikaze blast deals 40 to a robot and 30 to the miner (existing explosion tests updated); barrel damage unchanged; committed.
- gdd_check: enemy-kamikaze and explosions read (2026-10-23): damage 40 to robots, 30 to the miner written (provisional, owner 2026-10-23). No open value. No capture: a damage value, not a look.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-294-01-cpp-kamikaze-split-damage
- result: open

## Subtasks
1. task-294-01-cpp-kamikaze-split-damage — cpp: Separate miner damage for explosions

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-294-01-cpp-kamikaze-split-damage]] — step 01, cpp.
