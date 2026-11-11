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




- [[task-504-options-screen-look]] — Options screen look: one Kie.ai GPT Image 2 mockup validated by the owner, then every element mapped to an existing sprite or a new PNG. Status todo.
- [[task-505-options-screen-sound-graphics]] — Options screen built from the task-504 look: Sound and Graphics tabs, settings saved and applied at launch, opened by the main menu Options button. Status todo.
- [[task-506-options-gameplay-and-pause]] — Adds the Gameplay tab (damage numbers, aim indicator, screen shake, focus-loss pause) and an Options button to the pause menu. Status todo.
