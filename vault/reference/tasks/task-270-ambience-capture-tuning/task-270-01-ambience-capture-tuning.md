---
id: task-270-01-ambience-capture-tuning
description: "The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience tuned on screenshots

- goal: The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), color-code (vault/reference/gdd/art-audio/color-code.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: Baseline -game captures (before) of a fixed frame set at the standard camera (arm 1400, fov 75, pitch -50), 1920x1080, in Saved/LookTune/r0/: A miner idle on open floor, B miner next to a rock wall, C miner digging (blocks falling), D robot group with a yellow chaser near the miner, E XP pickups on the floor near the miner, F crystals or deposits in view; then, in the same run when the engine allows it without code, variant frames with runtime overrides of the lighting knobs (PlayerLight intensity and radius, sun and sky intensity, exposure), each labelled with the values actually applied and checked to have taken effect. Exact command lines recorded so later rounds repeat the same frames.
- done_when: Captures exist and are described; command lines recorded in the result; no game process left; Orchestrator judged the frames against the reference.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 66, cache write 202744, cache read 2400065, output 3935
- runs: 1
- milestone: M1
- result: Baseline frames A idle, B wall, C dig (no falling blocks: shots too late), D robots+XP, E XP, F deposits and 11 runtime variants in Saved/LookTune/r0/ (not committed); commands in Saved/LookTune/capture_cmd.txt. Seen: beyond the pool pure black (RGB about 3,0,12), pool dull orange-brown with half-luminance radius 230-275 px (0.26 of the width, target 0.18), rock tops in the radius very bright, chasers read orange, robots outside the pool invisible, XP 12-21 px (bloom about half). Runtime overrides that work: setnopec <Class> <Prop> <Value> + r.RecreateRenderStateContext, r.ExposureOffset, r.BloomQuality; plain set on PointLightComponent hangs the game. Sun/sky at 0.12/0.08 have almost no visible effect.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
