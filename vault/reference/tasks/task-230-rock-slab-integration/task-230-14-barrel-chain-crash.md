---
id: task-230-14-barrel-chain-crash
description: "Fix the 30-barrel chain crash (Lumen assert) and the GPU matrix precision ensure fired on every dig since task-230."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Fix the 30-barrel chain crash (Lumen assert) and the GPU matrix precision ensure fired on every dig since task-230

- goal: the 30-barrel chain stress run and digging run without crash or ensure (owner 2026-10-18, after the 230-12 check): 230-12 crashed with an assert in LumenMeshCards.cpp:1332 after "precision loss while converting matrix to GPU format" (DoubleFloat.cpp:19) and Matrix.h:468; 230-13 saw the same ensure (OriginX <= OriginMax) in FDistanceFieldSceneData::UpdateDistanceFieldObjectBuffers on every rock dig.
- agent: cpp
- depends_on: task-230-13-rock-collision-fix
- inputs: vault/reference/tech/systems/world/sys-grid/sys-grid-rock-pieces.md, vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md, vault/reference/tech/systems/world/sys-grid/sys-grid-rock-piece-data.md, vault/reference/tech/performance/perf-rock-slab.md, vault/reference/tech/performance/perf-stress-grid-cells.md; crash reports Saved/Crashes/UECC-Windows-59FE2BF94B668A10A708EA9F20D75029_0001 and _0002; code Source/RogueRigs/Grid/
- expected_output: cause found with evidence (which instance or transform is invalid), fix in C++, docs updated.
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; a -game dig run logs no precision ensure; the 30-barrel chain run of perf-rock-slab completes without crash, its numbers appended to perf-rock-slab; committed.
- editor_state: closed
- token_ceiling: 100000
- model: opus
- tokens: input 148, cache write 267977, cache read 6937961, output 1571
- runs: 2
- milestone: M2
- result: Commits 6838e0c5 (code), d6aa3b38, 97139f9d (docs, perf). Cause: HISM RemoveInstance leaves an unmapped render slot with an uninitialized transform until the async rebuild; distance field and Lumen read it (ensure, then Lumen assert). Fix: piece visuals are plain ISMs with remove-at-swap. Build 0; tests 262/264 (2 known pre-existing). Dig run 0 ensures; 30-barrel chain run completed, event frames avg 11.6 ms max 17.8, GPU 8.47 ms. Open: cell HISMs (barrels, crystals, floor) still remove the HISM way, no ensure seen.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
