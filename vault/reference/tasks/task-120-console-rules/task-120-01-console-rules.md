---
id: task-120-01-console-rules
description: "The console hack zone radius is 7 cells, every rock in the zone (ore included) must be destroyed before hacking, and the hack lasts 12 s."
parent: task-120-console-rules
kind: task
status: todo
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
- result: open

## Relations
- [[task-120-console-rules]] — parent global task.
