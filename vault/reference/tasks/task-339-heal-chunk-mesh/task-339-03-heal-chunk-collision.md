---
id: task-339-03-heal-chunk-collision
description: "cpp, editor closed: one simple collision primitive per heal chunk variant with pickup_simple_collision.py (best fit), so the Chaos rest and roll fit the shape."
parent: task-339-heal-chunk-mesh
kind: task
status: todo
---
# Heal pickup chunks: simple collision

- goal: the pickup body simulates with the mesh simple collision ([[sys-pickups-lifecycle]] physics); give SM_HealChunk_01..03 one primitive each with `Tools/unreal/pickup_simple_collision.py` in `best` mode (the ore chunk rule of task-229-01: oriented box when at least 15 percent smaller than the AABB, convex hull when the box is more than 2 times the hull volume), headless, editor closed: `/Game/RogueRigs/Art/HealChunk/SM_HealChunk_01:best`, `_02:best`, `_03:best`. Run `--analyse` first only if the first run fails. No code change.
- agent: cpp
- depends_on: task-339-02-heal-chunk-import
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md) (simple_collision line: the precedent), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md); `Tools/unreal/pickup_simple_collision.py` (usage in its header)
- expected_output: the three meshes saved with one primitive each; change summary for the doc subtask: the three `PICKUPCOLLISION ... ok` lines (choice, centre, rotation, size or hull bounds and volume). No doc node written (doc subtask).
- done_when: three `PICKUPCOLLISION <path> ok` lines, each with exactly one primitive read back; no `Python script executed with errors`; committed by path (the three .uasset files).
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-339-heal-chunk-mesh]] — parent global task.
