---
id: handover-2026-10-12-gameplay-hud
description: "State after the gameplay, HUD, pool and ground-circle session of 2026-10-07 to 10-12: all validated by the owner, open owner decisions, how to resume."
parent: production-index
kind: note
---
# Handover — gameplay, HUD and performance session (to 2026-10-12)

Scope: the Orchestrator session that ran tasks 070 to 071, 080 to 100, 103 to 105 and 109 to 115, plus Assignment 4 (tasks 072 to 087). The art and barrel session has its own note: [[handover-2026-10-12]]. Nothing is running, the editor is closed, every task of this session is `done` ([[tasks-index]]). Last full headless run: 261 of 261 `RogueRigs.*` tests (after the last Blueprint change). Build "up to date" means only assets changed: the tests do not check Blueprints.

## Built, now validated on screen
On 2026-10-13 the owner validated everything listed in [[validation-checklist-2026-10-12]] (HUD, pools, gameplay, zones, barrel, heavy ground effect, big XP block). Nothing is left to validate; the checklist stays as the record of what was checked.

## Open owner decisions
- Subtitle queue: `extraction_ready` is replaced by `shuttle_countdown` in the same frame, so its line is never seen. Proposal: a queue of one pending line.
- One 700 uu barrel explosion costs about 8 ms of game thread (budget 5 ms): spread the block destruction over frames, or accept ([[sys-grid]]).
- Reused robots still cost about 1 ms each (reset and reactivation): Insights capture, a per-frame reuse cap, or both ([[sys-actor-pool]]).
- Cold start: the first mass kill creates cubes for 3 frames (about 60 ms); the prewarm of task-111 covers 30 cubes only ([[sys-spawner]]).
- Danger circles (barrel, heavy) are dotted like zone circles: confirm the look once seen ([[ground-telegraph]]).
- Task 069 (minimum specification) is cancelled by the owner: it comes at the end of the project.
- Projectile pooling: not now ([[perf-projectile-pool-evaluation]]); revisit at 15 projectiles in one frame.

## Task numbering collisions
Two sessions created numbers in parallel. 106 is still both `weapons-ui-size` and `wear-mask-texture` (the second was in progress during the vault pass, so it was not renamed). The old 108 `barrel-on-floor` is now `task-117-barrel-on-floor`. Run `ls vault/reference/tasks` right before creating a task.

## Things to know
- Dates: nodes created after midnight (machine time) read 2026-10-13 (tasks 110 to 115 and later): same session.
- The owner's two weapon UI captures live in `Reference/DRGS/` (git-ignored, local only): `ui-weapons-current-2026-10-12.webp`, `ui-weapons-reference-2026-10-12.webp`.
- The HUD z-order: cards 10, alerts 8, minimap 6, the other panels 0 ([[wbp-hud]]).

## How to resume
Read [[tasks-to-do-index]] (empty), this node, the checklist, then [[orchestrator-lessons-2026-10-12]]. Assignment 4 is described in [[assignment-4-content-pipeline]]; its README is submitted-ready.

## Relations
- [[production-index]] — parent index.
- [[handover-2026-10-12]] — the art and barrel session of the same day.
- [[validation-checklist-2026-10-12]] — what to look at on screen, per area.
- [[orchestrator-lessons-2026-10-12]] — owner rules and agent lessons of this session.
- [[hud-refactor-audit]] — the HUD split, its structure and events.
- [[ground-telegraph]] — the ground circles used by zones, barrel and heavy.
- [[sys-actor-pool]] — pools, prewarm and the reuse cost.
- [[sys-spawner]] — the prewarm rule and the wave data it reads.
- [[placeholder-weapon-icons]] — the temporary icons and how to replace them.
- [[perf-projectile-pool-evaluation]] — why projectiles are not pooled yet.
- [[assignment-4-content-pipeline]] — Assignment 4 brief and status.
- [[tasks-index]] — tasks and status.
