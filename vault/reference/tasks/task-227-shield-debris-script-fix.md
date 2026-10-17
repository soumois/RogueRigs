---
id: task-227-shield-debris-script-fix
description: "Make shield_debris_pieces.py run again on the charcoal shield texture (its cut-cap texel search asserted a yellow texel)."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shield debris cut script: charcoal texture

- request: "Update le script de découpe" (owner 2026-10-17, after task-226: the script asserts a yellow cap texel and would fail on the charcoal texture).
- goal: `Tools/art/blender/shield_debris_pieces.py` runs on the current shield texture and produces the same five pieces, with cut caps in the plate colour (charcoal).
- acceptance: the script runs; pieces and json unchanged except the cap UV if needed; commit.
- gdd_check: [[art-prompts-shield]] (charcoal plate #33363B since task-226).
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-227-01-shield-debris-script-fix
- result: open

## Subtasks
- [[task-227-01-shield-debris-script-fix]] — 3D agent: fix the cap texel search, re-run, compare.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-222-shield-break-debris]] — the pieces it rebuilds.
