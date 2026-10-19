---
id: task-234-03-deposit-crystal-visual
description: "The grid draws one crystal formation per deposit (variant and quarter-turn rotation hashed from the deposit), not one cluster per cell."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# The grid draws one crystal formation per deposit (variant and quarter-turn rotation hashed from the deposit), not one cluster per cell

- goal: ARRArenaGrid draws one crystal instance per 2 x 2 deposit at the deposit centre on the slab top (owner 2026-10-19: a deposit is one entity), mesh chosen from a per-ore-type mesh list by a hash of the deposit id, yaw a hashed quarter turn (owner: play with rotation); removed when the deposit breaks; shake moves it like the current crystals; marked and ore colour data kept; when the list of a type is empty (heal until its images exist) the current per-cell clusters stay.
- agent: cpp
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md, vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md, vault/reference/gdd/terrain/ore-blocks.md, vault/reference/tech/blueprints/world/bp-arena-grid.md, vault/reference/agents/agent-cpp.md
- expected_output: UPROPERTY mesh lists per ore type (Duskite, heal), deposit crystal HISM per type, instance per deposit; tests updated; sys-grid docs updated.
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; committed.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
