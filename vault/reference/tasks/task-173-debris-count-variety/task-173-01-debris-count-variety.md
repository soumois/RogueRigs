---
id: task-173-01-debris-count-variety
description: "Throw 2 to 4 debris meshes per death with equal odds and check with a game run that the pieces drawn vary, fixing any bias."
parent: task-173-debris-count-variety
kind: task
status: done
---
# Robot debris: 2 to 4 pieces and variety

- goal: Throw 2 to 4 debris meshes per death with equal odds and check with a game run that the pieces drawn vary, fixing any bias.
- agent: cpp
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: ARREnemy::LaunchDebris draws the count in {2, 3, 4} with equal odds (FRRDebrisSelection::Choose handles 4: distinct groups, cut limb counts 2, never above the count); the Niagara pool and spawn burst accept 4 entries; a -game run with a temporary or Verbose log of the chosen mesh indices over at least 50 deaths gives the frequency of each of the 13 meshes and of each count, and the Niagara particle MeshIndex matches the requested one (fx.Niagara.DumpComponents); any bias found is fixed and the run repeated; sys-enemies-base updated
- done_when: Build passes with the editor closed; existing enemy tests still pass except the known EyeMesh one; the frequency table of the 13 meshes and of the counts is reported (values, no screenshots); doc updated; committed by path
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 116, cache write 333533, cache read 4096459, output 40652
- runs: 1
- milestone: M3
- result: Count RandRange(2,4); Choose redraws when a cut would exceed the count (was: forced whole, cuts 35 %, now 42 %); Verbose log of chosen indices; 732 deaths: torso 44 %, halves 17-21 %, wholes 24-28 %, counts 232/248/252; Niagara MeshIndex matches requests (207 components); torso frequency follows equal group odds (owner call); build OK, 15/16 tests (known EyeMesh); commit d6ce98a

## Relations
- [[task-173-debris-count-variety]] — parent global task.
