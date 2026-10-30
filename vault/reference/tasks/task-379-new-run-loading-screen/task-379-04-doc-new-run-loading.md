---
id: task-379-04-doc-new-run-loading
description: "Doc: Update the menu, run, transition and menu game mode docs for the New Run loading path"
parent: task-379-new-run-loading-screen
kind: task
status: todo
---
# Main menu New Run: show the loading screen: doc

- goal: Update the doc nodes from the change summaries of task-379-01 and task-379-02 (git show of their commits) and the capture result of task-379-03: the New Run entry point and its loading path, the menu game mode LoadingScreenWidgetClass, the controller re-show for level 1, log lines, verification.
- agent: doc
- depends_on: task-379-01-new-run-loading-code, task-379-02-menu-game-mode-loading-class, task-379-03-new-run-loading-capture
- inputs: sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), bp-main-menu-game-mode (vault/reference/tech/blueprints/player/bp-main-menu-game-mode.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), agent-doc (vault/reference/agents/agent-doc.md), Global request and gdd_check of task-379-new-run-loading-screen for the decided values
- expected_output: Updated doc nodes, lint clean, one commit `task-379-04-doc-new-run-loading: <summary>`.
- done_when: sys-main-menu describes StartNewRun through the transition loading and the new game mode property; sys-run-transition lists the New Run entry point; bp-main-menu-game-mode lists LoadingScreenWidgetClass = WBP_Loading_C; wbp-loading says it is also shown for New Run; sys-run gotcha updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
