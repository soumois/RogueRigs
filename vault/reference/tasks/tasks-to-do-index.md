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
- [[task-477-bottom-hud-back-to-mockup]] — The bottom HUD (health strip, weapon slots, XP rail) is redone from the retained art mockup and reworked in a capture loop of at most 6 rounds until it matches it. Status in-progress.
- [[task-478-wall-lines-emissive-edge]] — The dark lines on wall faces go away so walls read as one continuous surface like owner image 3, and the orange emissive also lights the edge of the top face; up to 4 correction rounds. Status todo.
