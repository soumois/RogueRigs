---
id: task-257-01-pulse-desync
description: "Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate."
parent: task-257-pulse-desync
kind: task
status: todo
---
# Pulse desync material inputs

- goal: Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate.
- agent: bp
- depends_on: none
- inputs: bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: M_XPNutGlow (parent of the ore, heal and deposit crystal instances) gets two extra per-instance inputs, pulse phase (0 to 1 turns) and pulse rate scale (about 0.8 to 1.2), read from Custom Primitive Data for pickups and from per-instance custom data for the deposit instanced meshes; neutral values (phase 0, rate 1) leave XP nut and every current look unchanged; the indices chosen are written in the doc nodes for the C++ subtask.
- done_when: M_XPNutGlow compiles; neutral inputs give the same emissive as before (formula read back); the data indices (not clashing with GlowFadeDataIndex, console mark 0 and ore colour 1-3) documented in bp-pickup-xp and bp-pickup-ore; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-257-pulse-desync]] — parent global task.
