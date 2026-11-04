---
id: task-430-03-ore-number-capture
description: "Capture: `-game` capture of the ore number on Duskite collection"
parent: task-430-ore-pickup-number
kind: task
status: done
---
# Ore pickup number on the miner: capture

- goal: check on frames that the ore number shows as decided, with heal and robot numbers unchanged.
- agent: capture
- depends_on: task-430-02-ore-number-cpp
- inputs: agent-capture (vault/reference/agents/agent-capture.md), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), `Art/Source/damage-digits/task-324-heal-captures/` (look of the heal numbers before this task), debug: `rr.Debug.SpawnNear <BP_PickupOre class> 120 0 4` to drop Duskite chunks next to the miner, `rr.Debug.SpawnNear <BP_PickupHeal class> 120 0 1 15` with the miner below full health, `rr.Debug.Shots`
- expected_output: frames and a contact sheet in `Art/Source/damage-digits/task-430-ore-captures/` (keep); report per point seen / not seen / ambiguous with frame paths.
- done_when: checklist reported: C1 collecting a Duskite chunk shows the violet Duskite icon followed by "+1" above the miner; C2 the icon is readable and does not overlap the "+"; C3 "+1" is white; C4 four chunks collected together give four separate stacked numbers; C5 a heal pickup still shows a green "+N" without icon; C6 robot damage numbers still white (critical light orange), no icon. Ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 56, cache write 79473, cache read 744519, output 13062
- usage: turns 27, context start 13k peak 50k, resumes 1 (message x1), 6 min, units 0.24 M
- runs: 1
- milestone: M2
- started: 2026-11-04T21:13:28Z
- waiting_for: none
- result: C1-C5 seen, C6 seen on contact sheet (crit colour unverified); icon violet after bp fix f092bfb7, no OreIconAdvance change; 4th stacked number partly under HUD bar (camera/stack height); frames in Art/Source/damage-digits/task-430-ore-captures/final_*

## Relations
- [[task-430-ore-pickup-number]] — parent global task.
