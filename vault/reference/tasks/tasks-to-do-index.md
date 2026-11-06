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


- [[task-463-aim-assist]] — A rifle bullet is deflected a few degrees toward a nearby robot, unless the shot already hits a robot or a barrel. Status todo.
- [[task-464-player-shot-colour]] — Player laser shots and muzzle flashes (rifle, turrets) turn a varied green from the owner reference; shotgun and enemy shots unchanged. Status todo.
- [[task-465-shot-impact-burst]] — Player shot impacts become a flat cartoon burst of spikes and triangles from the owner reference: white for projectiles, green for lasers. Status todo.
- [[task-466-extractor-ground-wind-refs]] — The extractor ground blast is redone after the owner sprite sheet and in-game mockup: flattened swirl rings, curved wind streaks, dust puffs pushed outward. Status in-progress.
- [[task-467-bottom-hud-match-target]] — Health strip, weapon slots and XP bar reworked, captured in game and compared with the owner target image, round after round until they match. Status todo.
