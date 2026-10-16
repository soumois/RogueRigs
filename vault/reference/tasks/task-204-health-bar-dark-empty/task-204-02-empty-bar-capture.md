---
id: task-204-02-empty-bar-capture
description: "In-game capture after a hit showing the health bar's dark empty part, measured and sent to the owner."
parent: task-204-health-bar-dark-empty
kind: task
status: done
---
# Empty bar capture in game

- goal: a -game capture proves the empty part is dark (about #0B0C14) with the bar at 280 x 28, 20,232.
- agent: cpp
- depends_on: task-204-01-empty-bar-color
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), Source/RogueRigs/Debug/RRVisualCheckCommands.cpp
- spec: Same run as task-203-09 (L_ArenaM1, 1920 x 1080, rr.Stress.Peace 1, rr.Debug.DamageMiner 54 at 10 s), captures with the HUD (ShowUI 1) at full health and about 1.5 s after the hit, when the chunk has settled; prefix t204_02_. Measure the bar bounds and the empty part colour from the PNG pixels; describe fill and chunk. No code change.
- expected_output: PNG paths and descriptions; verified line of wbp-health-status; one commit of the doc (no PNG).
- done_when: the post-hit capture shows the empty part close to #0B0C14 and the bar 280 x 28 at 20,232; paths reported; doc committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 18, cache write 27247, cache read 145355, output 524
- runs: 1
- milestone: M1
- result: -game 1920 x 1080 L_ArenaM1, hit 54: bar 280 x 28 at 20,232; 1.5 s after the hit yellow fill to x 148, chunk settled, empty part measured #05060B (target #0B0C14: the tint multiplies the grey image, half as bright; doubling the tint to linear 0.0068/0.0075/0.0143 would match). Captures t204_02_full_00.png, t204_02_settled_00.png sent to the owner. Commit b7bab3dc.

## Relations
- [[task-204-health-bar-dark-empty]] — parent global task.
