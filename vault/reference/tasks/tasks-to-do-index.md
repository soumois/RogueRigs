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
- [[task-341-shop-design-prompt]] — A ready-to-paste ChatGPT prompt giving the full context of the shop screen so the owner can discuss design and game feel proposals. Status todo.
- [[task-342-shop-ore-flight]] — When a shop card is bought, ore crystals fly from the balance counter to that card's Buy button while the balance counts down. Status todo.
- [[task-343-shop-buy-juice]] — Clicking an affordable Buy button gives a juicy press reaction of its own, separate from the ore flight. Status todo.
- [[task-344-shop-card-burn]] — A bought shop card plays the level-up card pick effect, without the flight to the weapon or XP bar, before showing the sold look. Status todo.
- [[task-345-turret-meshy-files]] — Identify which owner Meshy turret zip is the optimised mesh by headless Blender triangle counts, save both in Art/Source/turret-meshy/ and record them in a new node art-prompts-turret. Status wait-answers.
- [[task-346-turret-canon]] — Replace the wrong canon of the optimised Meshy turret with a canon taken from or modelled after the unoptimised mesh, as the owner chooses. Status wait-answers.
- [[task-347-turret-flat-surfaces]] — Make the surfaces that should be flat on the optimised Meshy turret flat again, using the unoptimised mesh as the reference. Status wait-answers.
