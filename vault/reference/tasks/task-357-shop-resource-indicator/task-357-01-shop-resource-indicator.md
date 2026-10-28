---
id: task-357-01-shop-resource-indicator
description: "Bp: WBP_ShopScreen IndicatorRessource rebuilt without a Row container, ore icon and value centred on the font line."
parent: task-357-shop-resource-indicator
kind: task
status: in-progress
---
# Shop screen: resource indicator centred on its font: WBP_ShopScreen

- goal: in WBP_ShopScreen, inside IndicatorRessource (its own anchors set by task-356-01 stay): remove the CounterRow HorizontalBox and place OreIcon and OreBalanceText as decided in the global `## Questions` Q3, with explicit anchors and alignment, CounterBg stretched to the indicator area; icon size tied to the font size. Read the tree first and start from it (owner's edits committed by task-356-01). Keep names and graphs: CounterBg, OreIcon (variable, ore-flight start through CenterOf), OreBalanceText (written by RefreshShop and the flight count-down), FlyIcon0..9, every 342 to 344 graph. Few nesting levels, clear names.
- agent: bp
- depends_on: none
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), run-shop (vault/reference/gdd/run/run-shop.md), global task-357-shop-resource-indicator `## Questions`, umg-layout (vault/reference/tech/conventions/umg-layout.md) (the rule to apply), frames of task-356-03 in `Saved/ShopFix/task-356/` (look before)
- expected_output: WBP_ShopScreen compiled (warnings as errors) and saved; change summary (tree of IndicatorRessource before and after, anchors, alignment, sizes, font size); one commit.
- done_when: compiled and saved; no HorizontalBox left in IndicatorRessource; anchors read back; OreIcon and OreBalanceText still variables; graphs still present; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T21:37:43Z
- waiting_for: none
- result: open

## Relations
- [[task-357-shop-resource-indicator]] — parent global task.
