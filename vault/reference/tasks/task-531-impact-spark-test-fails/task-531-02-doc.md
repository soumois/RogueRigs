---
id: task-531-02-doc
description: "Doc: Update the system doc if the fix changed the impact effect code or its contract; otherwise record that no doc change was needed."
parent: task-531-impact-spark-test-fails
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T18:08:13Z
- waiting_for: none
- result: open

## Relations
- [[task-531-impact-spark-test-fails]] — parent global task.
