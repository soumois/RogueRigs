---
id: task-230-13-rock-collision-fix
description: "Fix the regression reported by the owner after 230-11: the player walks through rock walls without digging; restore block collision."
parent: task-230-rock-slab-integration
kind: task
status: todo
---
# Rock wall collision regression

- goal: rock (and ore, heal ore, special ore) cells block the player again exactly as before task-230 (owner 2026-10-18: "les murs n'ont plus de collision, je peux passer à travers sans devoir attendre le minage", seen in the 230-12 capture game on L_ArenaM1); gameplay unchanged is an acceptance of [[task-230-rock-slab-integration]].
- agent: cpp
- depends_on: task-230-11-rock-slab-assign
- inputs: vault/reference/tech/systems/world/sys-grid/sys-grid-rock-pieces.md, vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md, vault/reference/tech/blueprints/world/bp-arena-grid.md, vault/reference/tech/systems/tools/sys-automation-tests.md; code Source/RogueRigs/Grid/; changes since e9e1f93e (commits of 230-07, 230-08, 230-11 on BP_ArenaGrid)
- expected_output: cause found with evidence from a real run (the collision ISM chunk components of BP_ArenaGrid on L_ArenaM1: instance counts, mesh, collision enabled, physics bodies), fix in C++ (or the exact BP property to restore, reported for the bp agent); a -game check on L_ArenaM1 showing the player is blocked by rock again (no new automation test: owner rule); existing tests updated only if the fix breaks them; docs updated.
- done_when: build exits 0 editor closed; -game check shows the player blocked by rock; all RogueRigs.* tests pass except the 2 known pre-existing failures; committed.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
