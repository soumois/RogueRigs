---
id: task-026-00-landing-clearance
description: "Console and shuttle land only where their whole footprint (2 × 2 and 4 × 3 cells at 67 uu) is open floor; tests."
parent: task-026-pressure
kind: task
status: in-progress
---
# Landing clearance for console and shuttle

- goal: the console and shuttle meshes never overlap rock when they land.
- agent: cpp
- depends_on: task-029-07-final-check
- inputs: [[objective-console]], [[extraction]], [[sys-objectives]], [[sys-grid]], [[cpp-standard]], [[sys-automation-tests]], task-029-07 result (overlap finding in RogueRigs.Integration.FineConsoleShuttlePlacement)
- expected_output: landing cell selection for the console and the shuttle requires every cell of the footprint (console 2 × 2, shuttle 4 × 3, sizes as editable properties; orientation as today) to be open floor and reachable; fallback when no spot fits is logged; FineConsoleShuttlePlacement asserts zero rock overlap over its seeds; docs sys-objectives.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-026-pressure]] — parent global task.
