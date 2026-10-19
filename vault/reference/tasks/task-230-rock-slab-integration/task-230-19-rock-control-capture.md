---
id: task-230-19-rock-control-capture
description: "Control capture after 230-18: Duskite, rock top texture, rim, cracks and sag in game, side by side with 230-17."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Control capture after 230-18: Duskite, rock top texture, rim, cracks and sag in game, side by side with 230-17

- goal: -game captures show Duskite violet, readable top texture, no lighter rectangle, crack lines on damaged cells and the sag, compared with Saved/RockSlab/after2/ (owner 2026-10-19).
- agent: cpp
- depends_on: task-230-18-rock-material-diagnosis, task-230-20-rock-vertex-colour-reimport, task-230-21-rock-rim-tone-restore
- inputs: vault/reference/tech/performance/perf-rock-slab.md (capture spots), vault/reference/agents/agent-cpp.md (BugItGo then Walk), vault/reference/tech/systems/core/sys-cheats.md (rr.Debug.DamageCell, rr.Debug.Camera)
- expected_output: captures in Saved/RockSlab/after3/ at the 230-17 spots (a_spawn, e_duskite, h_crystals, sag close and low with the same damage values), side by side with after2, plain description of each.
- done_when: captures sent to the owner; no game process left; committed if docs change.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 58, cache write 69709, cache read 1226233, output 4846
- runs: 1
- milestone: M2
- result: Captures Saved/RockSlab/after3/ (sbs_* against after2). Fixed in game: Duskite glows violet; tops show the T_RockTop grain; no lighter rectangle; crack lines on 0.3 cells; sag visible. Remaining: broken-piece tops flat lavender, untextured (square patches); tops near white instead of lavender; rim band not visible; heal crystals read orange-red; Duskite bloom strong at distance. No code or asset change.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
