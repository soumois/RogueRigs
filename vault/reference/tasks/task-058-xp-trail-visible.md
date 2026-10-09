---
id: task-058-xp-trail-visible
description: "Global task: The XP cube Niagara trail is visible in game (the sparkle already is)."
parent: tasks-index
kind: task
status: done
---
# Visible XP trail

- request: "je vois les étincelles lorsque l’xp arrive au joueur, mais pas la traînée" (owner, 2026-10-07)
- goal: The XP cube Niagara trail is visible in game (the sparkle already is).
- acceptance: PIE check with no error; docs updated; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Cause: the NS_XPTrail ribbon emitter had no spawn module, so it spawned no particles (TrailFX active but empty). Fix: engine SpawnRate module at 60 per second; world space, Screen facing, cyan, width 10, lifetime 0.35 s kept. Compiled clean; PIE no Niagara error; particle count not readable through MCP, visual check left to the owner. Commits b90ab87, 244b84f.

## Subtasks
1. task-058-01-xp-trail-visible — bp

## Relations
- [[tasks-index]] — parent index.
- [[task-058-01-xp-trail-visible]] — step 1, bp.
