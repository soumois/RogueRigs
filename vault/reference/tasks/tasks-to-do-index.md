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
- [[task-343-shop-buy-juice]] — Clicking an affordable Buy button gives a juicy press reaction of its own, separate from the ore flight. Status in-progress.
- [[task-344-shop-card-burn]] — A bought shop card plays the level-up card pick effect, without the flight to the weapon or XP bar, before showing the sold look. Status todo.
- [[task-346-turret-canon]] — Rebuild the optimised Meshy turret canon by Blender script after the unoptimised mesh, with a perforated machine-gun shroud of real holes. Status in-progress.
- [[task-347-turret-flat-surfaces]] — Make the surfaces that should be flat on the optimised Meshy turret flat again, using the unoptimised mesh as the reference. Status todo.
- [[task-348-shop-screen-redesign]] — Rebuild the shop screen to the ChatGPT mockup look, making only the sprites the game lacks; stat icons and card frames stay. Status todo.
