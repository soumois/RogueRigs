---
id: task-230-17-rock-sag-visible-check
description: "Show broken pieces sagging on a partly damaged wall and recapture the 15 and 16 fixes in game."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Show broken pieces sagging on a partly damaged wall and recapture the 15 and 16 fixes in game

- goal: a -game capture shows broken pieces on cells damaged but not destroyed, sagging toward the void (owner 2026-10-18, after the 230-12 check: no sag visible in 230-12), plus captures of Duskite crystals and of a rock mass top after 15 and 16. If the sag is not visible, find why (swap, custom data, material) and fix the C++ part or report the material change needed.
- agent: cpp
- depends_on: task-230-16-rock-top-texture-shade
- inputs: vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md, vault/reference/tech/systems/world/sys-grid/sys-grid-rock-piece-data.md, vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/tech/performance/perf-rock-slab.md (capture spots), vault/reference/agents/agent-cpp.md (BugItGo then Walk)
- expected_output: captures in Saved/RockSlab/after2/ (close view of a wall with cells at about 30 % and 70 % damage, Duskite, a mass top), side by side with 230-12; fix if needed.
- done_when: captures sent to the owner; any fix built and tested (2 known pre-existing failures); committed.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 150, cache write 153302, cache read 6796677, output 4993
- runs: 2
- milestone: M2
- result: Commits b0166d9b, cebd8e3a, faf75dd3. Sag: swap and data correct, but the M_RockSlab WPO is not drawn on the Nanite path in game; broken pieces now off Nanite (bBrokenRockPiecesUseNanite false, provisional, owner to confirm), sag visible (Saved/RockSlab/after2/rs_s_sag_*). Debug commands rr.Debug.DamageCell, rr.Debug.Camera. Build 0, tests 262/264 (2 known). Still not fixed: Duskite grey with MI_OreChunk_Bright assigned; rim band, flat top and no crack lines unchanged by 230-16.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
