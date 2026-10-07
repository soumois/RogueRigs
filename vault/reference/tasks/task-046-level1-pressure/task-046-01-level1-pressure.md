---
id: task-046-01-level1-pressure
description: "Level 1 pressure: 80 robots alive at most, wave interval ending at 5.5 s, packs of one wave at least 120° apart."
parent: task-046-level1-pressure
kind: task
status: done
---
# Level 1 pressure

- goal: Level 1 pressure: 80 robots alive at most, wave interval ending at 5.5 s, packs of one wave at least 120° apart.
- agent: cpp
- depends_on: task-048-01-last-level-profile
- inputs: enemies-index (vault/reference/gdd/enemies/enemies-index.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: FRRWaveDirectorConfig::MakeDefault (level 1): alive cap 150 → 80 (wherever the cap lives in C++; report if only in a Blueprint); wave interval end 4.1 → 5.5 s (start 7.1 s and ramp unchanged); the packs of one wave pick sides at least 120° apart around the miner (replaces distinct sides; keep the heading sector draw); 2–3 packs from 90 s unchanged; update only existing tests this breaks; also fix Integration.PlacedSpawnerMix (RRShooterIntegrationTests.cpp:465), stale since task-045-02 set the BP_EnemySpawner heavy MaxAlive to 3 (it still expects 5); docs sys-spawner-director.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 58, cache write 229116, cache read 1032073, output 28101
- runs: 1
- milestone: M2
- result: ARREnemySpawner::MaxAlive 150 → 80 (C++, not overridden; shared by all levels); MakeDefault wave interval end 4.1 → 5.5 s; PackMinSeparationDegrees 120 (anchor angles; 3-pack waves placed at 120° steps); MakeLastLevel untouched; PlacedSpawnerMix fixed to heavy cap 3. Build 0, 195/195. Commit 5ccbb9b.

## Relations
- [[task-046-level1-pressure]] — parent global task.
