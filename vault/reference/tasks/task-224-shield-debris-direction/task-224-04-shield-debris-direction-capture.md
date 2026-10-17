---
id: task-224-04-shield-debris-direction-capture
description: "cpp: one -game run, a shield chaser broken by rr.Debug.BreakShields from the front; capture the fragments leaning to the shield side."
parent: task-224-shield-debris-direction
kind: task
status: done
---
# Shield debris direction: capture

- goal: an image where the shield fragments fly back and toward the shield side.
- agent: agent-cpp
- depends_on: task-224-03-bp-shield-chaser-hitdir
- inputs: `vault/reference/agents/agent-cpp.md`, `vault/reference/tech/systems/core/sys-cheats.md` (rr.Debug.BreakShields), the launch script of task-220-04 (`scratchpad\run3.ps1` in the session scratchpad, named in that task's report) and its log.
- method: ONE game run, editor closed, no build. Shield chaser walking toward the miner (so the miner is in front of it), rr.Debug.BreakShields, shots during the first second and at about 2 s. Save `vault/assets/shield-chaser-debris-direction.png`. Say in the report on which side of the robot the fragments land.
- done_when: the image exists and the side is reported; no Blueprint or Niagara error.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 34, cache write 57813, cache read 542189, output 6041
- runs: 1
- milestone: M1
- result: Commit c7611a63: vault/assets/shield-chaser-debris-direction.png; one -game run; robot facing the miner, rr.Debug.BreakShields (miner hit from the front): fragments travel back and toward the robot left (shield side), about +163 px back and +58 px left from the break point, about 20 degrees; no Blueprint or Niagara error. Fragments look like long motion-blur streaks (game at 4-6 fps during shots); BreakShields counts 6 living shield chasers for 1 spawned.

## Relations
- [[task-224-shield-debris-direction]] — parent global task.
