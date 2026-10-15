---
id: task-185-02-alert-label-test
description: "Update the integration test that reads AlertLabel (RRIntegrationTests.cpp, pause check) now that the label is gone."
parent: task-185-alert-label-remove
kind: task
status: in-progress
---
# Alert label removed: test expectation

- goal: Update the integration test that reads AlertLabel (RRIntegrationTests.cpp, pause check) now that the label is gone.
- agent: cpp
- depends_on: task-185-01-alert-label-widget
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-tests-run-integration (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-run-integration.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: In Source/RogueRigs/Tests/RRIntegrationTests.cpp around line 2019, the check "AlertLabel shows the pause" removed or replaced by a check on what still shows the pause (the alert bar state or the run flow paused flag), nothing else changed; test doc updated if it lists that check
- done_when: Build passes with the editor closed; the run integration tests pass except known failures; committed by path
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-185-alert-label-remove]] — parent global task.
