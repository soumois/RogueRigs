---
id: task-049-01-shuttle-distance
description: "Shuttle lands 37 to 50 cells from the miner (was at least 75, no maximum)."
parent: task-049-shuttle-distance
kind: task
status: in-progress
---
# Closer shuttle

- goal: Shuttle lands 37 to 50 cells from the miner (was at least 75, no maximum).
- agent: cpp
- depends_on: none
- inputs: extraction (vault/reference/gdd/run/extraction.md), sys-objectives (vault/reference/tech/systems/sys-objectives.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: shuttle landing: ShuttleMinDistanceCells 75 → 37 and a new ShuttleMaxDistanceCells 50 (editable); keep the 4 × 3 footprint and reachability rules; fallback when nothing fits in the band: nearest fitting spot outside it (logged); check BP_ExtractionShuttle / BP_RRGameMode do not override them on disk (report, do not edit assets); update only existing tests this breaks; docs sys-objectives.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-049-shuttle-distance]] — parent global task.
