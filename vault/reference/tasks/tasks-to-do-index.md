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
- [[task-496-generator-in-runs]] — Every run level built from the C++ layout generator: level 1 prepared from the main menu, next level from the extractor contact, hand JSON as fallback. Status wait-answers.
- [[task-497-barks-in-game]] — Rare barks of Rook and Vesna (through the drone) shown as subtitles on key events, one line at a time, 30 s cooldown, alerts keep priority. Status todo.
- [[task-498-heavy-slam-impact-fx]] — Capture first whether the heavy slam already shows a dust ring and rock chips; build only what is missing, in the explosions cartoon style. Status todo.
- [[task-499-robot-logo-decals]] — The yellow Kessler-Voss emblem added as a decal on the Heavy, Kamikaze and Chaser v2 meshes, like the shield emblem, without regenerating the Meshy meshes. Status todo.
