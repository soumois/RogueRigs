---
id: task-025-04-per-digit-contract
description: "C++ pushes one entry per digit (offset position, digit 0–9, size, colour, crit, spawn time) and a batch id, so Niagara needs no index math."
parent: task-025-damage-numbers
kind: task
status: in-progress
---
# Per-digit damage number contract

- goal: Niagara only draws one sprite per pushed digit.
- agent: cpp
- depends_on: task-025-03-numbers-perf
- inputs: [[sys-damage-numbers]], [[stress-damage-numbers]], [[bp-damage-numbers]], [[cpp-standard]], [[sys-automation-tests]]
- expected_output: URRDamageNumberSubsystem lays out digits itself (left-to-right for the top-down camera, spacing from size, centred on the number position) and pushes per-digit arrays: User.DigitPositions (Position), User.DigitIndices (Int32 0–9), User.DigitSizes, User.DigitColors, User.DigitCriticals (Int32 0/1, avoids the bool array), User.DigitSpawnTimes, plus User.DigitCount (number of digits pushed) and User.DigitBatchId (Int32 incremented per push, never reset) so Niagara spawns exactly once per batch; crit colour (1, 0.8, 0.3); old number-level parameters removed; the Niagara Data Channel route may be used instead if it is simpler and documented; contract documented in sys-damage-numbers for the bp agent; tests updated (digit layout, batch id, crit flag).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-025-damage-numbers]] — parent global task.
