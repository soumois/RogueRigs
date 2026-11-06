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


- [[task-460-alert-letters-readable]] — On the alert bar the C, B and S letters stay readable when a red horde "!" is at or moves over their position. Status in-progress.
- [[task-461-bottom-hud-panel]] — Health bar and the four weapon slots in one riveted panel, XP bar in its own riveted frame with a framed Lv cap, as in the retained mockup. Status todo.
- [[task-462-extractor-blast-cartoon]] — The extractor ground blast, today blurry white blobs, is redone from found references in the cartoon style of the explosions. Status todo.
- [[task-463-aim-assist]] — A rifle bullet is deflected a few degrees toward a nearby robot, unless the shot already hits a robot or a barrel. Status todo.
- [[task-464-player-shot-colour]] — Player shots, fired far more often, get a colour as vivid as the red enemy lasers; enemy shots stay red. Status wait-answers.
