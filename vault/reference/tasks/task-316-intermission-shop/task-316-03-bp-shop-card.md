---
id: task-316-03-bp-shop-card
description: "New WBP_ShopCard widget: one smaller priced upgrade card for the intermission shop (bp): upgrade shop between levels, step 03."
parent: task-316-intermission-shop
kind: task
status: done
---
# Shop card widget

- goal: Create /Game/RogueRigs/UI/WBP_ShopCard: one upgrade card with the same look as a WBP_UpgradeChoice stat card (baked frame, tier tint, stat icon, value, name, target, lore), sized so 5 fit in one row on a 1920 x 1080 screen with gaps (about 330 x 470), a price row below (ore icon T_Icon_Ore + price) and a buy button; functions SetCard(Card, Price, bAffordable), SetBought (empty slot look); greyed when unaffordable; no deal, suspense or fly animation (owner default Q9); display only, a click event the parent binds. New doc node shop card in tech/blueprints/ui.
- agent: bp
- depends_on: task-316-01-intermission-shop
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md, as updated by task-316-01); template vault/templates/blueprint.md
- expected_output: /Game/RogueRigs/UI/WBP_ShopCard, doc node wbp-shop-card listed in blueprints-ui-index
- done_when: compiled with warnings as errors and saved; tree and graphs read back; doc node created and linked; lint clean; committed
- editor_state: open
- token_ceiling: 50000
- model: sonnet
- tokens: input 142, cache write 171625, cache read 7383731, output 58090
- usage: turns 70, context start 10k peak 175k, resumes 1 (message x1), 15 min, units 1.24 M
- runs: 2
- milestone: M2
- result: WBP_ShopCard: 330x470 host scaling the WBP_UpgradeChoice stat card stack, price row (T_Icon_Ore + price), Buy button; SlotIndex var, SetCard, SetBought, RefreshFromShop, LoadLabels (screen_texts Labels 2-4), dispatcher OnBuyClicked; self-refreshing on OnShopChanged, Destruct unbinds only its own event; compiled warnings as errors, saved; doc wbp-shop-card; commits c36ffac4 dc853dcf; nothing seen in game

## Relations
- [[task-316-intermission-shop]] — parent global task.
