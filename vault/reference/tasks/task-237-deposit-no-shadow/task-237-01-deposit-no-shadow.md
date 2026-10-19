---
id: task-237-01-deposit-no-shadow
description: "Ore and heal deposit formations cast no shadow."
parent: task-237-deposit-no-shadow
kind: task
status: done
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
- model: opus
- tokens: input 86, cache write 211650, cache read 1440594, output 747
- runs: 3
- milestone: M2
- result: Commits 1513608e, 60a571ca, 5d8e7471, 297de7ee. bDepositCrystalsCastShadow (default false) turns off cast, dynamic, static and contact shadows on deposit formation ISMs and the cluster fallback. RockDamageData made aware of heal deposit formations (broken by 234-04) and checks CastShadow false. Build 0; Dig 13/13; full run 262/264 (2 known). Look checked by the owner.

## Relations
- [[task-237-deposit-no-shadow]] — parent global task.
