---
id: task-271-01-xp-bloom-capture-tuning
description: "XP pickups read as small bright cyan dots (about 8 to 14 px at 1080p, small halo) on real -game screenshots judged by the Orchestrator."
parent: task-271-xp-bloom-capture-tuning
kind: task
status: todo
---
# XP glow tuned on screenshots

- goal: XP pickups read as small bright cyan dots (about 8 to 14 px at 1080p, small halo) on real -game screenshots judged by the Orchestrator.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), color-code (vault/reference/gdd/art-audio/color-code.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: Before captures of XP pickups at the standard camera after task-270 (frame E plus a crop measured in px), in Saved/LookTune/xp0/.
- done_when: Captures exist, XP size measured in px; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-271-xp-bloom-capture-tuning]] — parent global task.
