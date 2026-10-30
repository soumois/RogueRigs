---
id: task-388-02-doc-chest-effects
description: "Doc: update the preload and chest docs with the effects cleared between levels"
parent: task-388-chest-glow-cleared-between-levels
kind: task
status: done
---
# Bonus chest glow removed between levels: doc

- goal: Record what the ClearingLevel step now removes for the bonus chest, the cause found and the log verification.
- agent: doc
- depends_on: task-388-01-clear-chest-effects
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), global request and gdd_check: vault/reference/tasks/task-388-chest-glow-cleared-between-levels.md
- expected_output: sys-run-transition-preload (ClearingLevel list, log line, verification), sys-objectives-console-chest (gotcha: what the chest leaves in the world and who clears it), bp-bonus-chest only if the Blueprint changed; commit `task-388-02-doc-chest-effects: ...`.
- done_when: The listed nodes state the new clear, its log line and that the look is left to the owner (no capture); lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 40725, cache read 142313, output 3680
- usage: turns 6, context start 11k peak 41k, resumes 0, 1 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-10-30T20:57:48Z
- waiting_for: none
- result: Doc: sys-run-transition-preload, sys-objectives-console-chest, bp-bonus-chest updated (commit 3114f850); look left to owner.

## Relations
- [[task-388-chest-glow-cleared-between-levels]] — parent global task.
