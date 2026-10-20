---
id: task-255-01-camera-arm-1400
description: "The miner camera arm is 1400 uu instead of 1700, to test the closer view."
parent: task-255-camera-arm-1400
kind: task
status: done
---
# Camera distance 1400

- goal: The miner camera arm is 1400 uu instead of 1700, to test the closer view.
- agent: cpp
- depends_on: none
- inputs: sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), miner (vault/reference/gdd/player/miner.md)
- expected_output: ARRMiner::CameraArmLength default 1400 (provisional, owner 2026-10-20, to test; was 1700); sys-player-classes and bp-miner doc lines updated; BP_Miner overrides nothing (read in the doc, check the Blueprint default is not overridden: read the class default through the headless tools if needed).
- done_when: Build passes; the default reads 1400 in the C++ and no Blueprint override of CameraArmLength exists; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 80k
- model: opus
- tokens: input 28, cache write 28435, cache read 281654, output 1872
- runs: 1
- milestone: M1
- result: ARRMiner CameraArmLength default 1700 -> 1400 (provisional, to test); no BP_Miner override (checked in the asset file); Dig and RunFlow 20 of 20 pass; docs updated (bp-miner also had an outdated 2200). RRHUDWidget.h comment about 1700 left. Look unverified.

## Relations
- [[task-255-camera-arm-1400]] — parent global task.
