---
id: task-316-04-bp-intermission-shop
description: "WBP_Intermission hosts the shop: 5 WBP_ShopCard in a row, ore balance, reroll with cost, Continue (bp): upgrade shop between levels, step 04."
parent: task-316-intermission-shop
kind: task
status: done
---
# Shop on the intermission screen

- goal: In WBP_Intermission replace the "Shop opens later" placeholder with the shop, using the full screen: recap compact at the top, a row of 5 WBP_ShopCard in the middle, the ore balance (T_Icon_Ore + secured ore), a Reroll button with the ore icon and its cost (greyed when unaffordable) and the Continue button at the bottom. Bind to the C++ shop API of task-316-01 (offer, prices, buy, reroll, change event); labels from screen_texts.json as listed by task-316-02. Display only. Update the doc node wbp-intermission.
- agent: bp
- depends_on: task-316-02-text-shop-labels, task-316-03-bp-shop-card
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-shop-card (doc node created by task-316-03 in vault/reference/tech/blueprints/ui/), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md, as updated by task-316-01), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md); files: Content/RogueRigs/Data/Text/screen_texts.json
- expected_output: /Game/RogueRigs/UI/WBP_Intermission changed, doc node wbp-intermission updated
- done_when: compiled with warnings as errors and saved; tree and graph wiring read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 50000
- model: sonnet
- tokens: input 98, cache write 116268, cache read 3651909, output 33770
- runs: 1
- milestone: M2
- result: WBP_Intermission hosts the shop: Salvage Stall title (Labels 0), row of 5 WBP_ShopCard (SlotIndex 0-4), balance row (T_Icon_Ore + GetOreBalance), Reroll button (Labels 5, ore icon, GetRerollCost, disabled when CanReroll false, RequestReroll), Continue unchanged; RefreshShop on Construct and OnShopRefreshed; compiled warnings as errors, saved; doc wbp-intermission; commit 809d366f; nothing seen in game, 1080 px height fit tight

## Relations
- [[task-316-intermission-shop]] — parent global task.
