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
- [[task-487-main-menu-buttons-plate]] — rework the main menu New Run, Options and Exit buttons so they fit the menu look instead of bare floating text. Open to run or review it. Status wait-answers.
- [[task-488-loading-screen-rework]] — replace the plain dark loading screen (New Run and between levels) with an illustrated, framed screen in the game style. Open to run or review it. Status in-progress.
- [[task-489-main-menu-b-preview]] — one temporary mockup of the main menu with option B buttons (dark metal plates, white text, orange edge) so the owner can choose task-487 Q1. Status in-progress.
