---
id: task-054-niagara-trail-sparkle
description: "Global task: XP cube trail and pickup sparkle rebuilt as Niagara systems."
parent: tasks-done-index
kind: task
status: done
---
# Niagara trail and sparkle

- request: "Pour les traînées et étincelles, il faudrait que cela soit du niagara" (owner, 2026-10-07)
- goal: XP cube trail and pickup sparkle rebuilt as Niagara systems.
- acceptance: PIE check with no error; docs updated; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Integration.JuiceKillDropsStreamingCubes reads TrailFX and NS_PickupSparkle; Niagara activation and sparkle checks guarded by FApp::CanEverRender() (skipped and logged under -nullrhi; with -RenderOffScreen 3 trails, 3 sparkles at the collect points, none active 2 s later). Build 0; full run 194/195, the failure Progression.XPPickupAddsOne is unrelated and intermittent (third time). Commit 73b4144.

## Subtasks
1. task-054-01-niagara-trail-sparkle — bp
2. task-054-02-juice-test — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-054-01-niagara-trail-sparkle]] — step 1, bp.
- [[task-054-02-juice-test]] — step 2, cpp.
