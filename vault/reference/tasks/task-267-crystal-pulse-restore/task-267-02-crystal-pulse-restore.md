---
id: task-267-02-crystal-pulse-restore
description: "M_DepositCrystalGlow reads the new pulse indices 13 and 14."
parent: task-267-crystal-pulse-restore
kind: task
status: in-progress
---
# Deposit crystal pulse indices

- goal: M_DepositCrystalGlow reads the new pulse indices 13 and 14.
- agent: bp
- depends_on: task-267-01-crystal-pulse-restore
- inputs: bp-deposit-crystal-material (vault/reference/tech/blueprints/fx/bp-deposit-crystal-material.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: M_DepositCrystalGlow: PerInstanceCustomData indices 4 and 5 become 13 and 14, the Custom Primitive Data parameters PulsePhase and PulseRate are removed from this material (per-instance data only), neutral defaults kept; the fix of the cause found in 267-01 for M_XPNutGlow if it concerns it (pickups and XP nut must keep pulsing); compile, read-back, docs.
- done_when: Compile OK; the node graph read back; the deposit instances still reference the material; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-267-crystal-pulse-restore]] — parent global task.
