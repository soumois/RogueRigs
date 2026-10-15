---
id: task-177-wider-upgrade-cards
description: "Global task: Make the upgrade cards wider: 500 x 720 instead of 440 x 720, frames re-baked at the new size, same frame thickness."
parent: tasks-done-index
kind: task
status: done
---
# Upgrade cards: wider cards

- request: Tu peux prendre un peu plus sur la largeur pour que les cartes soient un peu plus large (owner, 2026-10-15)
- goal: Make the upgrade cards wider: 500 x 720 instead of 440 x 720, frames re-baked at the new size, same frame thickness.
- acceptance: Four PNGs 1000 x 1440, band thickness unchanged, insets reported, committed. No Kie credits; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-15).
- confirmed_by_owner: yes (2026-10-15)
- milestone: M2
- next_step: none
- result: Done. Stat, accents and weapon baked textures re-imported at 1000 x 1440 (brushes re-assigned); frame images 500 x 720; CardSize 512 x 732 (40 px gap kept, row 1616 px of 1920); CardPanel 14/24/14/25 (shared by both card kinds), CardBox 38/58/38/60, WeaponBody 38/56/38/44, ImageArea 408 x 320, WeaponImgBox 408 x 290. Glow texture left at 880 x 1440 until 178-02. Compiled with warnings as errors, saved, read back, doc updated. Not seen on screen. Commit b2e7062.

## Subtasks
1. task-177-01-wider-upgrade-cards — art
2. task-177-02-wider-cards-layout — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-177-01-wider-upgrade-cards]] — step 1, art.
- [[task-177-02-wider-cards-layout]] — step 2, bp.
