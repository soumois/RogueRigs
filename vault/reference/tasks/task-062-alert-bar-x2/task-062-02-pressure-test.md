---
id: task-062-02-pressure-test
description: "Integration.PressureHordeBanner updated to the doubled alert bar (icons at Position × 600)."
parent: task-062-alert-bar-x2
kind: task
status: done
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
- model: opus
- tokens: input 26, cache write 32163, cache read 171776, output 3673
- runs: 1
- milestone: M2
- result: Integration.PressureHordeBanner expects icons at Position × 600 (PressureAlertBarWidth 300 → 600). Build 0; full run 194/195 (only the known FlowField.BudgetAt67uu). Commit 693b54a. With task-062-01: alert bar 600 × 36, icons × 2.

## Relations
- [[task-062-alert-bar-x2]] — parent global task.
