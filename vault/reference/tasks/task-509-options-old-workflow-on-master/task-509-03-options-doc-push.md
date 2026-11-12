---
id: task-509-03-options-doc-push
description: "Doc: vault docs of the old version in place, new-version docs removed, 504 to 506 marked superseded, lint, push master"
parent: task-509-options-old-workflow-on-master
kind: task
status: done
---
# Écran Options : version de l'ancien workflow sur master, puis push: doc

- goal: vault docs of the old version in place, new-version docs removed, 504 to 506 marked superseded, lint, push master
- agent: doc
- depends_on: task-509-02-options-capture
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), sys-game-settings (vault/reference/tech/systems/ui/sys-game-settings.md), sys-options-screen (vault/reference/tech/systems/ui/sys-options-screen.md), wbp-options (vault/reference/tech/blueprints/ui/wbp-options.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), sys-pause-menu (vault/reference/tech/systems/ui/sys-pause-menu.md), cost-log (vault/reference/production/cost-log.md)
- expected_output: see done_when
- done_when: A1 and A2 hold for the vault; tasks 504 to 506 carry a "remplacée par task-510" line; A7 recorded in the result; lint clean; committed; master pushed to origin (A8).
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-509
- usage: included in the executor run of task-509
- runs: 1
- milestone: M2
- started: 2026-11-12T13:30:59Z
- waiting_for: none
- result: 1cf36a89. A1 old-workflow options screen on master (code, Blueprints, sprites, sound classes, Art/Source images from e0968216 in 1abc6245; vault docs options-screen, sys-options, wbp-options, menu, hover, damage-numbers, art-prompt nodes). A2 the 504-506 code, assets and docs (options-menu, sys-game-settings, sys-options-screen) removed, no gdd or tech node names them; tasks 504, 505, 506 carry 'remplacée par task-510-options-screen'. A3 maps and Config identical to master, tasks 500-503, 507, 508 and both cost histories kept. A4 old task imported as task-510-options-screen. A5 build exit 0, RogueRigs.DamageNumbers and Pickups.OreNumber 11/11, game launch without crash. A6 captures at four resolutions from the main menu and the pause menu match the old ones (task-509-02, Saved/Captures/t509_sheets). A7 not verified: the keep-or-revert countdown reaching 0, a real window mode change, the gameplay toggles during combat, real mouse clicks. A8 lint clean; master pushed to origin by the relay after the owner signed in to GitHub (12717257..811dcc71, origin/master == master); the first push by the executor failed without credentials. The closing commit is pushed by the relay.

## Relations
- [[task-509-options-old-workflow-on-master]] — parent global task.
