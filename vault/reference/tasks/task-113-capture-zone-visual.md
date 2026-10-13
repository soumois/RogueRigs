---
id: task-113-capture-zone-visual
description: "Global task: The console and chest hack zones show a sharp zone circle and a growing progress circle."
parent: tasks-done-index
kind: task
status: done
---
# Capture zone visual (C++ side)

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: The console and chest hack zones show a sharp zone circle and a growing progress circle; the old diffuse circle is gone; the rules are unchanged.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. HackRing (engine Plane) and its SetRelativeScale3D call removed from BP_ObjectiveConsole and BP_BonusChest; glow calls kept; MI_GroundRingTeal and MI_GroundRingGold have no referencers (left in place); not seen in PIE. Commits 75d1513, 7c24452, f983540.

## Subtasks
1. task-113-01-capture-zone-visual — cpp
2. task-113-02-capture-zone-visual — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-113-01-capture-zone-visual]] — step 1, cpp.
- [[task-113-02-capture-zone-visual]] — step 2, bp.
