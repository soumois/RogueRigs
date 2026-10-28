---
id: task-348-06-shop-card-restyle
description: "Bp: WBP_ShopCard to the mockup: tier tab, violet price, styled Buy button with need-N-more, burnt sold look."
parent: task-348-shop-screen-redesign
kind: task
status: done
---
# Shop screen redesign: WBP_ShopCard

- goal: Restyle WBP_ShopCard after the mockup, display only: a tier tab (T_Shop_TierTab tinted by TintCard, label = tier name, in the tier colour, Q5) at the top of the card; a thin divider under the name; the target line kept (Q6); price always ore violet (0.85, 0.45, 1.0) (Q7); BuyButton style from T_Shop_ButtonBuy as a box brush with hover lighter, pressed darker, disabled grey by tint (Q3); label "BUY" or "NEED {n} MORE" with n = price minus balance (Q4); sold look (SetBought) = the stat frame in grey with T_Shop_SoldPanel, T_Shop_IconSold, "SOLD" and "NO REFUNDS." (Q10). Keep every variable, function, event and animation of tasks 342 to 344 (BuyButton, CardStack, FrameGroup, EndPurchaseFx, burn): only brushes, colours and new children change. Font Roboto Bold (Q8).
- agent: bp
- depends_on: task-348-04-shop-screen-texts, task-348-05-shop-sprites-import
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md) (tier colours), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md) (balance and price getters), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), the label index map in task-348-04 `result`, bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-348 `## Questions`
- expected_output: WBP_ShopCard compiled (warnings as errors) and saved; change summary; one commit.
- done_when: compiled and saved; tree, brushes and graphs read back (tab tint, need-N label, sold look); 342 to 344 nodes still present; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 112, cache write 156771, cache read 5780970, output 53527
- usage: turns 56, context start 11k peak 162k, resumes 0, 7 min, units 1.04 M
- runs: 1
- milestone: M2
- started: 2026-10-28T16:44:06Z
- waiting_for: none
- result: WBP_ShopCard restyled (commit fbbf8ca2), compiled and saved, no C++. Added TierTab (TierTabImg/TierTabText), NameDivider, SoldBox with SoldIcon/SoldSub; BuyButton box brush T_Shop_ButtonBuy with tints; functions ApplyShopLook (called from RefreshFromShop) and ApplySoldLook (end of SetBought); label defaults BUY/SOLD/NEED {n} MORE. 342-344 nodes kept. Tab plate tinted tier colour x0.45, label full tier colour (nuance on Q5). Unverified: no PIE/capture; NEED n runtime value, LEGENDARY fit, 342-344 fx.

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
