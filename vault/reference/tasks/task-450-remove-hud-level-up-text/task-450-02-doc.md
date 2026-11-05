---
id: task-450-02-doc
description: "Doc: Remove the cyan text from the docs that describe it as current."
parent: task-450-remove-hud-level-up-text
kind: task
status: todo
---
# Remove the cyan LEVEL UP text from the HUD: doc

- goal: Remove the cyan text from the docs that describe it as current.
- agent: doc
- depends_on: task-450-01-remove-text
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: No current doc describes it; lint clean.
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
- [[task-450-remove-hud-level-up-text]] — parent global task.
