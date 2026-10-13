---
id: sys-flow-field-updates
description: "Flow field update paths: full rebuild, opened cells, goal change, worker thread, fallback budget. Open to change when the field is rebuilt."
parent: sys-flow-field
kind: doc
---
# Flow field — updates

  - full rebuild, synchronous: first computation, grid or goal actor changed (`SetGrid`, `SetGoalActor`, size), `MarkDirty` (fallback); at most once per frame, in Tick and lazily in every query. Drops queued work.
  - opened cell (`OnCellDestroyed`): queued; Tick (or the worker job) marks it walkable, gives it its best neighbour + 1 and pushes it and its 8 neighbours (new edges, unblocked diagonals) on a min-heap; decrease-only relaxation, lowest distance first, directions re-chosen on pop and on equal-distance ties, so the result equals a full rebuild.
  - goal enters a new cell (fallback): BFS into a second buffer, sliced, swapped in when done; cells opened meanwhile are re-relaxed on the new field. Until then queries answer from the previous goal cell; a goal move during a rebuild starts the next one.
  - worker thread (default, task-066-01): one `UE::Tasks` job at a time. Tick polls it (`IsCompleted`, never waits), swaps the finished field in (`Swap`, the old one becomes the spare buffer), then launches the next: openings validated against the grid on the game thread and applied to a copy of the walkable array; goal changed cell: BFS from the new cell, else relaxation on copies of the distances and directions. The worker reads only its job; robots read the live field until the swap. Cells destroyed while a job runs stay queued for the next job. A synchronous full rebuild bumps the generation: the job in flight is dropped. Only `FlushPendingWork` and `Deinitialize` wait for it. Latency: launch frame + swap frame.
  - fallback (`rr.FlowField.Async 0`): the time-sliced passes above and the budget below; a switch applies a finished job first, a switch back hands the open relaxation to the next job.
  - budget (fallback): one Tick pass per frame, openings first, then the goal rebuild; the clock is read every 32 steps and each part always advances, so constant digging cannot starve the goal. Queries never wait.

## Relations
- [[sys-flow-field]] — parent hub: summary and verdict.

