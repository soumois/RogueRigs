---
id: task-237-01-deposit-no-shadow
description: "Ore and heal deposit formations cast no shadow."
parent: task-237-deposit-no-shadow
kind: task
status: todo
---
# Deposit formations without shadows

- goal: Ore and heal deposit formations cast no shadow.
- agent: cpp
- depends_on: none
- inputs: sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: ARRArenaGrid: a UPROPERTY bDepositCrystalsCastShadow (default false) applied to the deposit formation ISMs (Duskite and heal) and to the per-cell crystal cluster fallback; docs updated; no capture (owner checks).
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; committed.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-237-deposit-no-shadow]] — parent global task.
