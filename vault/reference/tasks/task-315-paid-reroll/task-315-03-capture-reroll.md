---
id: task-315-03-capture-reroll
description: "Capture the level-up screen with the paid reroll button in game (cpp): paid level-up reroll, step 03."
parent: task-315-paid-reroll
kind: task
status: todo
---
# Capture the paid reroll in game

- goal: Headless -game run with cheats: grant ore and XP to open a level-up screen, frame the screen with the reroll cost (15), reroll twice (20, 25), then frame it unaffordable (greyed); compare with an earlier level-up screen frame if one exists in Saved/; contact sheet for the owner. Nothing else changed.
- agent: cpp
- depends_on: task-315-02-bp-reroll-button
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md)
- expected_output: Saved/PaidReroll/ frames and contact sheet, report with the ore count before and after each reroll
- done_when: frames show the cost 15, 20, 25 and the greyed button; ore count drops by the cost; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-315-paid-reroll]] — parent global task.
