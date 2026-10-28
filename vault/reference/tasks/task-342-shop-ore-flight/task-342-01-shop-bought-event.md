---
id: task-342-01-shop-bought-event
description: "Cpp: URRIntermissionShopSubsystem broadcasts OnSlotBought(SlotIndex, Price, BalanceBefore)."
parent: task-342-shop-ore-flight
kind: task
status: done
---
# Shop purchase: ore flies from the balance to the Buy button: URRIntermissionShopSubsystem broadcasts OnSlotBought(SlotIndex, Price, BalanceBefore)

- goal: Add a BlueprintAssignable event OnSlotBought(int32 SlotIndex, int32 Price, int32 BalanceBefore) on URRIntermissionShopSubsystem, broadcast by BuySlot after a successful purchase and before OnShopChanged, so both the card click and rr.Shop.Buy start the purchase effects. No rule change.
- agent: cpp
- depends_on: none
- inputs: sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), run-shop (vault/reference/gdd/run/run-shop.md), agent-cpp (vault/reference/agents/agent-cpp.md), global task-342 `gdd_check`
- expected_output: C++ change in `Source/RogueRigs/Run/RRIntermissionShopSubsystem.*`, build green, change summary; one commit.
- done_when: build succeeds; a -game log or headless check shows OnSlotBought fired once per successful buy and never on a refused one; committed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 34, cache write 52285, cache read 593939, output 8072
- usage: turns 17, context start 11k peak 52k, resumes 0, 4 min, units 0.17 M
- runs: 1
- milestone: M2
- started: 2026-10-28T06:29:53Z
- waiting_for: none
- result: OnSlotBought(SlotIndex,Price,BalanceBefore) added to URRIntermissionShopSubsystem, broadcast in BuySlot before OnShopChanged; build green; capture ShopBoughtEvent: 4 events for 4 buys, none on 3 refusals; commit da3b826e

## Relations
- [[task-342-shop-ore-flight]] — parent global task.
