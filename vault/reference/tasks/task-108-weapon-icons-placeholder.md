---
id: task-108-weapon-icons-placeholder
description: "Global task: placeholder padlock and weapon icons: source images validated by the owner, then transparent 256 px PNGs."
parent: tasks-done-index
kind: task
status: done
---
# Placeholder weapon and lock icons

- request: "demande à l'agent AI de fournir une image temporaire de locker et des armes (placeholder jusqu'à ce qu'on détermine les armes)" (owner 2026-10-12) (owner, 2026-10-12)
- goal: Placeholder UI icons (padlock and the three current weapons) exist as source images, validated by the owner, then prepared as transparent 256 x 256 PNGs.
- acceptance: candidates for the padlock and the three weapons exist with their run logs; self-review and style-guide review done; the owner validates; lint clean; committed (Art/Source is tracked); committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: none
- result: Done. Tools/art/ui_icon_prepare.js (dependency-free, distance key plus magenta key, despill, trim, 256 px, 10 % margin) and four transparent 256 x 256 icons in Art/UI/Icons; checks pass (corners transparent, no magenta residue, shotgun halo 293 -> 0 pink pixels). Commit f3e43d0.

## Subtasks
1. task-108-01-weapon-icons-placeholder — art
2. task-108-02-weapon-icons-placeholder — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-108-01-weapon-icons-placeholder]] — step 1, art.
- [[task-108-02-weapon-icons-placeholder]] — step 2, cpp.
