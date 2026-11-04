---
id: task-427-03-clear-zone-doc
description: "Doc: document ClearRadius and FadeRadius"
parent: task-427-darkener-clear-zone
kind: task
status: done
---
# Darkener shader: clear zone around the screen centre with a transition radius: doc

- goal: Update mpp-player-light-mask: the two new parameters in the table, the formula, tuning hints, revert to the task-426 HLSL, the verification state from the capture (C1 to C4 of the before/after pair, C4 corner ratio for the task-426 open point).
- agent: doc
- depends_on: task-427-01-clear-zone-material, task-427-02-clear-zone-capture
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Doc nodes updated, lint clean, commit.
- done_when: mpp-player-light-mask states ClearRadius and FadeRadius (default, unit, meaning), the new formula, the revert and the capture results; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 18944, cache read 92150, output 3731
- usage: turns 6, context start 11k peak 24k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-11-04T17:48:11Z
- waiting_for: none
- result: mpp-player-light-mask documented (a3ac37c8); gdd visual-style sentence already had clear zone, still says CornerDarkness 0.85 vs owner 1

## Relations
- [[task-427-darkener-clear-zone]] — parent global task.
