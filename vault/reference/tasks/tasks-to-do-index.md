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
- [[task-359-capture-tooling]] — Capture tooling: crops and labelled comparison sheets with ffmpeg built in, and a generated list of rr.* debug console commands per system. Status in-progress.
- [[task-360-capture-agent]] — New Sonnet capture agent: runs -game captures from a planner checklist with a minimal brief, reports each point seen, not seen or ambiguous. Status todo.
- [[task-361-bp-agent-cost]] — Cut bp agent cost: batch independent MCP calls, a compact tool sheet so toolset discovery is skipped, describe single widgets rather than trees. Status todo.
