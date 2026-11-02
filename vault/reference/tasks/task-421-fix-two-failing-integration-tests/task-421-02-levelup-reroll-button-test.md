---
id: task-421-02-levelup-reroll-button-test
description: "Cpp: LevelUpUpgradeLoop checks RerollButton after the 0.8 s deal"
parent: task-421-fix-two-failing-integration-tests
kind: task
status: todo
---
# Fix the two pre-existing failing integration tests: cpp

- goal: In `Source/RogueRigs/Tests/RRIntegrationTests.cpp`, `CheckHUDCards` (line ~1452) reads RerollButton enabled right after the offer, while WBP_UpgradeChoice keeps it disabled during the deal (StartDeal; AnimStep enables it = RerollAllowed at 0.8 s real time, from the widget tick). Make the check wait for the deal end: tick the WBP_UpgradeChoice instance (the HUD child `UpgradeChoice`) for at least 0.8 s of real time (world real time advances under the pause) before checking enabled == bCanReroll, for the first offer (true) and the rerolled offer (false). Fallback if the widget tick cannot run headless: check RerollButton disabled during the deal and the widget bool variable `RerollAllowed` == bCanReroll by reflection; say which path was used. Other assertions unchanged. Decided values: global request and gdd_check (Q2). Do not change WBP_HUD, WBP_UpgradeChoice or URRHUDWidget.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-tests-progression (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-progression.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), `Source/RogueRigs/Tests/RRIntegrationTests.cpp`, `Source/RogueRigs/UI/RRHUDWidget.h` (read only)
- expected_output: RRIntegrationTests.cpp changed, one commit; change summary (hash, how the deal end is reached, primary or fallback path) for the doc subtask.
- done_when: Build ok; `RogueRigs.Integration.LevelUpUpgradeLoop` passes headless and the other tests of RRIntegrationTests.cpp still pass; only that test file changed; committed.
- editor_state: closed
- token_ceiling: 0.6 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-421-fix-two-failing-integration-tests]] — parent global task.
