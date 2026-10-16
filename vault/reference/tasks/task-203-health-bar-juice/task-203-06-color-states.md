---
id: task-203-06-color-states
description: "The health fill colour follows the health left: green when full, yellow at half, red when low."
parent: task-203-health-bar-juice
kind: task
status: todo
---
# Colour by health

- goal: The health fill colour follows the health left: green when full, yellow at half, red when low.
- agent: cpp
- depends_on: task-203-05-punch
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/UI/RRHUDPanelWidget.h, Source/RogueRigs/Player/RRMiner.h
- spec: Base fill colour = gradient of the real health fraction: green #66d26f at 100%, yellow #e8c547 at 50%, red #e5544b at 20% and below, linear between stops (replaces FillColor; the flash of task-203-03 lerps from this colour). Colours and stops provisional, owner to confirm (the HUD bar is not in color-code). Changes only the colour, no rule. Updates sys-hud-health-bar.
- expected_output: C++ in Source/RogueRigs/UI/, sys-hud-health-bar doc; one commit.
- done_when: build exit 0 with the editor closed; RogueRigs.HUD tests pass (only expectations this change breaks updated, no new test); every value of this node is an EditDefaultsOnly UPROPERTY default, none hard-coded; per-frame work runs only while an effect is running (no permanent tick); sys-hud-health-bar doc matches the code; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-203-health-bar-juice]] — parent global task.
