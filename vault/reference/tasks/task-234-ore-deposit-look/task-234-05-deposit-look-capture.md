---
id: task-234-05-deposit-look-capture
description: "-game captures of Duskite deposits after 234-04, compared with the current clusters."
parent: task-234-ore-deposit-look
kind: task
status: done
---
# -game captures of Duskite deposits after 234-04, compared with the current clusters

- goal: captures of several Duskite deposits (variants and rotations visible) at the game camera and one closer view, side by side with Saved/RockSlab/after4/ spots, plain description.
- agent: cpp
- depends_on: task-234-04-deposit-mesh-assign
- inputs: vault/reference/tech/performance/perf-rock-slab.md (launch lines), vault/reference/agents/agent-cpp.md, vault/reference/tech/systems/core/sys-cheats.md
- expected_output: captures in Saved/OreDeposit/.
- done_when: captures sent to the owner; no game process left.
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 54, cache write 69196, cache read 969246, output 4221
- runs: 1
- milestone: M2
- result: Captures Saved/OreDeposit/ (sbs_od_a_spawn, sbs_od_e_duskite, sbs_od_h_crystals, grid_od_close_e, grid_od_close_h, grid_od_heal23, od_dig_*): one formation per deposit, variants and rotations visible, Duskite violet glow, heal red, no grey material, no floating or mirroring. Seen: Duskite crystals read pale pink-white under the glow; heal heap smaller and lower than its 2 x 2 and smaller than Duskite; heal rocks greyer than the slab; dig shots start mid-dig.

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
