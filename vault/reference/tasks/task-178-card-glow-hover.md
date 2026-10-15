---
id: task-178-card-glow-hover
description: "Global task: Stronger inner rarity glow on the upgrade cards (about 2x), and stronger still on hover (about 3x), driven by the button hover events."
parent: tasks-done-index
kind: task
status: done
---
# Upgrade cards: stronger glow and hover

- request: Il faudrait augmenter la lueur intérieur et lorsqu'on passe sur la carte encore d'avantage (owner, 2026-10-15)
- goal: Stronger inner rarity glow on the upgrade cards (about 2x), and stronger still on hover (about 3x), driven by the button hover events.
- acceptance: Glow PNG regenerated at the card size, control sheet shown to the owner, committed. No Kie credits; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-15).
- confirmed_by_owner: yes (2026-10-15)
- milestone: M2
- next_step: none
- result: Done. New glow re-imported (1000 x 1440), FrameGlow0..2 brushes re-assigned; RenderOpacity 0.6 at rest; Construct binds OnHovered / OnUnhovered of CardButton0..2 to events setting FrameGlow<i> opacity 1.0 / 0.6 (no tick); OnClicked handlers rebuilt (OnClicked_Event_7.._10, indices 0/1/2 and reroll fixed with set_pin_value; strays deleted). Compiled, saved, read back, doc updated. Not seen on screen. Commit 9de50ed.

## Subtasks
1. task-178-01-card-glow-hover — art
2. task-178-02-glow-hover-layout — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-178-01-card-glow-hover]] — step 1, art.
- [[task-178-02-glow-hover-layout]] — step 2, bp.
