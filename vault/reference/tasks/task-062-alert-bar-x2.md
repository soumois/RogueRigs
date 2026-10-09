---
id: task-062-alert-bar-x2
description: "Global task: Alert bar twice as wide and twice as tall, its icons scaled and repositioned."
parent: tasks-index
kind: task
status: done
---
# Bigger alert bar

- request: "Double la taille de la barre d’alerte, elle est trop petite" (owner, 2026-10-07)
- goal: Alert bar twice as wide and twice as tall, its icons scaled and repositioned.
- acceptance: PIE check with no error; docs updated; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. WBP_HUD AlertBar 300 × 18 → 600 × 36 (top 48), AlertLabel font 36 right-aligned, icons font 44, PlaceIcon Position × 600, ObjectiveText 140, HackBar 174, ShuttleText 196. Minimap not overlapped at 1920 wide (the long ALERT (paused) label may reach it below about 1300 px). PIE: no new error; not seen on screen. Doc wbp-hud. Commit dc64481.

## Subtasks
1. task-062-01-alert-bar-x2 — bp

## Relations
- [[tasks-index]] — parent index.
- [[task-062-01-alert-bar-x2]] — step 1, bp.
