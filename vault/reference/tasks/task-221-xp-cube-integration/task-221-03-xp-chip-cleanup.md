---
id: task-221-03-xp-chip-cleanup
description: "bp: delete SM_XPChipBig and the M_XPChip_* materials once nothing references them, delete the chip FBX and Blender scripts, fix docs."
parent: task-221-xp-cube-integration
kind: task
status: done
---
# XP chip cleanup

- request: "et clean celle-ci lorsque c'est fait" (owner 2026-10-17: explicit request to delete the chip).
- goal: no chip asset or source file left; nothing broken.
- agent: agent-bp
- depends_on: task-221-02-bp-pickup-xp-big-cube
- inputs: `vault/reference/tech/blueprints/fx/bp-pickup-xp.md`, `vault/reference/tech/pipelines/tool-unreal-reimport.md` (its test section names SM_XPChipBig), `vault/reference/gdd/art-audio/art-prompts-xp-core.md`, `vault/reference/agents/bp-mcp-lessons.md`.
- method: 1) In Unreal, list the referencers of `/Game/RogueRigs/Art/XPChip/SM_XPChipBig` and of each `M_XPChip_*` (board, casing, casing_chamfer, edge_band, core_bright, core_dark, legs); delete only those with no referencer left (report any other); delete the empty folder. 2) Repo: `git rm` `Art/Meshes/xp-chip-big-mesh/`, `Tools/art/blender/xp_chip_big_mesh.py`, `Tools/art/blender/xp_chip_big_preview.py`, and the chip's source folder `Art/Source/xp-chip/` only if nothing else uses it (check). 3) Docs: in the nodes above, replace current-state mentions of the chip by the cube, note the deletion (date, task); history in done task nodes stays as is.
- done_when: referencer lists empty before each delete; folder gone; git shows the removals; lint clean.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: sonnet
- tokens: input 36, cache write 34329, cache read 427657, output 978
- runs: 1
- milestone: M1
- result: Commit 60766d74: referencers checked empty, then deleted SM_XPChipBig, the seven M_XPChip_* and the folder; git rm Art/Meshes/xp-chip-big-mesh/, xp_chip_big_mesh.py, xp_chip_big_preview.py, Art/Source/xp-chip/; docs bp-pickup-xp, tool-unreal-reimport, art-prompts-xp-core updated. Kept: vault/assets/xp-chip-big.png (prompt input image); two stale comments in xp_cube_big_mesh.py.

## Relations
- [[task-221-xp-cube-integration]] — parent global task.
