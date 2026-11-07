---
id: task-484-07-doc
description: "Doc: Doc nodes of the new widget, the changed HUD widgets and the run-flow system updated."
parent: task-484-death-screen
kind: task
status: done
---
# Death screen with Retry and Main Menu: doc

- goal: Doc nodes of the new widget, the changed HUD widgets and the run-flow system updated.
- agent: doc
- depends_on: task-484-06-capture
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: A doc node for the new widget (tree, anchors table for owner retouches, events) is listed in blueprints-ui-index; wbp-alert-objectives, wbp-health-status, wbp-hud and the run-flow system node describe the new loss path; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-484
- usage: included in the executor run of task-484
- runs: 1
- milestone: M2
- started: 2026-11-07T01:47:19Z
- waiting_for: none
- result: Commit 5364acc9. Created wbp-run-lost (tree, anchors table for owner retouches, events, ApplyAppearance values, verification) listed in blueprints-ui-index, and sys-run-lost (URRRunLostSubsystem, FRRRunLostRecap, URRRunLostWidget, controller and HUD hooks, URRRunSettings tunables, debug commands, tests) under sys-run. Updated sys-objectives-run-flow (loss path, game mode restart removed, ReturnToMainMenu users), sys-hud-events (OnRunLost moment), wbp-alert-objectives (ShowRunEnd success branch), wbp-health-status (no GAME OVER), wbp-hud (HUD collapsed on a loss), bp-rr-player-controller (RunLostWidgetClass), sys-run-log (GetRunKillCount). Lint clean. ALERT cause: executor run over the standard budget (whole task).

## Relations
- [[task-484-death-screen]] — parent global task.
