---
id: task-509-03-options-doc-push
description: "Doc: vault docs of the old version in place, new-version docs removed, 504 to 506 marked superseded, lint, push master"
parent: task-509-options-old-workflow-on-master
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-509-options-old-workflow-on-master]] — parent global task.
