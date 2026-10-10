---
id: task-079-01-text-loader
description: "A4 loader: the text files are loaded in game and feed FRRUpgradeCard.DescriptionText and the existing alert events."
parent: task-079-text-loader
kind: task
status: in-progress
---
# A4 text loader

- goal: A4 loader: the text files are loaded in game and feed FRRUpgradeCard.DescriptionText and the existing alert events.
- agent: cpp
- depends_on: task-076-02-upgrade-texts, task-078-02-alert-texts
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), sys-upgrades (vault/reference/tech/systems/sys-upgrades.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ loader of Content/RogueRigs/Data/Text/ files (validated ones only) with fallback to the current functional text when a file or an id is missing; wired on FRRUpgradeCard.DescriptionText (stat name kept, flavor line added) and on the alert event texts; barks are loaded into a lookup but not displayed. Automation tests for the loader, the fallback and the wiring. Gameplay logic in C++, per cpp-bp-split.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-079-text-loader]] — parent global task.
