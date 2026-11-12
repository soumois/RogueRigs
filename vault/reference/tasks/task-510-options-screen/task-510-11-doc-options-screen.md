---
id: task-510-11-doc-options-screen
description: "Doc: write the options system and WBP_Options doc nodes, update the main and pause menu docs, link them in their indexes"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: doc

- goal: write the options system and WBP_Options doc nodes, update the main and pause menu docs, link them in their indexes
- agent: doc
- depends_on: task-510-10-options-capture
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-pause-menu (vault/reference/tech/systems/ui/sys-pause-menu.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md)
- expected_output: see done_when
- done_when: A sys node for the settings and options widget and a wbp node for WBP_Options (anchors table) exist and are linked; wbp-main-menu, wbp-pause-menu and the touched sys nodes updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:58:16Z
- waiting_for: none
- result: c6cc6421. Created sys-options (settings class, URROptionsSettings, URROptionsSubsystem, URROptionsWidget, menu wiring, rr.Options commands, flow, gotchas) and wbp-options (anchors table, naming of the controls, assets); updated sys-main-menu, sys-pause-menu, wbp-main-menu, wbp-pause-menu (Options button, new button anchors), sys-damage-numbers (toggle now on the options screen), systems-ui-index, blueprints-ui-index, art-prompts-options-screen-mockup (resized sprites). Lint clean.

## Relations
- [[task-510-options-screen]] — parent global task.
