---
id: task-496-02-request-timing
description: "Cpp: Level 1 layout requested when the main menu opens and used at New Run; next level requested at the extractor board contact instead of later."
parent: task-496-generator-in-runs
kind: task
status: in-progress
---
# Layout generator wired into real runs: cpp

- goal: Level 1 layout requested when the main menu opens and used at New Run; next level requested at the extractor board contact instead of later.
- agent: cpp
- depends_on: task-496-01-generator-provider
- inputs: sys-run-layout (vault/reference/tech/systems/world/sys-run/sys-run-layout.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), main-menu (vault/reference/gdd/run/main-menu.md), extraction (vault/reference/gdd/run/extraction.md), intermission (vault/reference/gdd/run/intermission.md)
- expected_output: see done_when
- done_when: Acceptance A2, A3 and A6 hold in a -game -nullrhi log run: time stamps show level 1 requested at menu open and ready before or at New Run with no long game-thread frame, level 2 requested at the board contact, stats screen path, Continue and the task-318 rebuild unchanged; Retry and Main Menu then New Run draw a new seed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-10T20:05:01Z
- waiting_for: none
- result: open

## Relations
- [[task-496-generator-in-runs]] — parent global task.
