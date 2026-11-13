---
id: task-521-03-doc
description: "Doc: widget doc node: the tab size fix"
parent: task-521-options-tab-button-shrinks
kind: task
status: done
---
# Options : l'onglet cliqué devient plus petit: doc

- goal: widget doc node: the tab size fix
- agent: doc
- depends_on: task-521-02-capture
- inputs: sys-options (vault/reference/tech/systems/ui/sys-options.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md)
- expected_output: see done_when
- done_when: A4 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-521
- usage: included in the executor run of task-521
- runs: 1
- milestone: M2
- started: 2026-11-13T13:33:20Z
- waiting_for: none
- result: sys-options updated (commit 23950212): data line gives ActiveTabTransparentBorder 26 px and the margin basis; new gotcha on the tab size fix (commit 3548c421, same size for every tab, UV crop of the active plate); relation to task-521. sys-button-hover unchanged: the tabs are not attached to the hover subsystem. Lint clean.

## Relations
- [[task-521-options-tab-button-shrinks]] — parent global task.
