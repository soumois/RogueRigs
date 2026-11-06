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


- [[task-462-extractor-blast-cartoon]] — The extractor ground blast, today blurry white blobs, is redone from found references in the cartoon style of the explosions. Status in-progress.
- [[task-463-aim-assist]] — A rifle bullet is deflected a few degrees toward a nearby robot, unless the shot already hits a robot or a barrel. Status todo.
- [[task-464-player-shot-colour]] — Player laser shots and muzzle flashes (rifle, turrets) turn a varied green from the owner reference; shotgun and enemy shots unchanged. Status todo.
- [[task-465-shot-impact-burst]] — Player shot impacts become a flat cartoon burst of spikes and triangles from the owner reference: white for projectiles, green for lasers. Status wait-answers.
