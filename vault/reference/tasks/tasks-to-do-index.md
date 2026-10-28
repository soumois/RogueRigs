---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.
- [[task-308-chest-lock-light]] — Chest lock light: red blinking while waiting, orange blinking faster with hack progress in the zone, steady green once open. Status todo.
- [[task-309-chest-opening]] — Chest opening: latch swings, lid slams open with overshoot, freeze and shake, inner light, ground ring, three popping XP chips, sparks. Status todo.
- [[task-314-chest-lid-logo]] — Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game. Status todo.
- [[task-354-shop-counter-overlap]] — Shop screen crystal counter no longer covers the top-left corner of the shop panel nor the first card rivet. Status todo.
- [[task-355-shop-card-frame-details]] — Shop cards: no second frame line beside slot 0, tier tab text with room, sold panel fitted to the frame opening. Status todo.
- [[task-356-shop-screen-anchors]] — Shop screen rebuilt on a Root Canvas with explicit anchors from the owner's hand-edited version, so it adapts to small changes of resolution and ratio. Status wait-answers.
- [[task-357-shop-resource-indicator]] — Shop resource indicator (IndicatorRessource): ore icon and value centred on the font line without a Row container, following the screen-scaled font. Status wait-answers.
