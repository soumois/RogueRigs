---
id: tasks-to-do-index
description: "Open global tasks (in progress, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done (`vault/scripts/task-status.js` does it).

## Relations
- [[tasks-index]] — parent index.
- [[task-188-editor-lock]] — Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it. Status todo.
- [[task-230-rock-slab-integration]] — Dual-grid slab rock look in the grid: pieces, world-UV top texture, cracks, plates, crystals. Status in-progress.
- [[task-234-ore-deposit-look]] — Each 2 x 2 ore or heal deposit is drawn as one crystal formation, at least 3 variants per type, from images the owner generates with prompts we write. Status in-progress.
- [[task-258-heal-deposit-vibration]] — The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it. Status todo.
- [[task-259-ore-dig-time-2s]] — Ore blocks (ore, special ore and heal ore) take 2 s instead of 1.5 s to dig at mining speed 1. Status todo.
- [[task-260-rock-dig-time-065]] — Rock blocks take 0.65 s instead of 0.5 s to dig at mining speed 1. Status todo.
