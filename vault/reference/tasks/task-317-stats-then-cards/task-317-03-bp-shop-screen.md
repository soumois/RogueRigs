---
id: task-317-03-bp-shop-screen
description: "New WBP_ShopScreen: the shop moved to its own screen on an opaque black background, set on the controller."
parent: task-317-stats-then-cards
kind: task
status: done
---
# New WBP_ShopScreen: the shop moved to its own screen on an opaque black background, set on the controller

- goal: New WBP_ShopScreen: the shop moved to its own screen on an opaque black background, set on the controller. No new test (owner rule): only fix existing tests the change breaks.
- agent: bp
- depends_on: task-317-01-stats-then-cards, task-317-02-text-stats-labels
- inputs: wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), bp-rr-player-controller (vault/reference/tech/blueprints/player/bp-rr-player-controller.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: Content/RogueRigs/UI/WBP_ShopScreen (parent URRShopScreenWidget), BP_RRPlayerController ShopScreenWidgetClass set; new doc node wbp-shop-screen in vault/reference/tech/blueprints/ui/, bp-rr-player-controller doc updated
- done_when: compiled with warnings as errors and saved; opaque black backdrop (alpha 1); title, 5 WBP_ShopCard, ore balance, reroll button with cost, Continue calling Continue(); refresh on OnShopRefreshed; ShopScreenWidgetClass set on BP_RRPlayerController; doc nodes written; lint clean.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 106, cache write 94326, cache read 3271716, output 31487
- runs: 1
- milestone: M2
- result: WBP_ShopScreen (duplicate of WBP_Intermission reparented to URRShopScreenWidget): opaque black backdrop, SPARE PARTS title, 5 WBP_ShopCard, balance, reroll, Continue -> Continue(), refresh on OnShopRefreshed; ShopScreenWidgetClass set on BP_RRPlayerController; doc wbp-shop-screen; not run in PIE. Commit bad2c6b6.

## Relations
- [[task-317-stats-then-cards]] — parent global task.
