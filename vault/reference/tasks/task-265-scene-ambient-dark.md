---
id: task-265-scene-ambient-dark
description: "Global task: A dark violet ambient with the sun lowered, so the player light is the main light and bright bodies are no longer burnt."
parent: tasks-done-index
kind: task
status: done
---
# Dark violet ambient

- request: lighting like the reference: dark purple ambient, warm light around the player; the yellow robot body is burnt by the light (owner, 2026-10-21)
- goal: A dark violet ambient with the sun lowered, so the player light is the main light and bright bodies are no longer burnt.
- acceptance: Lights read back with the new values and the previous ones recorded for revert; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. DirectionalLight_0 intensity 6 -> 1.0 with colour (0.75, 0.62, 1.0); SkyLight_0 intensity 0.3 -> 0.5 with colour (0.35, 0.25, 0.6); castShadows stay false; no exposure, bloom, Lumen or material change; node lights-dark-ambient with previous values for revert. Risks: default auto exposure may brighten the dark scene back; the sky light (captured scene, real-time capture off, no cubemap) may contribute nothing until recaptured. Saved with the documented empty save_assets route (only the two light files changed). Not seen on screen.

## Subtasks
1. task-265-01-scene-ambient-dark — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-265-01-scene-ambient-dark]] — step 1, bp.
