---
id: task-456-01-ghost-logic
description: "Cpp: The XP bar drives a ghost fill and a delayed catch-up from XP events."
parent: task-456-xp-gain-ghost
kind: task
status: done
---
# XP gain ghost on the XP bar: cpp

- goal: The XP bar drives a ghost fill and a delayed catch-up from XP events.
- agent: cpp
- depends_on: none
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-progression (vault/reference/tech/systems/core/sys-progression.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: see done_when
- done_when: Build ok; test as in A5 passes; HUD and Progression test families pass, figures in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-456
- usage: included in the executor run of task-456
- runs: 1
- milestone: M2
- started: 2026-11-06T01:01:19Z
- waiting_for: none
- result: Done (commit 85013287). New URRXPStatusWidget (Source/RogueRigs/UI/RRXPStatusWidget.h/.cpp, Abstract, future parent of WBP_XPStatus): binds progression OnXPChanged and OnLevelUp natively; BindWidgetOptional XPBar (fill) and XPGhostBar (ghost, behind); on a gain ghost = new fraction at once, fill waits CatchUpDelay 0.4 s (restarted by each gain) then Fill += (Ghost-Fill)*min(1, CatchUpRate 7*dt); level-up: ghost 1, fill catches up, bar empties, ghost = remainder, fill after the delay; several levels = one pass; level change without OnLevelUp (restore) snaps; GhostColor #c8f6ff; next-tick timer armed by events, stopped when settled. Build ok; RogueRigs.HUD.XPGhost added and passes; RogueRigs.HUD (15) + RogueRigs.Progression (4) = 19/19 success, exit 0.

## Relations
- [[task-456-xp-gain-ghost]] — parent global task.
