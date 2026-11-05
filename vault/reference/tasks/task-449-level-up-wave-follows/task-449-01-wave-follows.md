---
id: task-449-01-wave-follows
description: "Cpp: The level-up wave stays on the miner while it plays, the push staying one blow."
parent: task-449-level-up-wave-follows
kind: task
status: done
---
# Level-up floor wave follows the miner: cpp

- goal: The level-up wave stays on the miner while it plays, the push staying one blow.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), bp-level-up-burst (vault/reference/tech/blueprints/fx/bp-level-up-burst.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), task-441-level-up-effect (vault/reference/tasks/task-441-level-up-effect.md), Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp
- expected_output: see done_when
- done_when: Build ok; in the simulation ring and shard positions follow a moving miner and stay at floor height; push still applied once; LevelUp test family passes, figures given in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-449
- usage: included in the executor run of task-449
- runs: 1
- milestone: M2
- started: 2026-11-05T22:42:52Z
- waiting_for: none
- result: Commit 6e31c55a. URRLevelUpComponent ticks only while its pooled NS_LevelUpWave plays (TG_PostPhysics, after movement) and moves it under the miner's feet each frame; the local-space ring and shards follow. Text spawn and push unchanged. Build ok. RogueRigs.LevelUp under -RenderOffScreen: 9/9 pass, new WaveFollowsMiner: miner running 600 uu/s covers 300 uu during the 29 frames of the wave; wave and ring 0.00 uu off his feet, height error 0.00 uu; shards centroid within 31.5 uu of him, reach 321 uu around him, flat (0.00 uu); push launched once, none while following; 25 sprites; tick off after the wave. WaveAsset unchanged (25 sprites, 29 frames, ring 599 uu). Look in game unverified (owner).

## Relations
- [[task-449-level-up-wave-follows]] — parent global task.
