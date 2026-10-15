---
id: task-176-lone-stat-icon-larger
description: "Global task: stat icon 25 % larger (156 px) when shown alone on a card; unchanged next to a weapon icon."
parent: tasks-done-index
kind: task
status: done
---
# Upgrade cards: lone stat icon larger

- request: Tu peux encore agrandir de 25% l'icone seule; cela vaut pour les icones de stats, lorsqu'il y a une arme, l'icone de stat n'est pas seule (owner, 2026-10-15)
- goal: Make the stat icon 25 % larger (125 to 156 px) on cards where it is shown alone (All weapons or player stat); unchanged next to a weapon icon.
- acceptance: Compiles, saved, sizes read back for both cases, doc updated, committed. Owner checks on screen; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-15).
- confirmed_by_owner: yes (2026-10-15)
- milestone: M2
- next_step: none
- result: Done. StyleCard sets StatIcon desired size 156 x 156 when shown alone (Player, All weapons) and 100 x 100 next to a weapon icon; StatIconBox overrides off. Compiled with warnings as errors, saved, read back, doc updated. Not seen on screen. Commit 4d4c472.

## Subtasks
1. task-176-01-lone-stat-icon-larger — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-176-01-lone-stat-icon-larger]] — step 1, bp.
