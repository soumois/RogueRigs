---
id: task-051-turret-spacing
description: "Global task: A turret placed less than 100 uu from a standing turret moves to the nearest free floor cell at least 100 uu away."
parent: tasks-done-index
kind: task
status: done
---
# Turret spacing

- request: "Lorsqu’on pose deux tourelles au même endroit, elles se décalent pour ne pas être une sur l’autre" (owner, 2026-10-07)
- goal: A turret placed less than 100 uu from a standing turret moves to the nearest free floor cell at least 100 uu away.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. ARRTurretLauncher::FindSpacedCell: an aimed cell within TurretMinSpacing (100 uu, editable, 0 off) of a standing turret is replaced by the nearest visible floor cell within the 400 uu reach that is at least 100 uu from every turret; otherwise placed as aimed. No test changed. Build 0, 195/195. Commit cbe2a67.

## Subtasks
1. task-051-01-turret-spacing — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-051-01-turret-spacing]] — step 1, cpp.
