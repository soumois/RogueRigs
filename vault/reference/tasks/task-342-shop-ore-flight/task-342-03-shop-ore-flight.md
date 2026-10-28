---
id: task-342-03-shop-ore-flight
description: "Bp: WBP_ShopScreen flies ore icons from the balance to the Buy button, counts the balance down."
parent: task-342-shop-ore-flight
kind: task
status: in-progress
---
# Shop purchase: ore flies from the balance to the Buy button: WBP_ShopScreen flies ore icons from the balance to the Buy button, counts the balance down

- goal: WBP_ShopScreen: on OnSlotBought, flies the ore icons (Q7) from OreIcon to the BuyButton of ShopCard<SlotIndex> (geometry in screen space, widget tick on real time: the game is paused), counts OreBalanceText down from BalanceBefore as they land, blocks the shop buttons during the sequence (Q8) and calls EndPurchaseFx on the card when the last icon lands. Display only.
- agent: bp
- depends_on: task-342-02-shop-card-hold
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md) (pick animation: FlyStep, CenterAndSize, tick on real time), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-342 `gdd_check`
- expected_output: WBP_ShopScreen compiled (warnings as errors) and saved; change summary; one commit.
- done_when: compiled and saved; graphs read back (flight, count-down, button lock, EndPurchaseFx call); committed.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T06:38:48Z
- waiting_for: none
- result: open

## Relations
- [[task-342-shop-ore-flight]] — parent global task.
