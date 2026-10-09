---
id: task-062-02-pressure-test
description: "Integration.PressureHordeBanner updated to the doubled alert bar (icons at Position × 600)."
parent: task-062-alert-bar-x2
kind: task
status: in-progress
---
# Pressure test after the bigger alert bar

- goal: the existing pressure integration test matches the 600 px alert bar.
- agent: cpp
- depends_on: task-062-01-alert-bar-x2
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: RRPressureIntegrationTests.cpp, Integration.PressureHordeBanner: expected icon x positions follow Position × 600 (WBP_HUD since task-062-01); no other change, no new test; docs sys-automation-tests.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless except the known load-sensitive FlowField.BudgetAt67uu (task-063); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-062-alert-bar-x2]] — parent global task.
