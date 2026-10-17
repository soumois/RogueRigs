---
id: task-227-shield-debris-script-fix
description: "Make shield_debris_pieces.py run again on the charcoal shield texture (its cut-cap texel search asserted a yellow texel)."
parent: tasks-done-index
kind: task
status: done
---
# Shield debris cut script: charcoal texture

- request: "Update le script de découpe" (owner 2026-10-17, after task-226: the script asserts a yellow cap texel and would fail on the charcoal texture).
- goal: `Tools/art/blender/shield_debris_pieces.py` runs on the current shield texture and produces the same five pieces, with cut caps in the plate colour (charcoal).
- acceptance: the script runs; pieces and json unchanged except the cap UV if needed; commit.
- gdd_check: [[art-prompts-shield]] (charcoal plate #33363B since task-226).
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: Done. Commit 8e6e3977: shield_debris_pieces.py reads PLATE_BASE (#33363B) from shield_v2.py and picks a uniform plate-colour texel for the cut caps (same search order); re-run gives byte-identical json and FBX equal in geometry, UVs and slots (only header bytes differ), so outputs were not re-committed.

## Subtasks
- [[task-227-01-shield-debris-script-fix]] — 3D agent: fix the cap texel search, re-run, compare.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-222-shield-break-debris]] — the pieces it rebuilds.
