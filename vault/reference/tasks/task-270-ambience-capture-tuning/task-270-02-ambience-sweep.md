---
id: task-270-02-ambience-sweep
description: "Second runtime sweep of light combinations on the same frames, to pick the values bp applies."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience sweep, round 1

- goal: Second runtime sweep of light combinations on the same frames, to pick the values bp applies.
- agent: cpp
- depends_on: task-270-01-ambience-capture-tuning
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Saved/LookTune/capture_cmd.txt
- expected_output: Variant frames in Saved/LookTune/r1/, each named with its values, measured (pool radius, core and corner luminance).
- done_when: Frames exist and are measured; no game process left; Orchestrator picked values.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 100, cache write 148031, cache read 2906108, output 7929
- runs: 1
- milestone: M1
- result: Frames in Saved/LookTune/r1/ (D spawn with robots+XP, 12 variants; B wall 4; C dig with falling blocks at 0.36-0.60 s after BugItGo), measured; macros in r1/macros (LightColor needs an exec macro file in a space-free path containing Binaries). Best: v6 (P radius 520, 700 cd, cream 255/205/150, sun 1.2): cream pool core L 154, half radius about 200 px, violet rock tops L 44, robots readable, chasers pale yellow. Still off: open floor outside the pool almost black (L 8-14), sky light has no effect (0.3 = 0.08), rock tops inside the radius clip to a white crescent, chaser torso clips, yellow cylinder turns lime under the violet sun; light height cannot change at runtime. Picked for 03: P radius 520, 650 cd, colour (1.0, 0.80, 0.59), sun 1.5, rest unchanged.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
