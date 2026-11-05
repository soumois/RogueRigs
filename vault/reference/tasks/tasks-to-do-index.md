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

- [[task-451-hud-mockups]] — Image proposals of one unified in-game HUD, made with Kie.ai GPT Image from the current screenshot; the owner retains one before any build. Status wait-answers.
- [[task-452-health-above-weapons]] — The health bar leaves the top left and sits just above the weapon slots, in the retained unified look. Status todo.
- [[task-453-alert-bar-redesign]] — The alert bar with its swarm, console, bonus and shuttle marks is redrawn in the retained unified look; its rules stay. Status todo.
- [[task-454-swarm-icons-fixed]] — Until the first horde front is over, kills no longer move the swarms on the alert bar; afterwards they do as now. Status in-progress.
- [[task-455-xp-bar-ticks]] — Thin light ticks split the XP bar into 5 equal parts. Status todo.
- [[task-456-xp-gain-ghost]] — On an XP gain a pale ghost fill shows the new XP at once and the cyan fill catches up, the mirror of the health damage chunk. Status todo.
- [[task-457-radar-frame-ore]] — The radar gets a frame in the unified look, and the ore count moves into that frame instead of floating beside it. Status todo.
- [[task-458-weapon-slots-style]] — The frames of the 4 weapon slots take the retained unified look; their contents stay. Status todo.

