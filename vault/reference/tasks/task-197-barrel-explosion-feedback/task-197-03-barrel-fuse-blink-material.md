---
id: task-197-03-barrel-fuse-blink-material
description: "BP: the barrel body material blinks orange-white faster and faster from the fuse progress custom data float; barrel-asset doc updated."
parent: task-197-barrel-explosion-feedback
kind: task
status: todo
---
# Barrel fuse blink material

- goal: the barrel body material (`M_BarrelPaint` used by `MI_Barrel_body`, slot 0 overridden by the grid) reads the fuse progress per-instance custom data float from task-197-01 and adds an orange-white emissive blink whose rate rises from slow to fast as progress goes from 0 to 1 (provisional rates written in the doc); at 0 the barrel looks exactly as before; other `MI_Barrel_*` instances unchanged in look; `barrel-asset` doc updated.
- agent: bp
- depends_on: task-197-01-barrel-fuse-instability
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), barrel-asset (vault/reference/tech/pipelines/barrel-asset.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: material asset changes saved by explicit path; doc node; one commit.
- done_when: material compiles; custom data index matches `sys-grid-barrels`; in PIE a fuse lit through the BlueprintCallable `ARRArenaGrid::DestroyCell` on a barrel cell and the custom data value read back rises 0 to 1; look at progress 0 unchanged (parameters read back); doc updated; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
