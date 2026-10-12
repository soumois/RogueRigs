---
id: task-106-wear-mask-texture
description: "Global task: One shared grayscale wear mask texture exists, tileable, ready to be multiplied on flat colours for edge wear, scratches and light grime."
parent: tasks-done-index
kind: task
status: done
---
# Shared wear mask texture

- request: niveau 1 de la charte: détails par une texture de masque d'usure partagée multipliée sur les couleurs plates (owner 2026-10-11, après le baril dont l'usure en géométrie ne convenait pas) (owner, 2026-10-12)
- goal: One shared grayscale wear mask texture exists, tileable, ready to be multiplied on flat colours for edge wear, scratches and light grime.
- acceptance: T_WearMask.png is 1024 x 1024 grayscale, tileable (numeric edge check in the report), hard-edged patchy look (described after opening it), deterministic script; the report exists; nothing applied to an asset; committed by name; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: none
- result: Done. Mask rewritten as scratch lines only: 37 lines (26 isolated, 3 clusters), 1.44 % dark, tileable (edge diffs 0.0018 and 0.0047), seed 108; commits 62db100, 81e0b80. Shared wear mask delivered; applied to the barrel in task-101-06.

## Subtasks
1. task-106-01-wear-mask-texture — 3d (done)
2. task-106-02-scratch-mask — 3d

## Relations
- [[tasks-done-index]] — parent index.
- [[task-106-01-wear-mask-texture]] — step 1, 3d.
- [[task-106-02-scratch-mask]] — step 2, 3d.
