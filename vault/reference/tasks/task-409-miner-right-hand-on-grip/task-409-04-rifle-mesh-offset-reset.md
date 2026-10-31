---
id: task-409-04-rifle-mesh-offset-reset
description: "Bp: BP_BurstRifle: RifleMesh location (0, 0, 0), scale 1.25 kept, Muzzle (52.25, 0, 0)"
parent: task-409-miner-right-hand-on-grip
kind: task
status: in-progress
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it: bp

- goal: BP_BurstRifle RifleMesh relative location (0, 0, 0), rotation (0, 0, 0), scale (1.25, 1.25, 1.25) kept; Muzzle relative location (52.25, 0, 0) (scaled barrel tip, visual only).
- agent: bp
- depends_on: task-409-02-upper-rifle-v2-grip
- inputs: bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), task-404-01-rifle-mesh-transform (vault/reference/tasks/task-404-rifle-mesh-owner-offset/task-404-01-rifle-mesh-transform.md), Tools/unreal/mcp-tool-sheet.md
- expected_output: Change summary: read-back values, compile result, commit hash. No vault doc edit.
- done_when: RifleMesh reads back location (0, 0, 0), rotation (0, 0, 0), scale (1.25, 1.25, 1.25); Muzzle reads back (52.25, 0, 0); BP compiled without error and saved; committed by path `task-409-04-rifle-mesh-offset-reset: ...`.
- editor_state: open
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T19:44:52Z
- waiting_for: none
- result: open

## Relations
- [[task-409-miner-right-hand-on-grip]] — parent global task.
