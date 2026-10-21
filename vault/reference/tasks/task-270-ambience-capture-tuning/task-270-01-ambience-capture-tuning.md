---
id: task-270-01-ambience-capture-tuning
description: "The arena around the miner matches the DRGS reference on real -game screenshots judged by the Orchestrator: dark violet scene, tight warm light pool, readable robots."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
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
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
