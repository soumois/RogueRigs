---
id: task-413-02-doc-bullet-first-frame
description: "Doc: sys-projectiles, sys-weapons (first drawn frame at the Muzzle, cause and fix, debug log or cvar)"
parent: task-413-bullet-shown-at-muzzle
kind: task
status: done
---
# Bullet shown at the muzzle on its first frame: doc

- goal: Update the doc nodes for the task-413-01 change: where the bullet is first drawn, the cause found, the fix, and any debug log or cvar left; facts only from the commit and the cpp change summary.
- agent: doc
- depends_on: task-413-01-bullet-first-frame-at-muzzle
- inputs: task-413-bullet-shown-at-muzzle (vault/reference/tasks/task-413-bullet-shown-at-muzzle.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Updated doc nodes; lint clean; one commit `task-413-02-doc-bullet-first-frame: <summary>` by path.
- done_when: sys-projectiles (and sys-weapons if the fire path changed) state the first-drawn-frame rule, cause, fix and commit hash; `node vault/scripts/lint.js --write` clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: 1
- milestone: M2
- started: 2026-10-31T22:15:58Z
- waiting_for: none
- result: Commit b023819a: sys-projectiles (First drawn frame rule), sys-weapons, sys-weapons-shotgun updated; lint OK.

## Relations
- [[task-413-bullet-shown-at-muzzle]] — parent global task.
