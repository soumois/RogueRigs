---
id: task-053-weapon-info-centred
description: "Global task: Weapon slot info (name, ammo bar or blocks, reload bar, LOCKED) centred under each slot."
parent: tasks-done-index
kind: task
status: done
---
# Centred weapon info

- request: "Centre les infos sur les armes (munitions, locked, etc.)" (owner, 2026-10-07)
- goal: Weapon slot info (name, ammo bar or blocks, reload bar, LOCKED) centred under each slot.
- acceptance: PIE check with no error; docs updated; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. WBP_HUD: per slot the text (LOCKED included), AmmoBar, ReloadBar and BlockRow share one centre x (76, 246, 416, 586; canvas alignment 0.5); unused blocks collapsed, blocks auto-sized 12 × 10 with 1 px padding, rows auto-sized so visible blocks are centred. PIE: no log error; block rows not seen on screen. Doc wbp-hud. Commits 838e1f2, 01baf1d.

## Subtasks
1. task-053-01-weapon-info-centred — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-053-01-weapon-info-centred]] — step 1, bp.
