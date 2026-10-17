---
id: task-224-04-shield-debris-direction-capture
description: "cpp: one -game run, a shield chaser broken by rr.Debug.BreakShields from the front; capture the fragments leaning to the shield side."
parent: task-224-shield-debris-direction
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-224-shield-debris-direction]] — parent global task.
