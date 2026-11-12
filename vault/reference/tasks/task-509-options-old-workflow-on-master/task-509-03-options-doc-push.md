---
id: task-509-03-options-doc-push
description: "Doc: vault docs of the old version in place, new-version docs removed, 504 to 506 marked superseded, lint, push master"
parent: task-509-options-old-workflow-on-master
kind: task
status: wait-answers
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
- started: 2026-11-12T12:39:02Z
- waiting_for: owner action: git push origin master (10 commits, 1cf36a89) is held by a Git Credential Manager sign-in for github.com; complete the sign-in (or run git push origin master yourself), then resume task-509-03 to close it
- result: 1cf36a89. Vault docs of the old version in place since task-509-01 (options-screen, sys-options, wbp-options, menu, hover, damage-numbers and art-prompt nodes); options-menu, sys-game-settings, sys-options-screen removed, no gdd or tech node names the 504-506 code or assets (checked). Tasks 504, 505, 506 carry 'superseded: remplacée par task-510-options-screen' and a relation to it. wbp-options records the master re-check (t509 captures). A7 unverified: the keep-or-revert countdown reaching 0 (auto revert), a real window mode change, the gameplay toggles during combat (damage numbers, aim indicator, screen shake, pause on focus loss), real mouse clicks. Lint clean. Push: pending, held by the credential sign-in.

## Relations
- [[task-509-options-old-workflow-on-master]] — parent global task.
