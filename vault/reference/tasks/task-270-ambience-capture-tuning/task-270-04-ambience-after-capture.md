---
id: task-270-04-ambience-after-capture
description: "After captures at the frames of task-270-01 with the applied values, judged by the Orchestrator against the reference."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience after captures

- goal: After captures at the frames of task-270-01 with the applied values, judged by the Orchestrator against the reference.
- agent: cpp
- depends_on: task-270-03-ambience-apply
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Saved/LookTune/capture_cmd.txt
- expected_output: Frames in Saved/LookTune/r2/ named as in r0, measured, side by side with r0.
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them and sent them to the owner.
- editor_state: closed
- token_ceiling: 50k
- model: opus
- tokens: input 34, cache write 77030, cache read 828297, output 1093
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r2/ with sbs_* (r0 left, r2 right); values live in game confirmed by getall. Seen: cream pool core L 150, half radius 200 px (about 0.21 of width), violet rock tops outside the pool readable (L 4 -> 37-42), floor outside the pool very dark plum (L 0 -> 10), XP small cyan dots 10-17 px core. Still off: rock tops inside the radius burnt to a near-white crescent (L 212), chasers near-white cream (yellow lost, 643 clipped px vs 355), yellow cylinder reads lime under the violet sun, floor outside darker than DRGS. Cause judged: light too low (300 uu) for tall objects; round 3 raises it.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
