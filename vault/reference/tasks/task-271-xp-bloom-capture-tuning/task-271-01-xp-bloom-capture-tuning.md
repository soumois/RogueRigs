---
id: task-271-01-xp-bloom-capture-tuning
description: "XP pickups read as small bright cyan dots (about 8 to 14 px at 1080p, small halo) on real -game screenshots judged by the Orchestrator."
parent: task-271-xp-bloom-capture-tuning
kind: task
status: done
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
- model: none
- tokens: none (no agent run: judged on the task-270 capture frames)
- runs: 0
- milestone: M1
- result: No agent run and no asset change. Judged by the Orchestrator on the task-270 frames E_xp (Saved/LookTune/r0, r5) and Saved/LookTune/final_271_xp_r0_r5.png, final_271_xp_zoom3_r5.png: each XP reads as a cyan hex nut with a bright hole and a small tight halo; the large blob hiding the nut (owner image of 2026-10-20) is gone (task-269 glow values plus the fixed exposure of task-268). Size: 18-23 px far from the miner, about 34 px near the bottom edge, set by the 30 cm nut mesh, not by the glow (halo a few px). Reaching 8-14 px needs a smaller nut (template scale about 0.45), outside the glow knobs: owner decision, not done.

## Relations
- [[task-271-xp-bloom-capture-tuning]] — parent global task.
