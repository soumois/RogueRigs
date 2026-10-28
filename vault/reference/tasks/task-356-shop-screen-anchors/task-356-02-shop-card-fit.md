---
id: task-356-02-shop-card-fit
description: "Bp: WBP_ShopCard scales to the anchored slot the shop screen gives it instead of a fixed 330 x 470 host."
parent: task-356-shop-screen-anchors
kind: task
status: todo
---
# Shop screen: Canvas and anchors: WBP_ShopCard fit

- goal: WBP_ShopCard: the outer fixed size (CardHost SizeBox 330 x 470) no longer forces the card size; the card fills the slot WBP_ShopScreen gives it (task-356-01) and keeps its ratio through its ScaleBox; price row and Buy plate stay under the card and readable. Card internals (CardFace 500 x 720, TierTab, Buy plate sizes, SoldPanel) untouched: they belong to task-355. Keep every name used by graphs: CardHost, CardScale, CardFace, BuyButton, BuyText, BuyFlash, SparkHost, Spark0..7, BurnOverlay, SoldPanel, TierTab, DimShade, PriceRow, OreIcon, PriceText and all variables, functions and events of tasks 342 to 344 and 353. Global `## Questions` Q2.
- agent: bp
- depends_on: task-356-01-shop-screen-anchors
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-356-shop-screen-anchors `## Questions`, change summary of task-356-01, umg-layout (vault/reference/tech/conventions/umg-layout.md) if Q5 is accepted
- expected_output: WBP_ShopCard compiled (warnings as errors) and saved; change summary (widgets and slots changed, values); one commit.
- done_when: compiled and saved; WBP_ShopScreen recompiles without error; changed slots read back; names and graphs still present; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-356-shop-screen-anchors]] — parent global task.
