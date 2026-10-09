---
id: task-059-01-damage-numbers-no-merge
description: "Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept."
parent: task-059-damage-numbers-no-merge
kind: task
status: in-progress
---
# Damage numbers without merging

- goal: Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept.
- agent: cpp
- depends_on: none
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers (vault/reference/tech/systems/sys-damage-numbers.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: URRDamageNumberSubsystem: remove the merge of hits on the same target (one number per hit); add a random start offset in a horizontal disc of SpawnRadius (30 uu, editable) around the hit, 45 uu for the heavy (per-class or from the robot hit capsule, editable); keep the vertical stacking against overlap; update only the existing damage-number tests this breaks (merge tests become one-number-per-hit checks); docs sys-damage-numbers.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-059-damage-numbers-no-merge]] — parent global task.
