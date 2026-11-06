---
id: task-456-03-doc
description: "Doc: Docs describe the ghost."
parent: task-456-xp-gain-ghost
kind: task
status: done
---
# XP gain ghost on the XP bar: doc

- goal: Docs describe the ghost.
- agent: doc
- depends_on: task-456-02-ghost-bar
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md)
- expected_output: see done_when
- done_when: Behaviour and values recorded; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-456
- usage: included in the executor run of task-456
- runs: 1
- milestone: M2
- started: 2026-11-06T01:10:36Z
- waiting_for: none
- result: Done (commit cea4a039). New sys-hud-xp-bar (child of sys-hud): URRXPStatusWidget rules, event-armed update, values CatchUpDelay 0.4 s, CatchUpRate 7/s, snap 0.001, GhostColor #c8f6ff (provisional, Q32), test RogueRigs.HUD.XPGhost. wbp-xp-status updated: parent class, XPGhostBar row in the anchors table, XPBar background alpha 0, draw order ghost/fill/ticks, ShowXP level text only, verified task-456-02. Lint clean (vault OK, no problem).

## Relations
- [[task-456-xp-gain-ghost]] — parent global task.
