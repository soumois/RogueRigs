---
id: task-227-01-shield-debris-script-fix
description: "3D agent: shield_debris_pieces.py picks its cut-cap texel from the plate colour instead of asserting yellow; re-run and compare outputs."
parent: task-227-shield-debris-script-fix
kind: task
status: done
---
# Shield debris script fix

- goal: the cut script works with any plate colour.
- agent: agent-3d (Blender, no Meshy)
- depends_on: none
- inputs: `Tools/art/blender/shield_debris_pieces.py`, `Tools/art/blender/shield_v2.py` (plate colour constant #33363B), `Art/Source/shield-chaser/debris/` (current FBX, json, texture copies), `vault/reference/agents/art-3d-lessons.md`.
- method: replace the yellow assertion by a search for a uniform texel of the plate colour (read the plate colour from shield_v2.py or a shared constant, not a hard-coded hue), keep the same texel if it already qualifies. Re-run headless; compare the five FBX and the json with the committed ones (geometry, UVs, slots, offsets): only the cap UV may change; report any other difference. Do not touch Unreal.
- done_when: script runs; comparison reported; commit by path (script, and the outputs only if they changed).
- editor_state: any (no Unreal)
- token_ceiling: open
- model: sonnet
- tokens: input 20, cache write 32137, cache read 215498, output 1868
- runs: 1
- milestone: M1
- result: Commit 8e6e3977: shield_debris_pieces.py reads PLATE_BASE (#33363B) from shield_v2.py and picks a uniform plate-colour texel for the cut caps (same search order); re-run gives byte-identical json and FBX equal in geometry, UVs and slots (only header bytes differ), so outputs were not re-committed.

## Relations
- [[task-227-shield-debris-script-fix]] — parent global task.
