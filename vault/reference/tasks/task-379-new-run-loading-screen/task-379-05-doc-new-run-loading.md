---
id: task-379-05-doc-new-run-loading
description: "Doc: Update the menu, run, transition, loading widget and menu game mode docs"
parent: task-379-new-run-loading-screen
kind: task
status: done
---
# Main menu New Run: show the loading screen: doc

- goal: Update the doc nodes from the change summaries of task-379-01, 02 and 03 (git show of their commits) and the capture result of task-379-04: the New Run entry point and its loading path, IsLoadingProgressMeasured, the menu game mode LoadingScreenWidgetClass, the controller re-show for level 1, the WBP_Loading animation, log lines, verification.
- agent: doc
- depends_on: task-379-01-new-run-loading-code, task-379-02-menu-game-mode-loading-class, task-379-03-loading-animation-widget, task-379-04-new-run-loading-capture
- inputs: sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), bp-main-menu-game-mode (vault/reference/tech/blueprints/player/bp-main-menu-game-mode.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), agent-doc (vault/reference/agents/agent-doc.md), Global request, gdd_check and Questions of task-379-new-run-loading-screen for the decided values
- expected_output: Updated doc nodes, lint clean, one commit `task-379-05-doc-new-run-loading: <summary>`.
- done_when: sys-main-menu describes StartNewRun through the transition loading and the new game mode property; sys-run-transition lists the New Run entry point; sys-run-transition-fade lists IsLoadingProgressMeasured; bp-main-menu-game-mode lists LoadingScreenWidgetClass = WBP_Loading_C; wbp-loading describes LoadingAnimImage, its animation and the measured branch, and that it is shown for New Run; sys-run gotcha updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 20, cache write 59385, cache read 344912, output 10874
- usage: turns 10, context start 12k peak 59k, resumes 0, 1 min, units 0.16 M
- runs: 1
- milestone: M2
- started: 2026-10-30T04:30:34Z
- waiting_for: none
- result: Doc done (8bc43286): tick-driven spin, Q5 freeze, unverified first-spin delay line, async preload lead (not decided).

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
