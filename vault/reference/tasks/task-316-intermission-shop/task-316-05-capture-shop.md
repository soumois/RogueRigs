---
id: task-316-05-capture-shop
description: "Capture the intermission shop in game (cpp): upgrade shop between levels, step 05."
parent: task-316-intermission-shop
kind: task
status: todo
---
# Capture the shop in game

- goal: Headless -game run with cheats: grant ore, end level 1 (rr.Run.NextLevel), frame the intermission with the 5 cards, prices and balance; buy one card, reroll once, frame again; press Continue and check in the log that the bought card is replayed on the level 2 miner; compare with an earlier intermission frame if one exists in Saved/; contact sheet for the owner. Nothing else changed.
- agent: cpp
- depends_on: task-316-04-bp-intermission-shop
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run (vault/reference/tech/systems/world/sys-run.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md)
- expected_output: Saved/IntermissionShop/ frames and contact sheet, report with balance before and after buy and reroll
- done_when: frames show 5 priced cards, balance, reroll cost and Continue; balance drops by price and reroll cost; bought card present on the level 2 miner; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-316-intermission-shop]] — parent global task.
