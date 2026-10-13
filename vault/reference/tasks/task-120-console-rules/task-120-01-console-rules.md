---
id: task-120-01-console-rules
description: "The console hack zone radius is 7 cells, every rock in the zone (ore included) must be destroyed before hacking, and the hack lasts 12 s."
parent: task-120-console-rules
kind: task
status: in-progress
---
# Console zone and hack rules

- goal: The console hack zone radius is 7 cells, every rock in the zone (ore included) must be destroyed before hacking, and the hack lasts 12 s.
- agent: cpp
- depends_on: none
- inputs: objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-tests-console-extraction (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-console-extraction.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md)
- expected_output: ARRConsole zone radius 7 cells, marking radius equal to the zone radius with every rock type (Rock and Ore kinds) marked, hack time 12 s, drain unchanged; chest unchanged; sys docs updated; report whether BP_ObjectiveConsole overrides these values.
- done_when: build exits 0 with the editor closed; RogueRigs.Console.*, RunFlow.*, Extraction.*, Integration.ConsoleExtractionWin pass headless (expectations updated only where the new values change them); sys-objectives-console-chest updated; lint clean
- editor_state: editor closed (C++ build)
- token_ceiling: 60 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: `ARRConsole` `ZoneRadiusCells` 7 (469 uu), `HackDuration` 12, `DrainPerSecond` 0.05 unchanged; `MarkRadiusCells` removed: `FindMarkableCells` marks every `MarkedCellTypes` block (Rock, Ore, HealOre, SpecialOre) whose center is within `ZoneRadiusCells` of the footprint centre (the zone centre), ore table deposits now included (the 2026-10-10 deposit exclusion is dropped). Chest unchanged (8 s, 6 cells, no marks). No override: BP_ObjectiveConsole, BP_BonusChest, BP_RRGameMode, CSVs and Config hold none of these properties. Tests updated: HackRules and ZoneTelegraph use console anchor (125, 117) (the old cell was outside 7 cells), 12 s timings; DropPlacementAndMarks checks the 7-cell zone, deposits included; FineConsoleShuttlePlacement 469 uu and Ore.Placement deposit marked (both edited, not run). Build exit 0; Console.*, RunFlow.*, Extraction.*, Integration.ConsoleExtractionWin: 13/13 pass headless.

## Relations
- [[task-120-console-rules]] — parent global task.
