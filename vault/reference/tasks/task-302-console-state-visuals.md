---
id: task-302-console-state-visuals
description: "Global task: The console shows its state: lamp colour and blink, antenna head rotation and screen colour driven by ERRConsoleState."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Console state visuals

- request: "Clignote Rouge, l'antenne ne bouge pas, l'écran reste noir - Il faut encore détruire de la roche" "Clignote Orange, l'antenne bouge lentement, l'écran reste noir - Il faut être dans la zone" "Clignote Vert, l'antenne bouge normalement, l'écran est vert - On est en train de prendre la zone." "Le clignotement ne se fait que sur la partie antenne, mais les autres parties oranges devront suivre le même code couleur." (owner, 2026-10-23)
- goal: The console shows its state: lamp colour and blink, antenna head rotation and screen colour driven by ERRConsoleState.
- acceptance: Locked: red, antenna still, screen black; Idle: orange, antenna 10 deg/s, screen black; Hacking: green, antenna 45 deg/s, screen green; Complete: steady green, screen green, antenna stopped; blink 2 Hz on the antenna material only; -game frames of the states sent to the owner; committed.
- gdd_check: objective-console, art-style-guide, color-code read (2026-10-23). Conflict A7 (red and green are enemy colours): owner default accepted, exception "console state lamps" added to art-style-guide A7 and color-code. Values provisional, owner 2026-10-23. Runs after task-301.
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-302-01-console-state-drive
- result: open

## Subtasks
1. task-302-01-console-state-drive — cpp: Drive the console lamps, head and screen from its state
2. task-302-02-capture-console-states — cpp: Capture the console states in game

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-302-01-console-state-drive]] — step 01, cpp.
- [[task-302-02-capture-console-states]] — step 02, cpp.
