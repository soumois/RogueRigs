---
id: task-531-02-doc
description: "Doc: Update the system doc if the fix changed the impact effect code or its contract; otherwise record that no doc change was needed."
parent: task-531-impact-spark-test-fails
kind: task
status: done
---
# Test RogueRigs.Weapons.ImpactSparkAsset en échec: doc

- goal: Update the system doc if the fix changed the impact effect code or its contract; otherwise record that no doc change was needed.
- agent: doc
- depends_on: task-531-01-fix-impact-spark-test
- inputs: sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md)
- expected_output: see done_when
- done_when: If runtime code changed: the sys node that documents the impact effect pool states the corrected behaviour (prewarm, request, play) and links task-531; if only the test changed: the subtask report says no doc node needed an update. Lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-531
- usage: included in the executor run of task-531
- runs: 1
- milestone: M2
- started: 2026-11-13T18:08:13Z
- waiting_for: none
- result: Done. No doc node needed an update: only the test RogueRigs.Weapons.ImpactSparkAsset changed (716bdb8e), no runtime code, no contract change of ARRProjectile or URRNiagaraPoolSubsystem; sys-weapons does not cite the test or NS_ShotImpactSpark, and bp-shot-impact-burst already states the rifle plays NS_ShotImpactBurstGreen. Lint clean.

## Relations
- [[task-531-impact-spark-test-fails]] — parent global task.
